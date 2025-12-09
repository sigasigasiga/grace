module;

#include <concepts>
#include <tuple>

export module grace.tuple:tuple_like;

import grace.type_traits;
import :index_sequence_for_tuple;

// on cppreference, tuple protocol is described in the structured bindings article
// https://en.cppreference.com/w/cpp/language/structured_binding.html
//
// quote:
// For each structured binding, a variable whose type is
// "reference to std::tuple_element<I, E>::type" is introduced:
// lvalue reference if its corresponding initializer is an lvalue, rvalue reference otherwise.
template<
    typename ResultOfGet,
    std::size_t I,
    typename FwdTuple,
    typename Tuple = std::remove_reference_t<FwdTuple>,
    typename TupElement = std::tuple_element<I, Tuple>::type,
    typename TupElementRef = grace::type_traits::copy_ref_t<ResultOfGet &&, TupElement>>
concept convertible_to_tuple_element_ref = std::constructible_from<TupElementRef, ResultOfGet>;

template<std::size_t I, typename FwdTuple>
concept member_gettable = requires(FwdTuple &&tup) {
    { std::forward<FwdTuple>(tup).template get<I>() } -> convertible_to_tuple_element_ref<I, FwdTuple>;
};

template<std::size_t I, typename FwdTuple>
concept adl_gettable = requires(FwdTuple &&tup) {
    { get<I>(std::forward<FwdTuple>(tup)) } -> convertible_to_tuple_element_ref<I, FwdTuple>;
};

export namespace grace::tuple {

template<typename T>
concept tuple_like =
    []<std::size_t... Is>(std::index_sequence<Is...>) constexpr {
        // Notes:
        // 1. `get<Type>(tuple)` is not always available for `std::tuple`
        //    and is never available for `std::array` which is also tuple-like
        // 2. We can't access some elements using `t.get<I>()` and some elements using `get<I>(t)`,
        //    the type must support only one of these options for each of the fields.
        //    https://godbolt.org/z/c6TWssG14
        if constexpr((... || member_gettable<Is, T>)) {
            return (... && member_gettable<Is, T>);
        } else {
            return (... && adl_gettable<Is, T>);
        }
    }(
        (index_sequence_for_tuple<T>)()
    );

template<typename T>
concept pair_like = tuple_like<T> && std::tuple_size<std::remove_reference_t<T>>::value == 2;

} // namespace grace::tuple
