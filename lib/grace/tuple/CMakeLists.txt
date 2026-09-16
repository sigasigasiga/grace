add_library(grace.tuple)

target_sources(
    grace.tuple

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    for_each_in_tuple.cppm
    index_sequence_for_tuple.cppm
    make_tuple.cppm
    tuple_like.cppm

    module.cppm
)

target_link_libraries(
    grace.tuple

    grace.type_traits
)
