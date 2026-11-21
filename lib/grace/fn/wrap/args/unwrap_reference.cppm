export module grace.fn.wrap.args:unwrap_reference;

import std;

import grace.fn.invoke;

struct unwrap_reference_op
{
    template<typename... Args>
    static constexpr auto operator()(Args &&...args)
        noexcept(noexcept(grace::fn::invoke::unwrap_reference(std::forward<Args>(args)...)))
        -> decltype(grace::fn::invoke::unwrap_reference(std::forward<Args>(args)...))
    {
        return grace::fn::invoke::unwrap_reference(std::forward<Args>(args)...);
    }
};

export namespace grace::fn::wrap::args {

template<typename F>
[[nodiscard]] constexpr auto unwrap_reference(F &&fn)
    noexcept(noexcept(std::bind_front(unwrap_reference_op{}, std::forward<F>(fn))))
    -> decltype(std::bind_front(unwrap_reference_op{}, std::forward<F>(fn)))
{
    return std::bind_front(unwrap_reference_op{}, std::forward<F>(fn));
}

} // namespace grace::fn::wrap::args
