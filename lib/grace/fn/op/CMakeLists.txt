add_library(grace.fn.op)

target_sources(
    grace.fn.op

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    cast.cppm
    construct.cppm
    copy.cppm
    cut_rvalue_ref.cppm
    decay_copy.cppm
    free.cppm
    get.cppm
    get_reference.cppm
    indirect.cppm
    objectify.cppm
    ref.cppm
    subscript.cppm
    throw_exception.cppm

    module.cppm
)

target_link_libraries(
    grace.fn.op

    grace.tuple
    grace.utility
)

target_compile_features(grace.fn.op PUBLIC cxx_std_26)
