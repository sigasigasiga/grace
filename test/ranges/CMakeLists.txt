# testing binary
add_executable(pointer_view pointer_view.cpp)
target_link_libraries(pointer_view PRIVATE grace.ranges)

# target names are global, `get_lowest_base` is taken by the iterator test
add_executable(ranges_get_lowest_base get_lowest_base.cpp)
target_link_libraries(ranges_get_lowest_base PRIVATE grace.ranges)

# define tests
add_test(
  NAME pointer_view
  COMMAND $<TARGET_FILE:pointer_view>
)

add_test(
  NAME ranges_get_lowest_base
  COMMAND $<TARGET_FILE:ranges_get_lowest_base>
)
