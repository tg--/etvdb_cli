INCLUDE(FindPkgConfig)
pkg_check_modules(ECORE-FILE REQUIRED ecore-file)

include_directories(${ETVDB_SOURCE_DIR}/lib ${ECORE-FILE_INCLUDE_DIRS})
link_directories(${ETVDB_BINARY_DIR}/lib)

# don't build etvdb_test by default
#add_executable(etvdb_test etvdb_test.c)
#target_link_libraries(etvdb_test etvdb)

add_executable(etvdb_cli etvdb_cli.c)
target_link_libraries(etvdb_cli etvdb ${ECORE-FILE_LIBRARIES})

install(TARGETS etvdb_cli RUNTIME DESTINATION bin)
