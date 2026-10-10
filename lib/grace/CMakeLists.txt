add_subdirectory(bit)
add_subdirectory(compat)
add_subdirectory(concepts)
add_subdirectory(exception)
add_subdirectory(fn)
add_subdirectory(iterator)
add_subdirectory(memory)
add_subdirectory(meta)
add_subdirectory(ranges)
add_subdirectory(toy)
add_subdirectory(tuple)
add_subdirectory(type_traits)
add_subdirectory(utility)

add_library(grace)

target_sources(
    grace

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    module.cppm
)

target_link_libraries(
    grace

    PUBLIC

    grace.bit
    grace.compat
    grace.concepts
    grace.exception
    grace.fn
    grace.iterator
    grace.memory
    grace.meta
    grace.ranges
    grace.toy
    grace.tuple
    grace.type_traits
    grace.utility
)
