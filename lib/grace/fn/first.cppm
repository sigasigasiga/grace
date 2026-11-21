export module grace.fn:first;

import std;

export namespace grace::fn {

class first
{
public:
    constexpr first() = default;

public:
    [[nodiscard]] constexpr bool get() noexcept { return std::exchange(flag_, false); }
    [[nodiscard]] constexpr bool operator()() noexcept { return get(); }

private:
    bool flag_ = true;
};

} // namespace grace::fn
