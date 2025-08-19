module;

#include <concepts>

export module grace.iterator:remove_const;

export namespace grace::iterator {

// https://stackoverflow.com/a/10669041/10961484
template<typename Container, typename ConstIterator>
auto remove_const(Container& c, ConstIterator it)
    noexcept(noexcept(c.erase(it, it)))
    -> decltype(auto)
    requires requires { { c.erase(it, it) } -> std::same_as<typename Container::iterator>; }
{
    return c.erase(it, it);
}

} // namespace grace::iterator
