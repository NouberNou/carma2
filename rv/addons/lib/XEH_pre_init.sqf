#include "carma.hpp"

carma2_createdObjects = [];
carma2_objectIdCounter = 0;
carma2_criticalArgs = [];
carma2_criticalCounter = 0;

deleteLocation carma2_object;
carma2_object = createLocation ["CarmaType", [-10000,-10000,-10000], 0, 0];
carma2_object setText "carma2_obj";

carma2_object setVariable ["__id", -1];
carma2_object setVariable ["__handles", []];
carma2_object setVariable ["__prototype", objNull];




carma2_fnc_callCritical = {
    params ["_args", "_function"];
    private ["_return", "_counter"];
    _counter = carma2_criticalCounter;
    carma2_criticalCounter = carma2_criticalCounter + 1;
    carma2_criticalArgs set[_counter, _args];
    format["[%1] call %2; false;", _counter, _function] configClasses (configfile >> "CarmaBlank");
    _return = carma2_criticalArgs select _counter;
    carma2_criticalCounter = carma2_criticalCounter - 1;
    _return;
};

carma2_fnc_newObject = {
    [_this, "carma2_fnc_newObjectInternal"] call carma2_fnc_callCritical;
};


carma2_fnc_newObjectInternal = {
    CRITICAL_PARAMS ["_args", "_type", "_scriptHandle"];
    private ["_key", "_val", "_thisObj"];
    private _newObj = createLocation ["CarmaType", [-10000,-10000,-10000], 0, 0];
    _newObj setText "carma2_obj";
    {
        _key = _x;
        _val = _type getVariable _x;
        if(typeName _val != "CODE") then {
            _val = nil;
        };
        _newObj setVariable [_key, _val];
    } forEach (allVariables _type);
    _newObj setVariable ["__id", carma2_objectIdCounter];
    _newObj setVariable ["__handles", [_scriptHandle]];
    _newObj setVariable ["__prototype", _type];

    private _constructor = _newObj getVariable "__init";
    if(!isNil "_constructor") then {
        _thisObj = _newObj;
        _args call _constructor;
    };
    carma2_objectIdCounter = carma2_createdObjects pushback _newObj;
    CRITICAL_SETRETURN(_newObj);
};

carma2_fnc_delObject = {
    params ["_object"];
    deleteLocation _object;
};

carma2_fnc_methodInvoke = {
    params ["_thisObj","_method","_args"];
    _args call (_thisObj getVariable _method);
};

carma2_fnc_methodInvokeContext = {
    params ["_method","_args"];
    private _thisObj = _args deleteAt 0;
    _args call _method;
};

carma2_fnc_methodApplyContext = {
    params ["_method","_args"];
    _args params ["_thisObj", "_args"];
    _args call _method;
};

carma2_fnc_compile = {
    params ["_file", ["_execute", true]];
    private ["_result"];
    _text = preprocessFile _file;
    _text = "1" + _text;
    _result = "carma_dll" callExtension _text;
    _result = (compile preprocessFileLineNumbers _result);
    if(_execute) then {
        [] call _result;
    };
    _result;
};

carma2_fnc_spawnWrapper = {
    [_this, "carma2_fnc_spawnWrapperInternal"] call carma2_fnc_callCritical;
};

carma2_fnc_spawnWrapperInternal = {
    CRITICAL_PARAMS ["_args", "_func"];
    private ["_handle"];
    _handle = _args spawn _func;
    _arraySearch = {
        {
            if(IS_CARMAOBJECT(_x)) then {
                (_x getVariable "__handles") pushBack _handle;
            } else {
                if(typeName _x == "ARRAY") then {
                    _x call _arraySearch;
                };
            };
        } forEach _this;
    };
    if(IS_CARMAOBJECT(_args)) then {
        player sideChat format["pushback: %1", _handle];
        (_args getVariable "__handles") pushBack _handle;
    } else {
        if(typeName _args == "ARRAY") then {
            _args call _arraySearch;
        };
    };
    CRITICAL_SETRETURN(_handle);
};

{
    "carma_dll" callExtension ("0" + _x);
} forEach (supportInfo "");

CARMA_COMPILE("\x\carma2\rv\addons\lib\lib_carma2_hashmap.sqf");
