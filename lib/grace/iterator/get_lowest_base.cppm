module;

#include <iterator>
#include <utility>

export module grace.iterator:get_lowest_base;

import grace.meta;
import grace.utility;

namespace get_lowest_base {

class adl_tag{};

template<typename FwdIt>
[[nodiscard]] constexpr auto impl(adl_tag, FwdIt &&it, grace::meta::overload_priority<0>)
    noexcept(noexcept(grace::utility::cut_rvalue_ref(std::forward<FwdIt>(it))))
    -> decltype(grace::utility::cut_rvalue_ref(std::forward<FwdIt>(it)))
{
    return grace::utility::cut_rvalue_ref(std::forward<FwdIt>(it));
}

template<typename FwdIt>
[[nodiscard]] constexpr auto impl(adl_tag, FwdIt &&it, grace::meta::overload_priority<1>)
    noexcept(noexcept(impl(adl_tag{}, std::forward<FwdIt>(it).base(), grace::meta::overload_priority<1>{})))
    -> decltype(impl(adl_tag{}, std::forward<FwdIt>(it).base(), grace::meta::overload_priority<1>{}))
{
    return impl(adl_tag{}, std::forward<FwdIt>(it).base(), grace::meta::overload_priority<1>{});
}

} // namespace get_lowest_base

export namespace grace::iterator {

template<typename FwdIt>
requires std::input_or_output_iterator<std::remove_cvref_t<FwdIt>>
[[nodiscard]] constexpr auto get_lowest_base(FwdIt &&it)
    noexcept(noexcept(get_lowest_base::impl(get_lowest_base::adl_tag{}, std::forward<FwdIt>(it), grace::meta::overload_priority<1>{})))
    -> decltype(get_lowest_base::impl(get_lowest_base::adl_tag{}, std::forward<FwdIt>(it), grace::meta::overload_priority<1>{}))
{
    return get_lowest_base::impl(get_lowest_base::adl_tag{}, std::forward<FwdIt>(it), grace::meta::overload_priority<1>{});
}

} // namespace grace::iterator
