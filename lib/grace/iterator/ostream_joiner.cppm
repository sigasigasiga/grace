module;

#include <concepts>
#include <string>
#include <utility>

export module grace.iterator:ostream_joiner;

export namespace grace::iterator {

template<typename DelimT, typename CharT = char, typename Traits = std::char_traits<CharT>>
requires std::copy_constructible<DelimT> || std::move_constructible<DelimT>
class [[nodiscard]] ostream_joiner
{
public:
    using char_type = CharT;
    using traits_type = Traits;
    using ostream_type = std::basic_ostream<CharT, Traits>;
    using value_type = void;
    using difference_type = std::ptrdiff_t; // cppreference states that this should be `void` but
                                            // `std::ranges` algorithms do not work with that
    using pointer = void;
    using reference = void;
    using iterator_category = std::output_iterator_tag;

public:
    ostream_joiner(ostream_type &os, DelimT const &delim)
        : m_os{std::addressof(os)}
        , m_delim{delim}
        , m_first{true}
    {
    }

    ostream_joiner(ostream_type &os, DelimT &&delim)
        : m_os{std::addressof(os)}
        , m_delim{std::move(delim)}
        , m_first{true}
    {
    }

    ostream_joiner(ostream_joiner const &) = default;
    ostream_joiner(ostream_joiner &&) = default;

    ostream_joiner &operator=(ostream_joiner const &) = default;
    ostream_joiner &operator=(ostream_joiner &&) = default;

public:
    template<typename T>
    ostream_joiner &operator=(T const &value)
    {
        if (!std::exchange(m_first, false)) {
            *m_os << m_delim;
        }

        *m_os << value;

        return *this;
    }

    ostream_joiner &operator*() noexcept { return *this; }

    ostream_joiner &operator++() noexcept { return *this; }

    ostream_joiner &operator++(int) noexcept { return *this; }

private:
    ostream_type *m_os;
    DelimT m_delim;
    bool m_first;
};

template<typename CharT, typename Traits, typename DelimT>
auto make_ostream_joiner(std::basic_ostream<CharT, Traits> &os, DelimT &&delim)
{
    return ostream_joiner<std::decay_t<DelimT>, CharT, Traits>{os, std::forward<DelimT>(delim)};
}

} // namespace grace::iterator
