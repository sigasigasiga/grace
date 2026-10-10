add_library(grace.meta)

target_sources(
    grace.meta

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    dummy.cppm
    overload_priority.cppm
    string_literal.cppm
    tag.cppm
    type_list.cppm

    module.cppm
)

target_compile_features(grace.meta PUBLIC cxx_std_26)
