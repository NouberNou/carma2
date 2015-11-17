#define CUNIT_TEST(file) private _cUnitCompiledTests = call { _cUnitCompiledTests = if (isnil "_cUnitCompiledTests") then {[]} else {_cUnitCompiledTests}; _cUnitCompiledTests pushback (call ([file, false] call carma2_fnc_compile)); _cUnitCompiledTests}
#define CUNIT_EXEC(TESTS) ([TESTS] call ((carma getvariable "CUnit") getvariable "testAll"));
#define CUNIT_RUN call {if (isnil "_cUnitCompiledTests") then {_cUnitCompiledTests = []}; var _results = CUNIT_EXEC(_cUnitCompiledTests); _results};
