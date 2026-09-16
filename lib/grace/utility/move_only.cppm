export module grace.utility:move_only;

export namespace grace::utility {

class move_only
{
public:
    constexpr move_only() = default;

    constexpr move_only(move_only const &) = delete;
    constexpr move_only &operator=(move_only const &) = delete;

    constexpr move_only(move_only &&) = default;
    constexpr move_only &operator=(move_only &&) = default;
};

} // namespace grace::utility
