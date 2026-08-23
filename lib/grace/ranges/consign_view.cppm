module;

#include <ranges>
#include <version>

export module grace.ranges:consign_view;

import grace.type_traits;
import grace.utility;

export namespace grace::ranges {

// similar to `boost::asio::consign`.
// originally i planned to make `pointer_view` which'd accept a fancy ptr and a sentinel for it
// then the `begin` would be implemented as `std::to_pointer(fancy_ptr_)` and `end` is just `sent_`
// but `std::to_pointer` is not available for `std::unique_ptr<T[]>`, so i came up with this
template<std::ranges::view View, typename Value>
class consign_view : public std::ranges::view_interface<consign_view<View, Value>>,
                     // standard requires that views must copy and move construction must be O(1)
                     // we cannot guarantee that copying of `Value` is O(1), so we have disable it
                     private utility::move_only
{
private:
    View m_view;
    Value m_value;

public:
    constexpr consign_view(View view, Value value)
        : m_view{std::move(view)}
        , m_value{std::move(value)}
    {
    }

public:
    template<typename Self, typename USelf = type_traits::copy_cvref_t<Self &&, consign_view>>
    constexpr auto base(this Self &&self)
        noexcept(noexcept(View(utility::private_base_cast<USelf>(self).m_view)))
        -> decltype(View(utility::private_base_cast<USelf>(self).m_view))
    {
        return View(utility::private_base_cast<USelf>(self).m_view);
    }

    // NB: `View::begin` and `View::end` may be mutable (e.g. `filter_view`)
    template<typename Self, typename USelf = type_traits::copy_cvref_t<Self &&, consign_view>>
    constexpr auto begin(this Self &&self)
        noexcept(noexcept(std::ranges::begin(utility::private_base_cast<USelf>(self).m_view)))
        -> decltype(std::ranges::begin(utility::private_base_cast<USelf>(self).m_view))
    {
        return std::ranges::begin(utility::private_base_cast<USelf>(self).m_view);
    }

    template<typename Self, typename USelf = type_traits::copy_cvref_t<Self &&, consign_view>>
    constexpr auto end(this Self &&self)
        noexcept(noexcept(std::ranges::end(utility::private_base_cast<USelf>(self).m_view)))
        -> decltype(std::ranges::end(utility::private_base_cast<USelf>(self).m_view))
    {
        return std::ranges::end(utility::private_base_cast<USelf>(self).m_view);
    }

    constexpr auto size() const
        noexcept(noexcept(std::ranges::size(this->m_view)))
        -> decltype(std::ranges::size(this->m_view))
    {
        return std::ranges::size(this->m_view);
    }
};

namespace views {

#if 0

template<typename Value>
class consign_closure : public std::ranges::range_adaptor_closure<consign_closure<Value>>
{
public:
    constexpr consign_closure(Value value)
        : value_{std::move(value)}
    {
    }

public:
    template<typename Self, std::ranges::view View>
    constexpr auto operator()(this Self &&self, View &&view)
        noexcept(noexcept(consign_view(std::forward<View>(view), std::forward<Self>(self).value_)))
        -> decltype(consign_view(std::forward<View>(view), std::forward<Self>(self).value_))
    {
        return consign_view(std::forward<View>(view), std::forward<Self>(self).value_);
    }

private:
    Value value_;
};

constexpr auto consign(auto value)
    noexcept(noexcept(consign_closure(std::move(value))))
    -> decltype(consign_closure(std::move(value)))
{
    return consign_closure(std::move(value));
}

#endif // __cpp_lib_ranges >= 202202L

} // namespace views

} // namespace grace::ranges
