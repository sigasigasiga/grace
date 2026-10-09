module;

#include <concepts>
#include <iterator>

export module grace.iterator:rerase;

template<typename It, typename Container>
concept iter_for_container =
    std::same_as<It, typename Container::iterator> ||
    std::same_as<It, typename Container::const_iterator>
;

export namespace grace::iterator {

template<typename Container, iter_for_container<Container> It>
[[nodiscard]] constexpr auto rerase(
    Container &container,
    std::reverse_iterator<It> rit
)
    noexcept(noexcept(std::make_reverse_iterator(container.erase((++rit).base()))))
    -> decltype(std::make_reverse_iterator(container.erase((++rit).base())))
{
    return std::make_reverse_iterator(container.erase((++rit).base()));
}

template<typename Container, iter_for_container<Container> It>
[[nodiscard]] constexpr auto rerase(
    Container &container,
    std::reverse_iterator<It> rbegin,
    std::reverse_iterator<It> rend
)
    noexcept(noexcept(std::make_reverse_iterator(container.erase(rend.base(), rbegin.base()))))
    -> decltype(std::make_reverse_iterator(container.erase(rend.base(), rbegin.base())))
{
    return std::make_reverse_iterator(container.erase(rend.base(), rbegin.base()));
}

} // namespace grace::iterator
