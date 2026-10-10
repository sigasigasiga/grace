add_library(grace.fn.invoke)

target_sources(
    grace.fn.invoke

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    fold.cppm
    ignore_args.cppm
    reverse_arguments.cppm
    sequence.cppm
    unwrap_reference.cppm

    module.cppm
)

target_link_libraries(
    grace.fn.invoke

    grace.tuple
    grace.utility
)

target_compile_features(grace.fn.invoke PUBLIC cxx_std_26)
