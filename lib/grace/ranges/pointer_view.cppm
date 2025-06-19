module;

#include <memory>
#include <ranges>

export module grace.ranges:pointer_view;

import grace.memory;

export namespace grace::ranges {

// TODO: noexcept

// This class allows to create an owning view from a fancy pointer.
//
// It is better then `views::iota(0uz, size) | views::transform(grace::fn::bind::index_in(ptr))`
// because it only stores 2 pointers, while this combined view stores two `std::size_t`s and a ptr
template<typename Ptr, typename Sent>
requires requires(Ptr ptr, Sent sent) { memory::to_address_arr(ptr) == sent; }
class [[nodiscard]] pointer_view : public std::ranges::view_interface<pointer_view<Ptr, Sent>>
{
public:
    constexpr pointer_view(Ptr ptr, Sent sent)
        : m_ptr{std::move(ptr)}
        , m_sent{std::move(sent)}
    {
    }

    constexpr pointer_view(Ptr ptr, std::size_t n)
        : m_ptr{std::move(ptr)}
        , m_sent{memory::to_address_arr(m_ptr) + n}
    {
    }

public:
    [[nodiscard]] constexpr auto begin() const { return memory::to_address_arr(m_ptr); }
    [[nodiscard]] constexpr auto end() const { return m_sent; }

private:
    Ptr m_ptr;
    Sent m_sent;
};

template<typename Ptr>
pointer_view(Ptr, std::size_t)
    -> pointer_view<Ptr, typename std::pointer_traits<Ptr>::element_type *>;

} // namespace grace::ranges
