module;

#include <tuple>
#include <type_traits>

export module grace.tuple:index_sequence_for_tuple;

export namespace grace::tuple {

// Easy and SFINAE-friendly way to make an index sequence
template<
    typename FwdTuple,
    typename Tuple = std::remove_reference_t<FwdTuple>,
    std::size_t Size = std::tuple_size<Tuple>::value>
[[nodiscard]] constexpr std::make_index_sequence<Size> index_sequence_for_tuple() noexcept
{
    return {};
}

template<typename FwdTuple>
[[nodiscard]] constexpr auto index_sequence_for_tuple(FwdTuple &&)
    noexcept
    -> decltype(index_sequence_for_tuple<FwdTuple>())
{
    return index_sequence_for_tuple<FwdTuple>();
}

} // namespace grace::tuple
