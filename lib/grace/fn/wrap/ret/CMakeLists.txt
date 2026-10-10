add_library(grace.fn.wrap.ret)

target_sources(
    grace.fn.wrap.ret

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    cut_rvalue_ref.cppm
    decay_copy.cppm
    objectify.cppm

    module.cppm
)

target_link_libraries(
    grace.fn.wrap.ret

    grace.fn.bind
    grace.fn.op
)

target_compile_features(grace.fn.wrap.ret PUBLIC cxx_std_26)
