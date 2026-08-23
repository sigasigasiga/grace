# testing binary
add_executable(clamp_to clamp_to.cpp)
target_link_libraries(clamp_to PRIVATE grace.algorithm)

# define tests
add_test(
  NAME clamp_to
  COMMAND $<TARGET_FILE:clamp_to>
)
