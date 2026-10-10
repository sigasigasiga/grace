add_library(grace.ranges)

target_sources(
    grace.ranges

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    consign_view.cppm
    get_lowest_base.cppm
    make_subrange.cppm
    pointer_view.cppm

    module.cppm
)

target_link_libraries(
    grace.ranges

    grace.memory
    grace.meta
    grace.tuple
    grace.type_traits
    grace.utility
)

target_compile_features(grace.ranges PUBLIC cxx_std_26)
