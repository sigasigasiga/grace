module;

#include <memory>
#include <ranges>

export module grace.ranges:pointer_view;

import grace.memory;

export namespace grace::ranges {

// This class allows to create an owning view from a fancy pointer.
//
// It is better then `views::iota(0uz, size) | views::transform(grace::fn::bind::index_in(ptr))`
// because it only stores 2 pointers, while this combined view stores two `std::size_t`s and a ptr
template<typename Ptr, typename Sent>
requires
    std::move_constructible<Ptr> &&
    std::copy_constructible<Sent> &&
    requires(Ptr ptr, Sent sent) {
        memory::to_address_arr(ptr) == sent;
    }
class [[nodiscard]] pointer_view : public std::ranges::view_interface<pointer_view<Ptr, Sent>>
{
public:
    constexpr pointer_view(Ptr ptr, Sent sent)
        noexcept(
            std::is_nothrow_move_constructible_v<Ptr> &&
            std::is_nothrow_move_constructible_v<Sent>
        )
        : m_ptr{std::move(ptr)}
        , m_sent{std::move(sent)}
    {
    }

    constexpr pointer_view(Ptr ptr, std::size_t n)
        noexcept(std::is_nothrow_move_constructible_v<Ptr>)
        requires std::same_as<Sent, typename std::pointer_traits<Ptr>::element_type *>
        : m_ptr{std::move(ptr)}
        , m_sent{memory::to_address_arr(m_ptr) + n}
    {
    }

public:
    [[nodiscard]] constexpr auto begin() const noexcept { return memory::to_address_arr(m_ptr); }
    [[nodiscard]] constexpr auto end() const noexcept(std::is_nothrow_copy_constructible_v<Sent>) { return m_sent; }

private:
    Ptr m_ptr;
    Sent m_sent;
};

template<typename Ptr>
pointer_view(Ptr, std::size_t)
    -> pointer_view<Ptr, typename std::pointer_traits<Ptr>::element_type *>;

} // namespace grace::ranges
