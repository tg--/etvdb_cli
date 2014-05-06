cmake_minimum_required(VERSION 2.8)
project(ETVDB_CLI)
set(PACKAGE_VERSION 0.1.0)
set(PACKAGE ${CMAKE_PROJECT_NAME})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

INCLUDE(FindPkgConfig)
pkg_check_modules(EINA REQUIRED eina)
pkg_check_modules(ECORE REQUIRED ecore)
pkg_check_modules(ECORE-FILE REQUIRED ecore-file)

include_directories(${EINA_INCLUDE_DIRS} ${ECORE_INCLUDE_DIRS}
	${ECORE-FILE_INCLUDE_DIRS})

add_executable(etvdb_cli etvdb_cli.c)
target_link_libraries(etvdb_cli etvdb ${EINA_LIBRARIES}
	${ECORE_LIBRARIES} ${ECORE-FILE_LIBRARIES})
set_target_properties(etvdb_cli PROPERTIES OUTPUT_NAME etvdb)

install(TARGETS etvdb_cli RUNTIME DESTINATION bin)
