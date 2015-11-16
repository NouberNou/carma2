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

## Performance

A often run into drawback with object oriented systems in SQF are the overhead that objects introduce, either through their programmatic implementation or through their in engine implementation. In carma2, the language strives to be as close as possible to the engine, to minimize overhead. To do this carma2 utilizes the native `setVariable` and `getVariable` SQF functions on native SQF objects, which in this case are [locations](https://community.bistudio.com/wiki/createLocation). Locations in SQF add no apparent overhead to game performance, and are simply resident in the SQF engine's memory. As such, tens of thousands of them can be initiated with no performance impact. This is already being utilized in projects such as ACRE for implementing a fast, SQF native hash-map implementation.

Because of this member variable access is a simple call to `getVariable`. Assignments are a simple call to `setVariable`. Invoking a method simply calls a wrapper function that creates the `_thisObj` special variable and then calls the arguments on a `getVariable` call. Overhead on method invocation is as little as `0.0077ms`, and default single member access is often a third of that. This provides almost native SQF level speeds.

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

###Pseudo-static Members with :: Operator

Using the `::` operator you can easily access a objects prototype members/methods, the same as above using the `__prototype` member.

Since all objects that descend from a common prototype share the same instance of that prototype you can use it to define static methods/members that will be shared across all classes.

```
test_base = new carma2_object(); // this will be our prototype.

test_base.staticMember = 123; // assign the prototype object a member var

test_instance1 = new test_base();
test_instance2 = new test_base();

player sideChat format["test_instance1: %1", test_instance1::staticMember]; // prints 123

test_instance1::staticMember = 321; // assign the static variable on test_instance1 to 321

player sideChat format["test_instance2: %1", test_instance2::staticMember]; // print the static variable on test_instance2, prints 321
```

###Chaining

Chaining members with the `.` or the `::` operator is allowed if they are also objects (if they are not, undefined RPT errors may occur).
```
_var = _testObject.myMemberObject.anotherMember; // accessing a members member.
_testObject.myMemberObject.someMethod(1,2,3); // invoking a members method.

_var = _testObject::myStaticMember::anotherStaticMember; // accessing a static objects member.
_testObject::myStaticObject::someStaticMethod(1,2,3); // invoking a static object's method.
```

Chaining methods is also supported:
```
_testObject = new carma2_object();
_testObject.hello = { player sideChat "hello"; _thisObj; };
_testObject.world = { player sideChat "world"; _thisObj; };
_testObject.hello().world(); // prints "hello" and then prints "world"
```

###Calling Overriden Methods

Calling the original overridden methods is done via accessing the objects prototype object definition, either through the `::` operator, or the `__prototype` member, and then using the magic methods `__call(context, arg1, arg2, ...)` or `__apply(context, arg_array)`. These methods execute the desired overriden/parent method in a supplied context, commonly passing `_thisObj` to the method, along with the methods arguments.

An example is given below demonstrating the usage of the `__call` and `__apply` functions.

```
test_base = new carma2_object();
test_base.testVal = "base instance";
test_base.parentMethod = {
    diag_log text format["Object %1: %2, %3", _thisObj.__id, _thisObj.testVal, _this];
};

test_instance = new test_base();
test_instance.testVal = "child instance";

test_instance.testMethod = {
    // call the prototype method, in the context of the prototype, essentially a static method.
    _thisObj::parentMethod(1);
    
    // call the prototype method, but in the context of this instance using __call(context, arg1, arg2, ...)
    _thisObj::parentMethod.__call(_thisObj, 2);
    
    // call the prototype method, but using __apply(context, arg_array)
    _args = [3];
    _thisObj::parentMethod.__apply(_thisObj, _args);
};

test_instance.testMethod();
```

This prints to the RPT:
```
Object 1: base instance, [1]
Object 2: child instance, [2]
Object 2: child instance, [3]
```

An example of overridden method calling it's parent method is below.

```
test_base = new carma2_object();
test_base.testVal = "base instance";
test_base.testVal = 0;
test_base.testMethod = {
    diag_log text format["parent: %1 testVal: %2", _this[0], _thisObj.testVal];
};

test_instance = new test_base();
test_instance.testVal = 999;
test_instance.testMethod = {
    _thisObj::testMethod(333); // call the parent method as a static method, the context is the __prototype object.
    _thisObj::testMethod.__call(_thisObj, _this[0]); // call takes the args to the function in-situ after the context object
    _thisObj::testMethod.__apply(_thisObj, _this); // apply takes the args as an array
    diag_log text format["child: %1 testVal: %2", _this[0], _thisObj.testVal];
};

test_instance.testMethod(123);
```

Results in:
```
parent: 333 testVal: 0
parent: 123 testVal: 999
parent: 123 testVal: 999
child: 123 testVal: 999
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

