export module grace.fn.op:subscript;

import std;

export namespace grace::fn::op {

class [[nodiscard]] subscript
{
public:
    template<typename L, typename R>
    [[nodiscard]] static constexpr auto operator()(L &&lhs, R &&rhs)
        noexcept(noexcept(std::forward<L>(lhs)[std::forward<R>(rhs)]))
        -> decltype(std::forward<L>(lhs)[std::forward<R>(rhs)])
    {
        return std::forward<L>(lhs)[std::forward<R>(rhs)];
    }
};

} // namespace grace::fn::op
