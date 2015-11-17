// TODO Here until we move it to a different namespace
if (isNil "carma") then { carma = new carma_object(); };

carma.CUnit = new carma_object();

// So we have a local stored version that is shorter named
private _cunit = carma.CUnit;

_cunit.testAll = function(_tests) {
    private _testRunResults = [];
    {
        _testRunResults pushback (_thisObj.test(_x));
    } forEach _tests;

    _testRunResults;
};

_cunit.findAllTests = {
    params ["_object"];
    _object getVariable "tests";
};

_cunit.test = function(_object) {
    private _success = 0;
    private _failure = 0;
    private _inconclusive = 0;

    private _allTests = _thisObj.findAllTests(_object);
    if (isNil "_allTests") then { _allTests = [] };
    private _testResults = [];
    {
        try {
            _object{_x}();
            _success = _success + 1;
            _testResults pushback [_x, true, ["Test passed"]];
        } catch {
            if (typeName _exception == "ARRAY" && {(_exception params ["_error", "_message"])}) then {
                _exception params ["_error", "_message"];
                _failure = _failure + 1;
                _testResults pushback [_x, false, ["Test Failed", _error + ". " + _message]];
            } else {
                _inconclusive = _inconclusive + 1;
                _testResults pushback [_x, true, ["Test Inconclusive"]];
            };
        };
        _sum = _forEachIndex;
    } forEach _allTests;

    private _sum = _success + _failure + _inconclusive;
    [_sum, _success, _failure, _inconclusive, _testResults];
};

_cunit.assertEquals = function(_expected, _actual, _message) {
    if (isNil "_message") then {_message = ""};
    if !(_expected isEqualTo _actual) then {
        throw [format["Expected %1 but was %2", _expected, _actual], _message];
    };
};
_cunit.assertNotEquals = function(_expected, _actual, _message) {
    if (isNil "_message") then {_message = ""};
    if (_expected isEqualTo _actual) then {
        throw [format["Expected %1 to not be equal to %2", _expected, _actual], _message];
    };
};

_cunit.assertNotNull = function(_actual, _message) {
    if (isNil "_message") then {_message = ""};
    if (isNull _actual) then {
        throw [format["Value is null"], _message];
    };
};

_cunit.assertNull = function(_actual, _message) {
    if (isNil "_message") then {_message = ""};
    if (!isNull _actual) then {
        throw [format["Was %1, not null", _actual], _message];
    };
};

_cunit.assertTrue = function(_condition, _message) {
    if (isNil "_message") then {_message = ""};
    switch (typeName _condition) do {
        case ("BOOL"): {
            if (!_condition) then {
                throw [format["was false, not true"], _message];
            };
        };
        case ("CODE"): {
            if (!(([] call _condition) isEqualTo true)) then {
                throw [format["was false, not true"], _message];
            };
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
            if (_condition) then {
                throw [format["was true, not false"], _message];
            };
        };
        case ("CODE"): {
            if (!(([] call _condition) isEqualTo false)) then {
                throw [format["was true, not false"], _message];
            };
        };
        default {
            throw [format["was %1, not of boolean type", _condition], _message];
        };
    };
};
