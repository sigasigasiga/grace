module;

#include <memory>
#include <type_traits>
#include <utility>

export module grace.memory:to_unique_ptr;

export namespace grace::memory {

template<
    typename T,
    typename Ptr,
    typename FwdDel = std::default_delete<T>,
    typename Del = std::remove_cvref_t<FwdDel>
>
auto to_unique_ptr(Ptr ptr, FwdDel &&del = {})
    noexcept(noexcept(std::unique_ptr<T, Del>(ptr, std::forward<FwdDel>(del))))
    -> decltype(std::unique_ptr<T, Del>(ptr, std::forward<FwdDel>(del)))
{
    return std::unique_ptr<T, Del>(ptr, std::forward<FwdDel>(del));
}

template<
    typename T,
    typename Ptr,
    typename FwdDel = std::default_delete<T>,
    typename Del = std::remove_cvref_t<FwdDel>
>
requires std::is_array_v<T>
auto to_unique_ptr(Ptr ptr, FwdDel &&del = {})
    noexcept(noexcept(std::unique_ptr<T, Del>(ptr, std::forward<FwdDel>(del))))
    -> decltype(std::unique_ptr<T, Del>(ptr, std::forward<FwdDel>(del)))
{
    return std::unique_ptr<T, Del>(ptr, std::forward<FwdDel>(del));
}

} // namespace grace::memory
