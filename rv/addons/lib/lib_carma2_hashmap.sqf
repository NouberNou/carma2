//lib_carma_hashmap.sqf
/**
Unordered hash map implementation in carma2 sqf.
*/
carma2_hashmap = new carma_object();

carma2_hashmap.__init = {
    params [["_initial", []]];
    if(count _initial > 0) then {
        {
            _thisObj{_x[0]} = _x[1];
        } forEach _initial;
    };
};

carma2_hashmap._hasKey = {
    params ["_key"];
    !isNil { _thisObj{_key} };
};

carma2_hashmap._delete = {
    params ["_key"];
    _thisObj{_key} = nil;
    nil;
};


carma2_hashmap._keys = {
    private ["_keys", "_allVars"];
    _allVars = (allVariables _thisObj) - ["__id", "__handles", "__prototype", "_haskey", "_keys", "_delete"];
    _keys = [];
    {
        if(!isNil { _thisObj{_x} }) then {
            _keys pushBack _x;
        };
    } forEach _allVars;
    _keys;
};