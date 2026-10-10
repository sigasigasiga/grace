module;

#include <utility>

export module grace.fn.wrap.ret:cut_rvalue_ref;

import grace.fn.bind;
import grace.fn.op;

export namespace grace::fn::wrap::ret {

// useful for `tl::optional::transform`,
// although i think it should've been replacing `T &&` with `T` automatically
template<typename F>
[[nodiscard]] constexpr auto cut_rvalue_ref(F &&func)
    noexcept(noexcept(bind::pipe(std::forward<F>(func), op::cut_rvalue_ref())))
    -> decltype(bind::pipe(std::forward<F>(func), op::cut_rvalue_ref()))
{
    return bind::pipe(std::forward<F>(func), op::cut_rvalue_ref());
}

} // namespace grace::fn::wrap::ret
