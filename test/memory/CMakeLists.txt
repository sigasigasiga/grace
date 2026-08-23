# testing binary
add_executable(prepend_deleter prepend_deleter.cpp)
target_link_libraries(prepend_deleter PRIVATE grace.memory)

add_executable(to_address to_address.cpp)
target_link_libraries(to_address PRIVATE grace.memory)

add_executable(shared_from_this_base shared_from_this_base.cpp)
target_link_libraries(shared_from_this_base PRIVATE grace.memory)

# define tests
add_test(
  NAME prepend_deleter
  COMMAND $<TARGET_FILE:prepend_deleter>
)

add_test(
  NAME to_address
  COMMAND $<TARGET_FILE:to_address>
)

add_test(
  NAME shared_from_this_base
  COMMAND $<TARGET_FILE:shared_from_this_base>
)
