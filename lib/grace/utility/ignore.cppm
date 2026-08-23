export module grace.utility:ignore;

export namespace grace::utility {

class [[nodiscard]] ignore_t
{
public:
    constexpr const ignore_t &operator=(auto &&) const noexcept { return *this; }
    constexpr void operator()(auto &&...) const noexcept {}
};

inline constexpr ignore_t ignore;

} // namespace grace::utility
