module;

#include <exception>
#include <type_traits>

export module grace.exception:storage;

export namespace grace::exception {

// like `std::exception_ptr` but:
// 1. has value semantics
// 2. cannot be null
// 3. stores exceptions that are convertible to `ExBase` by pointer (`ExBase` may be `void`)
template<typename ExBase>
class [[nodiscard]] storage
{
    static_assert(
        std::is_same_v<ExBase, std::remove_cvref_t<ExBase>>,
        "`ExBase` must be cvref-unqualified"
    );

public:
    template<typename FwdEx = ExBase, typename Ex = std::remove_cvref_t<FwdEx>>
    requires std::is_convertible_v<Ex *, ExBase *> &&
             (!std::is_same_v<Ex, storage>)
    explicit constexpr storage(FwdEx &&fwd_ex) noexcept
        : m_ep{std::make_exception_ptr(std::forward<FwdEx>(fwd_ex))}
    {
    }

    template<typename ExDerived>
    requires std::is_convertible_v<ExDerived *, ExBase *> &&
             (!std::is_same_v<ExDerived, ExBase>)
    constexpr storage(const storage<ExDerived> &rhs) noexcept
        : m_ep{rhs.get_exception_ptr()}
    {
    }

public:
    [[noreturn]] constexpr void throw_exception() const { std::rethrow_exception(m_ep); }
    [[nodiscard]] constexpr std::exception_ptr get_exception_ptr() const noexcept { return m_ep; }

private:
    std::exception_ptr m_ep;
};

template<typename Ex>
storage(Ex) -> storage<Ex>;

} // namespace grace::exception
