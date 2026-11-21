export module grace.fn.wrap:uo;

import std;

import grace.fn.wrap.args;
import grace.fn.wrap.ret;

export namespace grace::fn::wrap {

// TODO: come up with a better name
template<typename F>
constexpr auto uo(F&& f)
    noexcept(noexcept(ret::objectify(args::unwrap_reference(std::forward<F>(f)))))
    -> decltype(ret::objectify(args::unwrap_reference(std::forward<F>(f))))
{
    return ret::objectify(args::unwrap_reference(std::forward<F>(f)));
}

} // namespace grace::fn::wrap
