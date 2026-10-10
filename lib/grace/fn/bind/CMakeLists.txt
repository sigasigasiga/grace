add_library(grace.fn.bind)

target_sources(
    grace.fn.bind

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    bind.cppm
    bind_expression.cppm
    consign.cppm
    lazy_eval.cppm
    lift_function.cppm
    lift_invocable.cppm
    operators.cppm
    overload.cppm
    pipe.cppm
    return_value.cppm
    sequence.cppm
    stored_func_invoker.cppm

    module.cppm
)

target_link_libraries(
    grace.fn.bind

    grace.fn.invoke
    grace.fn.op
    grace.meta
    grace.type_traits
    grace.utility
)

target_compile_features(grace.fn.bind PUBLIC cxx_std_26)
