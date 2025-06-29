module;

#include <functional>
#include <utility>

export module grace.fn.wrap.args:ignore;

import grace.fn.invoke;

constexpr auto ignore_args_op = []<typename... Args>
    (Args &&...args)
    noexcept(noexcept(grace::fn::invoke::ignore_args(std::forward<Args>(args)...)))
    -> decltype(grace::fn::invoke::ignore_args(std::forward<Args>(args)...))
{
    return grace::fn::invoke::ignore_args(std::forward<Args>(args)...);
};

export namespace grace::fn::wrap::args {

template<std::invocable F>
[[nodiscard]] constexpr auto ignore(F &&fn)
    noexcept(noexcept(std::bind_front(ignore_args_op, std::forward<F>(fn))))
    -> decltype(std::bind_front(ignore_args_op, std::forward<F>(fn)))
{
    return std::bind_front(ignore_args_op, std::forward<F>(fn));
}

} // namespace grace::fn::wrap::args
