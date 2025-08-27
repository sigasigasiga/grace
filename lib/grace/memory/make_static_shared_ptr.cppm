module;

#include <concepts>
#include <memory>

export module grace.memory:make_static_shared_ptr;

export namespace grace::memory {

// TODO: doesn't work on GCC. maybe it is not even complaint
#if 0
// Each unique call to this function produces a shared pointer to a static object,
// thanks to the lambda-parametrized non-type template parameter.
//
// https://stackoverflow.com/q/68798717/10961484
template<typename T, typename... Args, auto = [] {}>
requires std::constructible_from<T, Args &&...>
[[nodiscard]] std::shared_ptr<T> make_static_shared_ptr(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
{
    static T ret(std::forward<Args>(args)...);

    // Aliasing constructor takes a control block pointer from the first arg
    // and a data pointer from the second.
    //
    // By passing a default-constructed shared pointer to it,
    // we can get a shared pointer that'd behave as a regular pointer:
    // 1. It wouldn't own the underlying object
    // 2. It wouldn't increment atomic counter on copy
    //
    // Don't try to construct `weak_ptr` from this pointer though
    return std::shared_ptr<T>(std::shared_ptr<void>(), &ret);
}
#endif

} // namespace grace::memory
