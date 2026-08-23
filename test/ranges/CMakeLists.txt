# testing binary
add_executable(pointer_view pointer_view.cpp)
target_link_libraries(pointer_view PRIVATE grace.ranges)

# define tests
add_test(
  NAME pointer_view
  COMMAND $<TARGET_FILE:pointer_view>
)
