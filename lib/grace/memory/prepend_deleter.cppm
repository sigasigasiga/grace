export module grace.memory:prepend_deleter;

import std;

import :get_deleter;
import :to_unique_ptr;

import grace.fn.bind;
import grace.utility;

import grace.fn.invoke;

export namespace grace::memory {

// NB: if `D`'s move constructor throws, the program is terminated.
//     `std::unique_ptr`'s constructor has the same behavior
template<typename T, typename D, typename F>
constexpr auto prepend_deleter(std::unique_ptr<T, D> p, F &&fn)
    noexcept
    -> decltype((to_unique_ptr<T>)(
        p.release(),
        fn::bind::sequence(std::forward<F>(fn), (get_deleter)(std::move(p)))
    ))
{
    return (to_unique_ptr<T>)(
        p.release(),
        fn::bind::sequence(std::forward<F>(fn), (get_deleter)(std::move(p)))
    );
}

} // namespace grace::memory
