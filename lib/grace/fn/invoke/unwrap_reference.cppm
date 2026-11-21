export module grace.fn.invoke:unwrap_reference;

import std;

import grace.utility;

export namespace grace::fn::invoke {

template<typename F, typename... Args>
constexpr auto unwrap_reference(F &&f, Args &&...args)
    noexcept(noexcept(std::invoke(
        std::forward<F>(f),
        utility::get_reference(std::forward<Args>(args))...
    )))
    -> decltype(std::invoke(
        std::forward<F>(f),
        utility::get_reference(std::forward<Args>(args))...
    ))
{
    return std::invoke(
        std::forward<F>(f),
        utility::get_reference(std::forward<Args>(args))...
    );
}

} // namespace grace::fn::invoke
