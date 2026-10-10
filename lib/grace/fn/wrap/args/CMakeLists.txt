add_library(grace.fn.wrap.args)

target_sources(
    grace.fn.wrap.args

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    apply.cppm
    ignore.cppm
    unwrap_reference.cppm

    module.cppm
)

target_link_libraries(
    grace.fn.wrap.args

    grace.fn.invoke
)

target_compile_features(grace.fn.wrap.args PUBLIC cxx_std_26)
