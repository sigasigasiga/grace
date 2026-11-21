export module grace.fn.bind:lift_invocable;

import std;

export namespace grace::fn::bind {

template<auto Fn>
struct lift_invocable
{
    template<typename ...Args>
    static constexpr auto operator()(Args &&...args)
        noexcept(noexcept(std::invoke(Fn, std::forward<Args>(args)...)))
        -> decltype(std::invoke(Fn, std::forward<Args>(args)...))
    {
        return std::invoke(Fn, std::forward<Args>(args)...);
    }
};

} // namespace grace::fn::bind
