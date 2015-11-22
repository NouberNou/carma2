//lib_carma2_object.sqf
#include "carma.hpp"

carma2_object.__id = -1;
carma2_object.__handles = [];
carma2_object.__prototype = objNull;

carma2_object.__isTypeOf = function(_type) {
    private _found = false;
    private _check = _thisObj;
    while (_check != objNull) {
        if(_check == _type) {
            return true;
        };
        _check = _check.__prototype;
    };
    return false;
};

carma2_object.__baseMembers = allVariables(carma2_object);
pushBack(carma2_object.__baseMembers, "__basemembers");
