include_directories(${ETVDB_SOURCE_DIR}/lib)
link_directories(${ETVDB_BINARY_DIR}/lib)

# don't build etvdb_test by default
#add_executable(etvdb_test etvdb_test.c)
#target_link_libraries(etvdb_test etvdb)

add_executable(etvdb_cli etvdb_cli.c)
target_link_libraries(etvdb_cli etvdb)

install(TARGETS etvdb_cli RUNTIME DESTINATION bin)
