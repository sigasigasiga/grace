add_executable(read_only_value read_only_value.cpp)
target_link_libraries(read_only_value PRIVATE grace.utility)

add_executable(default_assign default_assign.cpp)
target_link_libraries(default_assign PRIVATE grace.utility)

add_test(
  NAME read_only_value
  COMMAND $<TARGET_FILE:read_only_value>
)

add_test(
  NAME default_assign
  COMMAND $<TARGET_FILE:default_assign>
)
