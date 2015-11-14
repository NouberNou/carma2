//carma.hpp

#define CARMA_COMPILE(file) [file, true] call carma2_fnc_compile
#define IS_CARMAOBJECT(var) (typeName var == "LOCATION" && {(text var) == "carma2_obj"})