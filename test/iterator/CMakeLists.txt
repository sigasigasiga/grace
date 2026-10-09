# testing binary
add_executable(rerase rerase.cpp)
target_link_libraries(rerase PRIVATE grace.iterator)

# define tests
add_test(
  NAME rerase
  COMMAND $<TARGET_FILE:rerase>
)
