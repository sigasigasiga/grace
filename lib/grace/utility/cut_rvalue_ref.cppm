module;

#include <utility>

export module grace.utility:cut_rvalue_ref;

export namespace grace::utility {

// usage: `cut_rvalue_ref(std::forward<T>(val))`
template<typename T>
[[nodiscard]] constexpr auto cut_rvalue_ref(T &&value)
    noexcept(noexcept(static_cast<T>(std::forward<T>(value))))
    -> decltype(static_cast<T>(std::forward<T>(value)))
{
    // cut_rvalue_ref(0) -> cut_rvalue_ref<int>(int &&);
    // cut_rvalue_ref(x) -> cut_rvalue_ref<int &>(int &);
    //
    // so if the passed value is lvalue, we return the lvalue,
    // and if the passed value is rvalue, we return the value
    return static_cast<T>(std::forward<T>(value));
}

} // namespace grace::utility
