add_subdirectory(bind)
add_subdirectory(invoke)
add_subdirectory(op)
add_subdirectory(wrap)

add_library(grace.fn)

target_sources(
    grace.fn

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    first.cppm

    module.cppm
)

target_link_libraries(
    grace.fn

    grace.fn.bind
    grace.fn.invoke
    grace.fn.op
    grace.fn.wrap
)


target_compile_features(grace.fn PUBLIC cxx_std_26)
