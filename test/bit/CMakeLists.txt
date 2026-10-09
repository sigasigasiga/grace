# testing binary
add_executable(make_bitset make_bitset.cpp)
target_link_libraries(make_bitset PRIVATE grace.bit)

add_executable(flag_set flag_set.cpp)
target_link_libraries(flag_set PRIVATE grace.bit)

# define tests
add_test(
  NAME make_bitset
  COMMAND $<TARGET_FILE:make_bitset>
)

add_test(
  NAME flag_set
  COMMAND $<TARGET_FILE:flag_set>
)
