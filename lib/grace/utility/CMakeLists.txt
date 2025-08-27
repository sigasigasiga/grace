add_library(grace.utility)

target_sources(
    grace.utility

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    cctype.cppm
    copy.cppm
    cut_rvalue_ref.cppm
    decay_copy.cppm
    default_assign.cppm
    get_reference.cppm
    ignore.cppm
    implicit_cast.cppm
    move_only.cppm
    objectify.cppm
    pick_overload.cppm
    private_base_cast.cppm
    read_only_value.cppm
    scoped.cppm
    storage_base.cppm

    module.cppm
)

target_link_libraries(
    grace.utility

    grace.type_traits
)
