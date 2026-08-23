add_executable(read_only_value read_only_value.cpp)
target_link_libraries(read_only_value PRIVATE grace.utility)

add_test(
  NAME read_only_value
  COMMAND $<TARGET_FILE:read_only_value>
)
