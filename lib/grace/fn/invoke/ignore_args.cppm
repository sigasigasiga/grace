export module grace.fn.invoke:ignore_args;

import std;

export namespace grace::fn::invoke {

template<typename F, typename... Args>
constexpr auto ignore_args(F &&f, Args &&...)
    noexcept(noexcept(std::invoke(std::forward<F>(f))))
    -> decltype(std::invoke(std::forward<F>(f)))
{
    return std::invoke(std::forward<F>(f));
}

} // namespace grace::fn::invoke
