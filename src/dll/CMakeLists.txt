file(GLOB_RECURSE CARMA_DLL_SOURCES *.h *.hpp *.c *.cpp)


SOURCE_GROUP("dll" FILES ${CARMA_DLL_SOURCES})

add_library(carma_dll MODULE ${CARMA_DLL_SOURCES})
set_target_properties(carma_dll PROPERTIES FOLDER Carma2)
target_link_libraries(carma_dll carma_compiler)
add_dependencies(carma_dll carma_compiler)
SET_TARGET_PROPERTIES(carma_dll PROPERTIES PREFIX "")