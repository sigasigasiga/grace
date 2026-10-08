add_executable(storage storage.cpp)
target_link_libraries(storage PRIVATE grace.exception)

add_test(
  NAME storage
  COMMAND $<TARGET_FILE:storage>
)
