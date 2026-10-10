add_library(grace.type_traits)

target_sources(
    grace.type_traits

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    copy_cvref.cppm

    module.cppm
)

target_compile_features(grace.type_traits PUBLIC cxx_std_26)
