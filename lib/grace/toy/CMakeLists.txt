add_library(grace.toy)

target_sources(
    grace.toy

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    cvref_string_from_args.cppm
    print.cppm
    printing_constructor.cppm

    module.cppm
)

target_compile_features(grace.toy PUBLIC cxx_std_26)
