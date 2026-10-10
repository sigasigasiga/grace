module;

#include <functional>

export module grace.fn.bind:lazy_eval;

import grace.type_traits;
import grace.utility;

export namespace grace::fn::bind {

template<typename F>
class [[nodiscard]] lazy_eval : private utility::storage_base<F>
{
public:
    using utility::storage_base<F>::storage_base;

private:
    template<typename Self, typename USelf = type_traits::copy_cvref_t<Self &&, lazy_eval>>
    constexpr static auto impl(Self &&self)
        noexcept(noexcept(std::invoke(utility::private_base_cast<USelf>(self).value())))
        -> decltype(std::invoke(utility::private_base_cast<USelf>(self).value()))
    {
        return std::invoke(utility::private_base_cast<USelf>(self).value());
    }

public:
    template<typename Self>
    [[nodiscard]] constexpr operator decltype((impl)(std::declval<Self>()))(this Self &&self)
        noexcept(noexcept((impl)(std::forward<Self>(self))))
    {
        return (impl)(std::forward<Self>(self));
    }

};

template<typename F>
lazy_eval(F) -> lazy_eval<F>;

} // namespace grace::fn::bind
