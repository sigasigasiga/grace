export module grace.fn.op:get;

import std;

import grace.tuple;

export namespace grace::fn::op {

template<typename T>
class [[nodiscard]] get_by_type
{
public:
    template<typename U> // do not require it to be tuple-like, as it may also be a variant
    requires(!requires(U &&v) { std::forward<U>(v).template get<T>(); })
    [[nodiscard]] static constexpr auto operator()(U &&val)
        noexcept(noexcept(get<T>(std::forward<U>(val))))
        -> decltype(get<T>(std::forward<U>(val)))
    {
        return get<T>(std::forward<U>(val));
    }

    template<typename U>
    [[nodiscard]] static constexpr auto operator()(U &&val)
        noexcept(noexcept(std::forward<U>(val).template get<T>()))
        -> decltype(std::forward<U>(val).template get<T>())
    {
        return std::forward<U>(val).template get<T>();
    }
};

template<auto V>
class [[nodiscard]] get_by_value
{
public:
    template<typename U> // do not require it to be tuple-like, as it may also be a variant
    requires(!requires(U &&v) { std::forward<U>(v).template get<V>(); })
    [[nodiscard]] static constexpr auto operator()(U &&val)
        noexcept(noexcept(get<V>(std::forward<U>(val))))
        -> decltype(get<V>(std::forward<U>(val)))
    {
        return get<V>(std::forward<U>(val));
    }

    template<typename U>
    [[nodiscard]] static constexpr auto operator()(U &&val)
        noexcept(noexcept(std::forward<U>(val).template get<V>()))
        -> decltype(std::forward<U>(val).template get<V>())
    {
        return std::forward<U>(val).template get<V>();
    }
};

template<typename T>
constexpr get_by_type<T> make_get() noexcept
{
    return {};
}

template<auto V>
constexpr get_by_value<V> make_get() noexcept
{
    return {};
}

// -------------------------------------------------------------------------------------------------

// same as `get_by_value` but also check if the type satisfies `pair_like`
class [[nodiscard]] get_key
{
public:
    template<tuple::pair_like FwdPair>
    [[nodiscard]] static constexpr auto operator()(FwdPair &&pair)
        noexcept(noexcept(get<0>(std::forward<FwdPair>(pair))))
        -> decltype(get<0>(std::forward<FwdPair>(pair)))
    {
        return get<0>(std::forward<FwdPair>(pair));
    }
};

class [[nodiscard]] get_value
{
public:
    template<tuple::pair_like FwdPair>
    [[nodiscard]] static constexpr auto operator()(FwdPair &&pair)
        noexcept(noexcept(get<1>(std::forward<FwdPair>(pair))))
        -> decltype(get<1>(std::forward<FwdPair>(pair)))
    {
        return get<1>(std::forward<FwdPair>(pair));
    }
};

} // namespace grace::fn::op
