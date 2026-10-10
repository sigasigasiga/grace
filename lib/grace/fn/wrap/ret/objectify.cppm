module;

#include <utility>

export module grace.fn.wrap.ret:objectify;

import grace.fn.bind;
import grace.fn.op;

export namespace grace::fn::wrap::ret {

template<typename F>
[[nodiscard]] constexpr auto objectify(F &&func)
    noexcept(noexcept(bind::pipe(std::forward<F>(func), op::objectify())))
    -> decltype(bind::pipe(std::forward<F>(func), op::objectify()))
{
    return bind::pipe(std::forward<F>(func), op::objectify());
}

} // namespace grace::fn::wrap::ret
