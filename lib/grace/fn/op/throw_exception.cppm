module;

#include <utility>

export module grace.fn.op:throw_exception;

export namespace grace::fn::op {

class [[nodiscard]] throw_exception
{
public:
    template<typename T>
    [[noreturn]] static constexpr void operator()(T &&value)
    {
        throw std::forward<T>(value);
    }
};

} // namespace grace::fn::op
