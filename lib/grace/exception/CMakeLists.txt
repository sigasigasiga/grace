add_library(grace.exception)

target_sources(
    grace.exception

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    storage.cppm

    module.cppm
)
