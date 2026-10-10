add_library(grace.memory)

target_sources(
    grace.memory

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    get_deleter.cppm
    make_static_shared_ptr.cppm
    prepend_deleter.cppm
    shared_from_this_base.cppm
    to_address.cppm
    to_address_arr.cppm
    to_unique_ptr.cppm

    module.cppm
)

target_link_libraries(
    grace.memory

    grace.fn.bind
    grace.meta
    grace.type_traits
    grace.utility
)

target_compile_features(grace.memory PUBLIC cxx_std_26)
