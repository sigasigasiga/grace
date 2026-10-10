module;

#include <functional>
#include <utility>

export module grace.fn.bind:pipe;

import grace.fn.invoke;

struct fold_op
{
    template<typename... Args>
    static constexpr auto operator()(Args &&...args)
        noexcept(noexcept(grace::fn::invoke::fold(std::forward<Args>(args)...)))
        -> decltype(grace::fn::invoke::fold(std::forward<Args>(args)...))
    {
        return grace::fn::invoke::fold(std::forward<Args>(args)...);
    }
};

export namespace grace::fn::bind {

template<typename... Fs>
[[nodiscard]] constexpr auto pipe(Fs &&...fs)
    noexcept(noexcept(std::bind_front(fold_op{}, std::tuple{std::forward<Fs>(fs)...})))
    -> decltype(std::bind_front(fold_op{}, std::tuple{std::forward<Fs>(fs)...}))
{
    return std::bind_front(fold_op{}, std::tuple{std::forward<Fs>(fs)...});
}

} // namespace grace::fn::bind
