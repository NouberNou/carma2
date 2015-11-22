//lib_carma_hashmap.sqf
/**
Unordered hash map implementation in carma2 sqf.
*/
#include "carma.hpp"

carma2_hashmap = new carma_object();

carma2_hashmap.__init = function() {
    params([["_initial", []]]);
    if (count _initial > 0) {
        forEach(_initial) {
            _thisObj{_x[0]} = _x[1];
        };
    };
};

carma2_hashmap._hasKey = function(_key) {
    return !isNil(function(){ return _thisObj{_key} });
};

carma2_hashmap._delete = function(_key){
    _thisObj{_key} = nil;
};


carma2_hashmap._keys = function() {
    private _allVars = (allVariables(_thisObj)) - carma2_hashmap.__baseMembers;
    private _keys = [];
    forEach(_allVars) {
        if (!isNil(function(){ return _thisObj{_x}; })) {
            pushback(_keys,_x);
        };
    };
    return _keys;
};

carma2_hashmap.__baseMembers = allVariables(carma2_hashmap);
