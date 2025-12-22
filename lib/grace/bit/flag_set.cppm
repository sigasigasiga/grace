module;

#include <type_traits>
#include <utility>

export module grace.bit:flag_set;

export namespace grace::util {

template<typename T>
requires std::is_enum_v<T>
class flag_set
{
public:
    using value_type = std::underlying_type_t<T>;

public:
    constexpr flag_set() = default;
    /* implicit */ constexpr flag_set(T value) noexcept : m_value{std::to_underlying(value)} {}

public:
    constexpr flag_set &operator&=(flag_set rhs) noexcept
    {
        m_value &= rhs.m_value;
        return *this;
    }

    constexpr flag_set &operator|=(flag_set rhs) noexcept
    {
        m_value |= rhs.m_value;
        return *this;
    }

    friend constexpr flag_set operator&(flag_set lhs, flag_set rhs) noexcept { return lhs &= rhs; }
    friend constexpr flag_set operator|(flag_set lhs, flag_set rhs) noexcept { return lhs |= rhs; }
    friend constexpr flag_set operator~(flag_set set) noexcept { return ~set.m_value; }

public:
    [[nodiscard]] constexpr value_type value() const noexcept { return m_value; }

    template<typename To>
    requires std::same_as<To, value_type> || std::same_as<To, bool>
    [[nodiscard]] explicit constexpr operator To() const noexcept
    {
        return value();
    }

private:
    value_type m_value;
};

} // namespace grace::util

using grace::util::flag_set;

export template<typename T>
struct std::hash<flag_set<T>>
{
    [[nodiscard]] constexpr static auto operator()(flag_set<T> set)
        noexcept(noexcept(std::hash<typename flag_set<T>::value_type>{}(set.value())))
        -> decltype(std::hash<typename flag_set<T>::value_type>{}(set.value()))
    {
        return std::hash<typename flag_set<T>::value_type>{}(set.value());
    }
};
