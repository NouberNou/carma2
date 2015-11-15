# carma2

A thinline object parser/translator for RV SQF offering a prototyped object system with automatic, unintrusive garbage collecting memory management.

Github: https://github.com/NouberNou/carma2

The only dependency is [CBA](https://github.com/CBATeam/CBA_A3).

## Concept

The concept of carma2 is to add a very low overhead, very simple object implementation in SQF. The focus is on maintaining as many paradigms with SQF as possible, so as to not create too much of a disassociation with base SQF.

Implementation of objects is done in a very simple fashion that ultimately is closer to syntactic sugar than it is a "proper" object implementation, but through the help of helpers provides a robust object system. Objects are created using the `new` keyword and members and methods are accessed using the `.` operator and assigned/defined using the standard SQF `=` operator. The only major difference is that method invocation is done using `()` following the method name, instead of the standard SQF `arg call function` format (though it is entirely possible to invoke methods this way, though with some caveats). 

A simple carma2 example is below:
```
_testObject = new carma2_object(); // create a new object from the default carma2_base object
_testObject.myMethod = { player sideChat "hello world!"; };
_testObject.myMethod(); // calls myMethod and displays "hello world!"
```

Method parameters can be accessed via the normal `_this` variable.

Objects as such then are defined as they are created, similar to the Javascript prototype system. As such creating a new object of an existing type is as easy as follows:

```
// using the code from above
_anotherTestObject = new _testObject();
_anotherTestObject.myMethod(); // calls myMethod on this new object.
```

Objects will copy their members (but will **not** copy their values) to the new object, and copy their methods as well. This means that you can easily define and create new objects. Type checking can be done via the special `__prototype` member in every object, which will contain the object that `new` was called on. Introspection can be achieved via using the normal `allVariables` SQF command on an object (carma2 objects are implemented as location objects, which have no in-game overhead).

Method definitions can access their calling object via the `_thisObj` variable.

For example:
```
_testObject = new carma2_object(); // create a new object from the default carma2_base object
_testObject.myMethod = { player sideChat format["myVal: %1", _thisObj.myVal]; };
_testObject.myVal = 2;
_testObject.myMethod(); // calls myMethod and displays "myVal: 2"
```

Objects can call a constructor like function on creation by assigning the special method `__init`. There are no destructors in carma2 as the system uses a garbage collecting reference tracker. Implementation of a special method for when the object is garbage collected (or the `del` keyword is used) is forthcoming, though programmers using carma2 should make sure to smartly implement resources that need to be freed in a way that is not dependent on the lifespan of the object.

## Usage

Using carma2 is very simple. Launch with the mod enabled/included, as well as CBA.

A simple usage example is here:
```
#include "\x\carma2\rv\addons\lib\carma.hpp"

CARMA_COMPILE("test.sqf");
```

That code will compile and load the file `test.sqf`. You can then access any globally defined objects compiled in there.

The `CARMA_COMPILE` macro is a macro to `carma2_fnc_compile`. If you just wish to compile your code with out executing it (not often the case), you can pass an optional `false` argument. The compilation function will return the compiled results either way.

## Advanced Concepts

###Calling Parent Functions

Calling the parent function of an overridden object method can be done via the `__prototype` member as seen below:
```
_testObject = new carma2_object(); // create a new object from the default carma2_base object
_testObject.myMethod = { player sideChat "hello world!"; };
_testObject.myMethod(); // calls myMethod and displays "hello world!"

_anotherTestObject = new _testObject();
_anotherTestObject.myMethod = { _thisObj.__prototype.myMethod(); player sideChat "good bye world!"; };
_anotherTestObject.myMethod(); // calls myMethod on this new object, which invokes myMethod from _testObject, printing "hello world!" and then "good bye world!"
```

An example of the output of this can be seen here: http://pastebin.com/raw.php?i=ar1y6PUV

###Chaining

Chaining members is allowed if they are also objects (if they are not, undefined RPT errors may occur).
```
_var = _testObject.myMemberObject.anotherMember; // accessing a members member.
_testObject.myMemberObject.someMethod(1,2,3); // invoking a members method.
```

Chaining methods is also supported:
```
_testObject = new carma2_object();
_testObject.hello = { player sideChat "hello"; _thisObj; };
_testObject.world = { player sideChat "world"; _thisObj; };
_testObject.hello().world(); // prints "hello" and then prints "world"
```

###Anonymous Objects

Passing an object to a SQF function or a carma2 object method can be done anonymously via the `new` keyword.
```
[new someObject()] call some_sqf_fnc;
_myObject.method(new subObject());
```

###Array [] Accessors

Arrays can now be accessed and manipulated via the more traditional `[]` operator as in other languages.

```
_testObject = new carma2_object();

_testObject.myArray = [1,2,3];
player sideChat format["_testObject.myArray[0] = %1", _testObject.myArray[0]]; // access via the [] operator

_testObject.myArray[0] = 2; // assign via the [] operator.
player sideChat format["_testObject.myArray[0] = %1", _testObject.myArray[0]];

_testObject.getArray = {
    _thisObj.myArray;
};
_testObject.getArray()[0] = 3; // assign via reference return.

player sideChat format["_testObject.myArray[0] = %1", _testObject.getArray()[0]]; //access via reference return
```

Arrays on their own can be accessed via the `[]` operator as well.

```
_testArray = [1,2,3];
player sideChat format["Test array: %1", _testArray[1]];
_testArray[1] = 48;
player sideChat format["Test array: %1", _testArray[1]];
```

###Traditional Function Calling

You can call any SQF defined function the same as you would in most languages using parenthesis.

```
my_func = { player sideChat format["this: %1", _this]; };
my_func(1,2,3); // prints "this: [1,2,3]"
```

###String Member Accesor {}

Using the `{}` operator you can access member variables using strings.
```
test.member = 123;
_val = test{"member"}; // assigns 123;
test{"member"} = 321; // reassign using string accessor;
```

This functionally turns objects into a hash map (they are internally in carma2, and in the engine represented as a hash map anyways, so this makes sense).

Because of this, carma2 comes with a default hash map object, `carma2_hashmap`. An example is provided below:
```
_testHash = new carma2_hashmap();
_testHash{"key1"} = 123;
_testHash{"key2"} = 321;
_testHash{"key3"} = 999;

{
    player sideChat format["%1: %2", _x, _testHash{_x}];
} forEach _testHash._keys();
```

Beyond the `carma2_hashmap._keys()` method show above there is also a `carma2_hashmap._hasKey(keyname)` and `carma2_hashmap._delete(key)` function as well. More documentation on this and other default objects will come in the wiki at some point in the future.

## Performance

A often run into drawback with object oriented systems in SQF are the overhead that objects introduce, either through their programmatic implementation or through their in engine implementation. In carma2, the language strives to be as close as possible to the engine, to minimize overhead. To do this carma2 utilizes the native `setVariable` and `getVariable` SQF functions on native SQF objects, which in this case are [locations](https://community.bistudio.com/wiki/createLocation). Locations in SQF add no apparent overhead to game performance, and are simply resident in the SQF engine's memory. As such, tens of thousands of them can be initiated with no performance impact. This is already being utilized in projects such as ACRE for implementing a fast, SQF native hash-map implementation.

Because of this member variable access is a simple call to `getVariable`. Assignments are a simple call to `setVariable`. Invoking a method simply calls a wrapper function that creates the `_thisObj` special variable and then calls the arguments on a `getVariable` call. Overhead on method invocation is as little as `0.0077ms`, and default single member access is often a third of that. This provides almost native SQF level speeds.