export module grace.fn.bind:sequence;

import std;

import grace.fn.invoke;

struct sequence_op
{
    template<typename ...Args>
    static constexpr auto operator()(Args &&...args)
        noexcept(noexcept(grace::fn::invoke::sequence(std::forward<Args>(args)...)))
        -> decltype(grace::fn::invoke::sequence(std::forward<Args>(args)...))
    {
        return grace::fn::invoke::sequence(std::forward<Args>(args)...);
    }
};

export namespace grace::fn::bind {

template<typename ...Fs>
[[nodiscard]] constexpr auto sequence(Fs &&...fs)
    noexcept(noexcept(std::bind_front(sequence_op{}, std::tuple(std::forward<Fs>(fs)...))))
    -> decltype(std::bind_front(sequence_op{}, std::tuple(std::forward<Fs>(fs)...)))
{
    return std::bind_front(sequence_op{}, std::tuple(std::forward<Fs>(fs)...));
}

} // namespace grace::fn::bind
