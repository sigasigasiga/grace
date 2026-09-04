module;

#include <functional>

export module grace.fn.bind:stored_func_invoker;

import grace.type_traits;
import grace.utility;

export namespace grace::fn::bind {

template<typename F>
class [[nodiscard]] stored_func_invoker : private utility::storage_base<F>
{
public:
    using utility::storage_base<F>::storage_base;

public:
    template<
        typename Self,
        typename... Args,
        typename USelf = type_traits::copy_cvref_t<Self &&, stored_func_invoker>
    >
    constexpr auto operator()(this Self &&self, Args &&...args)
        noexcept(noexcept(std::invoke(
            utility::private_base_cast<USelf>(self).value(),
            std::forward<Args>(args)...
        )))
        -> decltype(std::invoke(
            utility::private_base_cast<USelf>(self).value(),
            std::forward<Args>(args)...
        ))
    {
        return std::invoke(
            utility::private_base_cast<USelf>(self).value(),
            std::forward<Args>(args)...
        );
    }
};

template<typename F>
stored_func_invoker(F) -> stored_func_invoker<F>;

} // namespace grace::fn::bind
