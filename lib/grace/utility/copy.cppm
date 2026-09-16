export module grace.utility:copy;

export namespace grace::utility {

// works with explicit copy constructors
template<typename T>
[[nodiscard]] constexpr auto copy(T const &v)
    noexcept(noexcept(T(v)))
    -> decltype(T(v))
{
    return T(v);
}

} // namespace grace::utility
