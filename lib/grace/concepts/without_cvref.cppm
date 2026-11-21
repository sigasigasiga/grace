export module grace.concepts:without_cvref;

import std;

export namespace grace::concepts {

template<typename T>
concept without_cvref = std::same_as<std::remove_cvref_t<T>, T>;

} // namespace grace::concepts
