#include <concepts>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <version>

import grace.exception;

namespace {

using grace::exception::storage;

struct base {
    int value = 0;
};

struct derived : base {};

struct unrelated {};

struct private_derived : private base {};

// returns the message of the exception stored in `s`, or an empty string
// if the stored exception isn't a `std::exception`
template<typename ExBase>
std::string what_of(storage<ExBase> const &s)
{
    try {
        s.throw_exception();
    } catch (std::exception const &ex) {
        return ex.what();
    } catch (...) {
    }

    return {};
}

// checks that the dynamic type of the stored exception is exactly `Ex`
// (i.e. no slicing happened)
template<typename Ex, typename ExBase>
bool holds(storage<ExBase> const &s)
{
    try {
        s.throw_exception();
    } catch (Ex const &) {
        return true;
    } catch (...) {
    }

    return false;
}

template<typename S>
concept has_get_exception = requires(S const &s) { s.get_exception(); };

} // namespace

int main()
{
    // construction from an exception object is explicit
    static_assert(std::constructible_from<storage<std::exception>, std::runtime_error>);
    static_assert(!std::is_convertible_v<std::runtime_error, storage<std::exception>>);
    static_assert(std::is_nothrow_constructible_v<storage<std::exception>, std::runtime_error>);

    // only exceptions convertible to `ExBase` by pointer are accepted
    static_assert(std::constructible_from<storage<base>, base>);
    static_assert(std::constructible_from<storage<base>, derived>);
    static_assert(std::constructible_from<storage<base>, derived const &>);
    static_assert(!std::constructible_from<storage<base>, unrelated>);
    static_assert(!std::constructible_from<storage<base>, private_derived>);
    static_assert(!std::constructible_from<storage<derived>, base>);
    static_assert(!std::constructible_from<storage<int>, long>);

    // cannot be null
    static_assert(!std::is_default_constructible_v<storage<std::exception>>);
    static_assert(!std::constructible_from<storage<std::exception>, std::exception_ptr>);
    static_assert(!std::constructible_from<storage<std::exception>, std::nullptr_t>);

    // `storage<void>` accepts any exception type
    static_assert(std::constructible_from<storage<void>, int>);
    static_assert(std::constructible_from<storage<void>, std::runtime_error>);
    static_assert(std::constructible_from<storage<void>, unrelated>);

    // implicit conversion from a storage of a derived exception, but not the other way
    static_assert(std::is_convertible_v<storage<derived>, storage<base>>);
    static_assert(std::is_nothrow_constructible_v<storage<base>, storage<derived> const &>);
    static_assert(!std::constructible_from<storage<derived>, storage<base>>);
    static_assert(!std::constructible_from<storage<base>, storage<unrelated>>);
    static_assert(!std::constructible_from<storage<base>, storage<private_derived>>);
    static_assert(std::is_convertible_v<storage<int>, storage<void>>);
    static_assert(std::is_convertible_v<storage<std::runtime_error>, storage<void>>);
    static_assert(!std::constructible_from<storage<int>, storage<void>>);

    // copyable
    static_assert(std::is_copy_constructible_v<storage<std::exception>>);
    static_assert(std::is_copy_assignable_v<storage<std::exception>>);
    static_assert(std::is_nothrow_copy_constructible_v<storage<std::exception>>);
    static_assert(std::is_nothrow_copy_assignable_v<storage<std::exception>>);

    // CTAD
    static_assert(std::same_as<decltype(storage{std::runtime_error{""}}), storage<std::runtime_error>>);
    static_assert(std::same_as<decltype(storage{42}), storage<int>>);

    // accessors
    static_assert(std::same_as<decltype(std::declval<storage<base> const &>().get_exception_ptr()), std::exception_ptr>);
    static_assert(noexcept(std::declval<storage<base> const &>().get_exception_ptr()));

    // `throw_exception` rethrows the stored exception
    {
        storage<std::exception> s{std::runtime_error{"boom"}};

        if (!s.get_exception_ptr()) {
            throw "Unexpected null `exception_ptr`";
        }

        if (what_of(s) != "boom") {
            throw "Unexpected message of the rethrown exception";
        }
    }

    // the dynamic type of the exception is preserved (no slicing)
    {
        storage<std::exception> s{std::out_of_range{"oor"}};

        if (!holds<std::out_of_range>(s)) {
            throw "Unexpected dynamic type of the stored exception";
        }
    }

    // stored exception is a copy of the argument
    {
        derived d;
        d.value = 1;

        storage<base> s{d};
        d.value = 2;

        try {
            s.throw_exception();
        } catch (derived const &ex) {
            if (ex.value != 1) {
                throw "Stored exception is not a copy of the argument";
            }
        }
    }

    // braced-init of `ExBase` thanks to the default template argument
    {
        storage<base> s{{42}};

        try {
            s.throw_exception();
        } catch (base const &ex) {
            if (ex.value != 42) {
                throw "Unexpected value after braced-init construction";
            }
        }
    }

    // non-class exceptions
    {
        storage<int> s{42};

        try {
            s.throw_exception();
        } catch (int ex) {
            if (ex != 42) {
                throw "Unexpected value of the rethrown `int`";
            }
        }
    }

    // `storage<void>`
    {
        storage<void> s{std::runtime_error{"void"}};

        if (what_of(s) != "void") {
            throw "Unexpected message of the exception stored in `storage<void>`";
        }
    }

    // conversion from a storage of a derived exception shares the exception object
    {
        storage<std::runtime_error> derived_s{std::runtime_error{"conv"}};
        storage<std::exception> base_s = derived_s;

        if (base_s.get_exception_ptr() != derived_s.get_exception_ptr()) {
            throw "Converted storage does not share the exception object";
        }

        if (!holds<std::runtime_error>(base_s)) {
            throw "Unexpected dynamic type after conversion";
        }

        storage<void> void_s = base_s;

        if (void_s.get_exception_ptr() != derived_s.get_exception_ptr()) {
            throw "Converted `storage<void>` does not share the exception object";
        }
    }

    // direct-init from a non-const storage must convert, not wrap the storage itself
    {
        storage<derived> derived_s{derived{}};
        storage<base> base_s{derived_s};

        if (base_s.get_exception_ptr() != derived_s.get_exception_ptr()) {
            throw "Direct-init from a storage of a derived exception does not share the exception object";
        }

        storage<int> int_s{42};
        storage<void> void_s{int_s};

        if (void_s.get_exception_ptr() != int_s.get_exception_ptr()) {
            throw "Direct-init of `storage<void>` from a non-const storage wrapped the storage itself";
        }
    }

    // copy shares the exception object
    {
        storage<std::exception> a{std::runtime_error{"a"}};
        storage<std::exception> b = a;

        if (a.get_exception_ptr() != b.get_exception_ptr()) {
            throw "Copy does not share the exception object";
        }
    }

    // copy assignment
    {
        storage<std::exception> a{std::runtime_error{"a"}};
        storage<std::exception> b{std::logic_error{"b"}};

        b = a;

        if (a.get_exception_ptr() != b.get_exception_ptr()) {
            throw "Copy assignment does not share the exception object";
        }

        if (what_of(b) != "a") {
            throw "Unexpected message after copy assignment";
        }
    }

    // move is disabled: moving copies, so the source never becomes null
    {
        storage<std::exception> a{std::runtime_error{"moved"}};
        storage<std::exception> b = std::move(a);

        if (!a.get_exception_ptr()) {
            throw "Moved-from storage became null after move construction";
        }

        storage<std::exception> c{std::logic_error{"c"}};
        c = std::move(b);

        if (!b.get_exception_ptr()) {
            throw "Moved-from storage became null after move assignment";
        }

        if (what_of(c) != "moved") {
            throw "Unexpected message after move assignment";
        }
    }

#ifdef __cpp_lib_exception_ptr_cast

    // `get_exception` returns a reference to the stored exception
    {
        storage<std::exception> s{std::runtime_error{"get"}};

        static_assert(std::same_as<decltype(s.get_exception()), std::exception const &>);
        static_assert(noexcept(s.get_exception()));

        if (std::string{s.get_exception().what()} != "get") {
            throw "Unexpected message from `get_exception`";
        }

        if (dynamic_cast<std::runtime_error const *>(&s.get_exception()) == nullptr) {
            throw "Unexpected dynamic type from `get_exception`";
        }
    }

    // `get_exception` refers to the same object across copies
    {
        storage<std::exception> a{std::runtime_error{"same"}};
        storage<std::exception> b = a;

        if (&a.get_exception() != &b.get_exception()) {
            throw "`get_exception` refers to different objects across copies";
        }
    }

    // `get_exception` is unavailable for `storage<void>`
    static_assert(has_get_exception<storage<std::exception>>);
    static_assert(!has_get_exception<storage<void>>);

#endif // __cpp_lib_exception_ptr_cast
}
