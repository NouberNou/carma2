class CfgPatches {
	class CarmaLib {
		units[] = {};
		weapons[] = {};
		requiredVersion = 1.0;
		requiredAddons[] = { "CBA_XEH" };
		version = 1.0;
		author[] = {"Nou"};
	};
};

class Extended_PreInit_EventHandlers {
	class CarmaLib {
		init = "[] call (compile preprocessFileLineNumbers '\x\carma2\rv\addons\lib\XEH_pre_init.sqf');";
	};
};

class CarmaBlank {
    class CarmaDummy {
        val = 1;
    };
};

class CfgLocationTypes {
    class CarmaType {
        color[] = {0,0,0,0};
        drawStyle = "carma";
        font = "PuristaMedium";
        importance = 5;
        name = "carma";
        shadow = 0;
        size = 0;
        textSize = 0.0;
        texture = "";
    };
};