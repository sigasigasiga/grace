export module grace.fn.op:indirect;

import std;

export namespace grace::fn::op {

class [[nodiscard]] indirect
{
public:
    template<typename T>
    [[nodiscard]] static constexpr auto operator()(T &&object)
        noexcept(noexcept(*std::forward<T>(object)))
        -> decltype(*std::forward<T>(object))
    {
        return *std::forward<T>(object);
    }
};

} // namespace grace::fn::op
