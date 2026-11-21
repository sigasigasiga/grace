export module grace.fn.bind:bind_expression;

import std;

import :stored_func_invoker;

export namespace grace::fn::bind {

template<typename F>
class [[nodiscard]] bind_expression : public stored_func_invoker<F>
{
public:
    using stored_func_invoker<F>::stored_func_invoker;
};

template<typename F>
bind_expression(F) -> bind_expression<F>;

// TODO: move to `fn.wrap`? which `wrap` tho: `wrap.args`/`wrap.ret`/`wrap`/`wrap.something`?
template<typename F>
[[nodiscard]] constexpr auto make_bind_expression(F &&fn)
    noexcept(noexcept(bind_expression(std::forward<F>(fn))))
    -> decltype(bind_expression(std::forward<F>(fn)))
{
    return bind_expression(std::forward<F>(fn));
}

} // namespace grace::fn::bind

export template<typename F>
class std::is_bind_expression<grace::fn::bind::bind_expression<F>> : public std::true_type
{};
