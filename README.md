# carma2

A little thinline object parser/translator for RV SQF.

## Concept

The concept of carma2 is to add a very low overhead, very simple object implementation in SQF. The focus is on maintaining as many paradigms with SQF as possible, so as to not create too much of a disassociation with base SQF.

Implementation of objects is done in a very simple fashion that ultimately is closer to syntactic sugar than it is a "proper" object implementation, but through the help of helpers provides a robust object system. Objects are created using the `new` keyword and members and methods are accessed using the `.` operator and assigned/defined using the standard SQF `=` operator. The only major difference is that method invokation is done using `()` following the method name, instead of the standard SQF `arg call function` format (though it is entirely possible to invoke methods this way, though with some caveats). 

For example:
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

## Advanced Concepts

###Calling Parent Functions

Calling the parent function of an overridden object method can be done via the `__prototype` member as seen below:
```
_testObject = new carma2_object(); // create a new object from the default carma2_base object
_testObject.myMethod = { player sideChat "hello world!"; };
_testObject.myMethod(); // calls myMethod and displays "hello world!"

_anotherTestObject = new _testObject();
_testObject.myMethod = { _thisObj.__prototype.myMethod(); player sideChat "good bye world!"; };
_anotherTestObject.myMethod(); // calls myMethod on this new object, which invokes myMethod from _testObject, printing "hello world!" and then "good bye world!"
```

###Chaining

Chaining members is allowed if they are also objects (if they are not, undefined RPT errors may occur).
```
_var = _testObject.myMemberObject.anotherMember; // accessing a members member.
_testObject.myMemberObject.someMethod(1,2,3); // invoking a members method.
```

As of the time of writing, method chaining produces syntax bugs. It will be fixed before release.

###Anonymous Objects

Passing an object to a SQF function or a carma2 object method can be done anonymously via the `new` keyword.
```
[new someObject()] call some_sqf_fnc;
_myObject.method(new subObject());
```