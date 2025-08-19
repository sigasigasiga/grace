add_library(grace.iterator)

target_sources(
    grace.iterator

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    get_lowest_base.cppm
    ostream_joiner.cppm
    remove_const.cppm
    rerase.cppm

    module.cppm
)

target_link_libraries(
    grace.iterator

    grace.meta
    grace.utility
)
