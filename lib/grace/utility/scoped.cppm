export module grace.utility:scoped;

export namespace grace::utility {

class scoped
{
public:
    constexpr scoped() = default;

    constexpr scoped(scoped const &) = delete;
    constexpr scoped &operator=(scoped const &) = delete;

    constexpr scoped(scoped &&) = delete;
    constexpr scoped &operator=(scoped &&) = delete;
};

} // namespace grace::utility
