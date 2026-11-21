export module grace.meta:overload_priority;

import std;

export namespace grace::meta {

// https://stackoverflow.com/a/39905208
template<std::uintmax_t P>
class overload_priority : public overload_priority<P - 1>
{};

template<>
class overload_priority<0>
{};

} // namespace grace::meta
