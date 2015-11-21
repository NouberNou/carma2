//carma.hpp

#define CARMA_COMPILE(file) [file, true] call carma2_fnc_compile
#define CARMA_STRCOMPILE(string) [string, true] call carma2_fnc_compileStr
#define IS_CARMAOBJECT(var) (typeName var == "LOCATION" && {(text var) == "carma2_obj"})

#define CRITICAL_PARAMS   params ["_counter"]; (carma2_criticalArgs select _counter) params
#define CRITICAL_SETRETURN(val) carma2_criticalArgs set[_counter, val]

#define carma2_start_crit_section ____carma_crit_section = {
#define carma2_end_crit_section }; "_this call ____carma_crit_section; false;" configClasses (configfile >> "CarmaBlank")

#define var private
