file(GLOB_RECURSE CARMA_COMPILER_SOURCES *.h *.hpp *.c *.cpp)


SOURCE_GROUP("compiler" FILES ${CARMA_COMPILER_SOURCES})

add_library(carma_compiler STATIC ${CARMA_COMPILER_SOURCES})
set_target_properties(carma_compiler PROPERTIES FOLDER Carma2)