module;

#include <functional>

export module grace.fn.bind:consign;

import grace.type_traits;
import grace.utility;

template<typename F, typename Consignment>
class consign_impl
{
public:
    template<
        std::convertible_to<F> FwdF,
        std::convertible_to<Consignment> FwdConsignment>
    constexpr consign_impl(FwdF &&fn, FwdConsignment &&consignment)
        noexcept(
            std::is_nothrow_convertible_v<FwdF &&, F> &&
            std::is_nothrow_convertible_v<FwdConsignment &&, Consignment>
        )
        : m_fn(std::forward<FwdF>(fn))
        , m_consignment(std::forward<FwdConsignment>(consignment))
    {
    }

public:
    template<
        typename Self,
        typename ...Args,
        typename FwdSelf = grace::type_traits::copy_cvref_t<Self &&, consign_impl>>
    constexpr auto operator()(this Self &&self, Args &&...args)
        noexcept(noexcept(std::invoke(
            grace::utility::private_base_cast<FwdSelf>(self).m_fn,
            std::forward<Args>(args)...)
        ))
        -> decltype(std::invoke(
            grace::utility::private_base_cast<FwdSelf>(self).m_fn,
            std::forward<Args>(args)...)
        )
    {
        return std::invoke(
            grace::utility::private_base_cast<FwdSelf>(self).m_fn,
            std::forward<Args>(args)...
        );
    }

private:
    [[no_unique_address]] F m_fn;
    Consignment m_consignment;
};

template<typename F, typename Consignment>
consign_impl(F, Consignment) -> consign_impl<F, Consignment>;

export namespace grace::fn::bind {

template<typename F, typename Consignment>
[[nodiscard]] constexpr auto consign(F &&fn, Consignment &&consignment)
    noexcept(noexcept(consign_impl(std::forward<F>(fn), std::forward<Consignment>(consignment))))
    -> decltype(consign_impl(std::forward<F>(fn), std::forward<Consignment>(consignment)))
{
    return consign_impl(std::forward<F>(fn), std::forward<Consignment>(consignment));
}

} // namespace grace::fn::bind
