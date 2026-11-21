export module grace.fn.wrap.args:apply;

import std;

struct apply_op
{
    template <typename F, typename Tuple>
    static constexpr auto operator()(F &&f, Tuple &&tuple)
        noexcept(noexcept(std::apply(std::forward<F>(f), std::forward<Tuple>(tuple))))
        -> decltype(std::apply(std::forward<F>(f), std::forward<Tuple>(tuple)))
    {
        return std::apply(std::forward<F>(f), std::forward<Tuple>(tuple));
    }
};

export namespace grace::fn::wrap::args {

template<typename F>
[[nodiscard]] constexpr auto apply(F &&f)
    noexcept(noexcept(std::bind_front(apply_op{}, std::forward<F>(f))))
    -> decltype(std::bind_front(apply_op{}, std::forward<F>(f)))
{
    return std::bind_front(apply_op{}, std::forward<F>(f));
}

} // namespace grace::fn::wrap::args
