add_library(grace.fn.bind)

target_sources(
    grace.fn.bind

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    bind.cppm
    bind_expression.cppm
    compose.cppm
    consign.cppm
    lazy_eval.cppm
    lift_function.cppm
    lift_invocable.cppm
    operators.cppm
    overload.cppm
    return_value.cppm
    sequence.cppm
    stored_func_invoker.cppm

    module.cppm
)

target_link_libraries(
    grace.fn.bind

    grace.compat
    grace.fn.invoke
    grace.fn.op
    grace.meta
    grace.type_traits
    grace.utility
)
