include_directories(${ETVDB_SOURCE_DIR}/lib)
link_directories(${ETVDB_BINARY_DIR}/lib)

add_executable(etvdb_test etvdb_test.c)
target_link_libraries(etvdb_test etvdb)
