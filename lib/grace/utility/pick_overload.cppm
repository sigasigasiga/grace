export module grace.utility:pick_overload;

export namespace grace::utility {

// If a member function has both const and non-const overloads, cast it to the right one.

// TODO: should we also have an overload for class fields?
template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_const_overload(
    Ret (T::*fptr)(Args ...) const noexcept(Noexcept)
) noexcept {
    return fptr;
}

template<typename Ret, typename T, typename ...Args, bool Noexcept>
[[nodiscard]] constexpr auto pick_mutable_overload(
    Ret (T::*fptr)(Args ...) noexcept(Noexcept)
) noexcept {
    return fptr;
}

} // namespace grace::utility
