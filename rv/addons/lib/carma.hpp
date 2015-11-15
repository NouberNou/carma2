//carma.hpp

#define CARMA_COMPILE(file) [file, true] call carma2_fnc_compile
#define IS_CARMAOBJECT(var) (typeName var == "LOCATION" && {(text var) == "carma2_obj"})

#define CRITICAL_PARAMS   params ["_counter"]; (carma2_criticalArgs select _counter) params 
#define CRITICAL_SETRETURN(val) carma2_criticalArgs set[_counter, val]