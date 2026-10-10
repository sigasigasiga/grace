add_library(grace.concepts)

target_sources(
    grace.concepts

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    conceptify.cppm
    without_cvref.cppm

    module.cppm
)

target_compile_features(grace.concepts PUBLIC cxx_std_26)
