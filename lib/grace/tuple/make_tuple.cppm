module;

#include <tuple>

export module grace.tuple:make_tuple;

export namespace grace::tuple {

// like `std::make_tuple` but SFINAE-friendly
template<typename ...Args>
[[nodiscard]] constexpr auto make_tuple(Args &&...args)
    noexcept(noexcept(std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)))
    -> decltype(std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...))
{
    return std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...);
}

} // namespace grace::tuple
