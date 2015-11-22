// TODO Here until we move it to a different namespace
if (isNil "carma") { carma = new carma_object(); };

carma.CUnit = new carma_object();

// So we have a local stored version that is shorter named
private _cunit = carma.CUnit;

_cunit.execTests = function(_tests) {
    private _res = carma.CUnit.testAll(_tests);
    private _ret = 0;
    forEach(_res) {
        params(_x,["_sum", "_success", "_failure", "_inconclusive", "_testResults"]);
        forEach(_testResults){
            params(_x, ["_testName", "_passed", "_messages"]);
            diag_log(text(format["%1", _testName]));
            forEach(_messages){ diag_log text format["%1", _x]; };
        };
        diag_log(text(format["Ran tests %1", _sum]));
        diag_log(text(format["Passed: %1, Failed: %2, ", _success, _failure]));
        if (_failure > 0) { _ret = _ret + 1 };
    };
    return _ret;
};

_cunit.testAll = function(_tests) {
    private _testRunResults = [];
    forEach(_tests){
        pushback(_testRunResults, carma.CUnit.test(_x));
    };
    return _testRunResults;
};

_cunit.findAllTests = function(_object) {
    private _tests = [];
    forEach(allVariables(_object)){
        private _var = getvariable(_object, _x);
        // Collect all functions starting with test.
        if (!isNil("_var") && typeName(_var) == "CODE" && select(_x,[0,4]) == "test") {
            pushback(_tests, _x);
        };
    };

    return _tests;
};

_cunit.test = function(_object) {
    private _success = 0;
    private _failure = 0;
    private _inconclusive = 0;

    private _allTests = carma.CUnit.findAllTests(_object);
    if (isNil "_allTests") { _allTests = [] };

    private _testResults = [];
    forEach(_allTests) {
        //try {
            _object{_x}();
            _success = _success + 1;
            pushback(_testResults, [_x, true, ["Test passed"]]);
        //} catch {
            if (typeName _exception == "ARRAY" && {params(_exception,["_error", "_message"])}) {
                params(_exception, ["_error", "_message"]);
                _failure = _failure + 1;
                pushback(_testResults, [_x, false, ["Test Failed", _error + ". " + _message]]);
            } else {
                _inconclusive = _inconclusive + 1;
                pushback(_testResults, [_x, true, ["Test Inconclusive"]]);
            };
        //};
        _sum = _forEachIndex;
    };

    private _sum = _success + _failure + _inconclusive;
    return [_sum, _success, _failure, _inconclusive, _testResults];
};

_cunit.assertEquals = function(_expected, _actual, _message) {
    if (isNil "_message") {_message = ""};
    if (!isEqualTo(_expected,_actual)) {
        throw([format["Expected %1 but was %2", _expected, _actual], _message]);
    };
};
_cunit.assertNotEquals = function(_expected, _actual, _message) {
    if (isNil "_message") {_message = ""};
    if (isEqualTo(_expected,_actual)) {
        throw([format["Expected %1 to not be equal to %2", _expected, _actual], _message]);
    };
};

_cunit.assertNotNull = function(_actual, _message) {
    if (isNil "_message") {_message = ""};
    if (isNull _actual) {
        throw([format["Value is null"], _message]);
    };
};

_cunit.assertNull = function(_actual, _message) {
    if (isNil "_message") {_message = ""};
    if (!isNull _actual) {
        throw([format["Was %1, not null", _actual], _message]);
    };
};

_cunit.assertTrue = function(_condition, _message) {
    if (isNil "_message") {_message = ""};
    switch (typeName(_condition)) {
        case ("BOOL"):
            if (!_condition) {
                throw([format["was false, not true"], _message]);
            };
        break;
        case ("CODE"):
            if (!isEqualTo(_condition(),true)) {
                throw([format["was false, not true"], _message]);
            };
        break;
        default
            throw([format["was %1, not of boolean type", _condition], _message]);
        break;
    };
};

_cunit.assertFalse = function(_condition, _message) {
    if (isNil "_message") {_message = ""};
    switch (typeName _condition) {
        case ("BOOL"):
            if (_condition) {
                throw([format["was true, not false"], _message]);
            };
        break;
        case ("CODE"):
            if (!isEqualTo(_condition(),false)) {
                throw([format["was true, not false"], _message]);
            };
        break;
        default:
            throw([format["was %1, not of boolean type", _condition], _message]);
        break;
    };
};
