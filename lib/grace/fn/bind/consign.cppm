module;

#include <functional>

export module grace.fn.bind:consign;

export namespace grace::fn::bind {

template<typename F, typename ...Consignment>
[[nodiscard]] constexpr auto consign(F &&f, Consignment &&...consignment)
    // FIXME: noexcept
    // FIXME: SFINAE-friendliness
{
    return [
        f = std::forward<F>(f), // FIXME: no EBCO
        ..._ = std::forward<Consignment>(consignment)
    ] <
        typename Self,
        typename ...Args
    > (
        this Self &&,
        Args &&...args
    )
        noexcept(noexcept(std::invoke(std::forward_like<Self>(f), std::forward<Args>(args)...)))
        -> decltype(std::invoke(std::forward_like<Self>(f), std::forward<Args>(args)...))
    {
        return std::invoke(std::forward_like<Self>(f), std::forward<Args>(args)...);
    };
}

} // namespace grace::fn::bind
