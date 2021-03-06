// Iterator prototype surfaces.

load(libdir + "asserts.js");
load(libdir + "iteration.js");

function test(constructor) {
    var proto = Object.getPrototypeOf(constructor()[std_iterator]());
    var names = Object.getOwnPropertyNames(proto);
    names.sort();
    assertDeepEq(names, JS_HAS_SYMBOLS ? ['next'] : ['@@iterator', 'next']);
    assertEq(proto.hasOwnProperty(std_iterator), true);

    var desc = Object.getOwnPropertyDescriptor(proto, 'next');
    assertEq(desc.configurable, true);
    assertEq(desc.enumerable, false);
    assertEq(desc.writable, true);

    assertEq(proto[std_iterator](), proto);
    assertIteratorDone(proto, undefined);
}

//test(Array);
test(Map);
test(Set);
