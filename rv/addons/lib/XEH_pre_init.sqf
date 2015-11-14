carma2_createdObjects = [];
carma2_objectIdCounter = 0;
carma2_creationArgs = [];
carma2_creationCounter = 0;

deleteLocation carma2_object;
carma2_object = createLocation ["CarmaType", [-10000,-10000,-10000], 0, 0];
carma2_object setText "carma2_obj";

carma2_object setVariable ["__id", -1];
carma2_object setVariable ["__file", -1];
carma2_object setVariable ["__line", -1];
carma2_object setVariable ["__prototype", objNull];

carma2_fnc_newObject = {
    private ["_newObj", "_counter", "_newObj"];
    _counter = carma2_creationCounter;
    carma2_creationCounter = carma2_creationCounter + 1;
    carma2_creationArgs set[_counter, _this];
    format["[%1] call carma2_fnc_newObjectInternal; false;", _counter] configClasses (configfile >> "CarmaBlank");
    _newObj = carma2_creationArgs select _counter;
    carma2_creationCounter = carma2_creationCounter - 1;
    _newObj;
};


carma2_fnc_newObjectInternal = {
    params ["_counter"];
    (carma2_creationArgs select _counter) params ["_args", "_type", "_file", "_line"];
    private ["_newObj", "_constructor", "_key", "_val", "_thisObj"];
    _newObj = createLocation ["CarmaType", [-10000,-10000,-10000], 0, 0];
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
    _newObj setVariable ["__file", _file];
    _newObj setVariable ["__line", _line];
    _newObj setVariable ["__prototype", _type];
    carma2_objectIdCounter = carma2_objectIdCounter + 1;
    _constructor = _newObj getVariable "__init";
    if(!isNil "_constructor") then {
        _thisObj = _newObj;
        _args call _constructor;
    };
    carma2_createdObjects pushBack _newObj;
    carma2_creationArgs set[_counter, _newObj];
};

carma2_fnc_delObject = {
    params ["_object"];
    deleteLocation _object;
};

carma2_fnc_methodInvoke = {
    params ["_thisObj","_method","_args"];
    _args call (_thisObj getVariable _method);
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

{
    "carma_dll" callExtension ("0" + _x);
} forEach (supportInfo "");