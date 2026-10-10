add_subdirectory(args)
add_subdirectory(ret)

add_library(grace.fn.wrap)

target_sources(
    grace.fn.wrap

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    uo.cppm

    module.cppm
)

target_link_libraries(
    grace.fn.wrap

    grace.fn.wrap.args
    grace.fn.wrap.ret
)

target_compile_features(grace.fn.wrap PUBLIC cxx_std_26)
