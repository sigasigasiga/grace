export module grace.fn.op:ref;

import std;

export namespace grace::fn::op {

// `std::ref` that is created from an rvalue reference is *not* SFINAE-out.
// Even though I don't like that, I prefer to be consistent with STL.

class ref
{
public:
    template<typename T>
    [[nodiscard]] constexpr static auto operator()(T&& value) noexcept
    {
        return std::ref(std::forward<T>(value));
    }
};

class cref
{
public:
    template<typename T>
    [[nodiscard]] constexpr static auto operator()(T&& value) noexcept
    {
        return std::cref(std::forward<T>(value));
    }
};

} // namespace grace::fn::op
