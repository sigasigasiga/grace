# testing binary
add_executable(get_lowest_base get_lowest_base.cpp)
target_link_libraries(get_lowest_base PRIVATE grace.iterator)

add_executable(rerase rerase.cpp)
target_link_libraries(rerase PRIVATE grace.iterator)

# define tests
add_test(
  NAME get_lowest_base
  COMMAND $<TARGET_FILE:get_lowest_base>
)

add_test(
  NAME rerase
  COMMAND $<TARGET_FILE:rerase>
)
