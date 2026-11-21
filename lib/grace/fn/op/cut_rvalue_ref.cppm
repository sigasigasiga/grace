export module grace.fn.op:cut_rvalue_ref;

import std;

import grace.utility;

export namespace grace::fn::op {

class [[nodiscard]] cut_rvalue_ref
{
public:
    template<typename T>
    [[nodiscard]] static constexpr auto operator()(T &&value)
        noexcept(noexcept(utility::cut_rvalue_ref(std::forward<T>(value))))
        -> decltype(utility::cut_rvalue_ref(std::forward<T>(value)))
    {
        return utility::cut_rvalue_ref(std::forward<T>(value));
    }
};

} // namespace grace::fn::op
