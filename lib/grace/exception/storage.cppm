module;

#include <concepts>
#include <exception>
#include <type_traits>
#include <version>

export module grace.exception:storage;

export namespace grace::exception {

// like `std::exception_ptr` but:
// 1. has reference semantics
// 2. cannot be null
// 3. stores exceptions that are convertible to `ExBase` by pointer (`ExBase` may be `void`)
template<typename ExBase>
class [[nodiscard]] storage
{
    static_assert(
        std::is_same_v<ExBase, std::remove_cvref_t<ExBase>>,
        "`ExBase` must be cvref-unqualified"
    );

    template<typename E>
    consteval static bool is_exception_storage(storage<E> const volatile *) { return true; }
    consteval static bool is_exception_storage(...) { return false; }

public:
    // FIXME: `std::make_exception_ptr` may return a pointer to `std::bad_alloc` or `std::bad_exception`
    // gotta fix it by querying the exception stored inside the pointer with `std::exception_ptr_cast`
    // when it is available in libc++
    template<typename FwdEx = ExBase, typename Ex = std::remove_cvref_t<FwdEx>>
    requires std::is_convertible_v<Ex *, ExBase *> &&
             (!is_exception_storage(static_cast<Ex *>(nullptr)))
    explicit constexpr storage(FwdEx &&fwd_ex)
        noexcept(std::is_nothrow_constructible_v<Ex, FwdEx &&>)
        : m_ep{std::make_exception_ptr(std::forward<FwdEx>(fwd_ex))}
    {
    }

    template<typename ExDerived>
    requires std::is_convertible_v<ExDerived *, ExBase *> &&
             (!std::is_same_v<ExDerived, ExBase>)
    constexpr storage(storage<ExDerived> const &rhs) noexcept
        : m_ep{rhs.get_exception_ptr()}
    {
    }

    // disable move
    constexpr storage(storage const &rhs) = default;
    constexpr storage &operator=(storage const &rhs) = default;

public:
    [[noreturn]] constexpr void throw_exception() const { std::rethrow_exception(m_ep); }
    [[nodiscard]] constexpr std::exception_ptr get_exception_ptr() const noexcept { return m_ep; }

#ifdef __cpp_lib_exception_ptr_cast

    template<std::same_as<ExBase> E = ExBase>
    requires (!std::is_void_v<E>)
    [[nodiscard]] constexpr auto get_exception() const
        noexcept
        -> E const &
    {
        return *std::exception_ptr_cast<ExBase>(m_ep);
    }

#endif // __cpp_lib_exception_ptr_cast

private:
    std::exception_ptr m_ep;
};

template<typename Ex>
storage(Ex) -> storage<Ex>;

} // namespace grace::exception
