export module grace.fn.op:free;

import std;

export namespace grace::fn::op {

class free
{
public:
    static void operator()(void *ptr) noexcept { std::free(ptr); }
};

} // namespace grace::fn::op
