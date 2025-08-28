export module grace.utility:pick_overload;

export namespace grace::utility {

// If a member function has both const and non-const overloads, cast it to the right one.
// STL usually ignores `volatile` qualifier, so I decided to do the same

// TODO: should we also have an overload for class fields?

// lvalue + const
template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_lvalue_const_overload(
    Ret (T::*fptr)(Args ...) const noexcept(Noexcept)
) noexcept {
    return fptr;
}

template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_lvalue_const_overload(
    Ret (T::*fptr)(Args ...) const & noexcept(Noexcept)
) noexcept {
    return fptr;
}

// rvalue + const
template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_rvalue_const_overload(
    Ret (T::*fptr)(Args ...) const noexcept(Noexcept)
) noexcept {
    return fptr;
}

template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_rvalue_const_overload(
    Ret (T::*fptr)(Args ...) const && noexcept(Noexcept)
) noexcept {
    return fptr;
}

// lvalue + mutable
template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_lvalue_mutable_overload(
    Ret (T::*fptr)(Args ...) noexcept(Noexcept)
) noexcept {
    return fptr;
}

template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_lvalue_mutable_overload(
    Ret (T::*fptr)(Args ...) & noexcept(Noexcept)
) noexcept {
    return fptr;
}

// rvalue + mutable
template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_rvalue_mutable_overload(
    Ret (T::*fptr)(Args ...) noexcept(Noexcept)
) noexcept {
    return fptr;
}

template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_rvalue_mutable_overload(
    Ret (T::*fptr)(Args ...) && noexcept(Noexcept)
) noexcept {
    return fptr;
}

} // namespace grace::utility
