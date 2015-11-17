carma.CUnit = new carma_object();

// So we have a local stored version that is shorter named
private _cunit = carma.CUnit;

_cunit.testAll = function(_tests) {
    private _testRunResults = [];
    {
        _testRunResults pushback (_thisObj.test(_x));
    } forEach _tests;

    return _testRunResults;
}

_cunit.findAllTests = function(_object) {
    // TODO find all test methods from _object
    return [];
};

_cunit.test = function(_object) {
    private _success = 0;
    private _failure = 0;
    private _inconclusive = 0;

    private _testResults = [];
    {
        try {
            _object{_x}();
            _success++;
            _testResults pushback [_x, true, ["Test passed"]];
        } catch {
            if (typeName _exception == "ARRAY" && {(_exception params ["_error", "_message"])}) then {
                _failure++;
                _testResults pushback [_x, false, ["Test Failed", _error + ". " + _message]];
            } else {
                _inconclusive++;
                _testResults pushback [_x, true, ["Test Inconclusive"]];
            };
        };
        _sum = _forEachIndex;
    } forEach _cunit.findAllTests(_object);

    return [_sum, _success, _failure, _inconclusive, _testResults];
};

_cunit.assertEquals = function(_expected, _actual, _message) {
    if (isNil "_message") then {_message = ""};
    if (_expected isEqualWith _actual) then {
        return true;
    };
    throw [format["Expected %1 but was %2", _expected, _actual], _message];
};

_cunit.assertNull = function(_actual, _message) {
    if (isNil "_message") then {_message = ""};
    if (isNull _actual) then {
        return true;
    };
    throw [format["Was %1, not null.", _actual], _message];
};

_cunit.assertTrue = function(_condition, _message) {
    if (isNil "_message") then {_message = ""};
    switch (typeName _condition) do {
        case ("BOOL"): {
            if (_condition) then {return true;};
            throw [format["was false, not true"], _message];
        };
        case ("CODE"): {
            if ((([] call _condition) isEqualWith true)) then {
              return true;
            };
            throw [format["was false, not true"], _message];
        };
        default {
            throw [format["was %1, not of boolean type", _condition], _message];
        };
    };
};

_cunit.assertFalse = function(_condition, _message) {
    if (isNil "_message") then {_message = ""};
    switch (typeName _condition) do {
        case ("BOOL"): {
            if (!_condition) then {return true;};
            throw [format["was true, not false"], _message];
        };
        case ("CODE"): {
            if ((([] call _condition) isEqualWith false)) then {
              return true;
            };
            throw [format["was true, not false"], _message];
        };
        default {
            throw [format["was %1, not of boolean type", _condition], _message];
        };
    };
};
