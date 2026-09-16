export module grace.fn.op:copy;

import grace.utility;

export namespace grace::fn::op {

class [[nodiscard]] copy
{
public:
    template<typename T>
    [[nodiscard]] static constexpr auto operator()(T const &v)
        noexcept(noexcept(utility::copy(v)))
        -> decltype(utility::copy(v))
    {
        return utility::copy(v);
    }
};

} // namespace grace::fn::op
