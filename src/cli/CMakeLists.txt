file(GLOB_RECURSE CARMA_CLI_SOURCES *.h *.hpp *.c *.cpp)

include_directories ("${PROJECT_SOURCE_DIR}")

add_executable(carma_cli ${CARMA_CLI_SOURCES})
target_link_libraries(carma_cli carma_compiler)
add_dependencies(carma_cli carma_compiler)
SET_TARGET_PROPERTIES(carma_cli PROPERTIES PREFIX "")
set_target_properties(carma_cli PROPERTIES FOLDER Carma2)