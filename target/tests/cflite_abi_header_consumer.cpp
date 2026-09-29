#include "mpr3/target/cflite_abi.hpp"

using namespace mpr3::target;
template<class A, class B> inline constexpr bool same = false;
template<class A> inline constexpr bool same<A, A> = true;
static_assert(sizeof(void *) == 8 && sizeof(int) == 4 && sizeof(long long) == 8);
static_assert(same<CFArrayGetTypeIDFn, unsigned int (*)()>);
static_assert(same<CFDictionaryGetTypeIDFn, unsigned int (*)()>);
static_assert(same<CFDictionaryGetTypedValueFn, const OpaqueCFObject *(*)(
    const OpaqueCFDictionary *, const OpaqueCFString *, unsigned int, int *)>);
static_assert(same<CFDictionaryGetInt64Fn, long long (*)(
    const OpaqueCFDictionary *, const OpaqueCFString *, int *)>);
static_assert(same<CFArrayGetCountFn, int (*)(const OpaqueCFArray *)>);
static_assert(same<CFArrayGetTypedValueAtIndexFn, const OpaqueCFObject *(*)(
    const OpaqueCFArray *, int, unsigned int, int *)>);
static_assert(same<CFDictionaryCreateMutableCopyFn, OpaqueCFDictionary *(*)(
    const void *, int, const OpaqueCFDictionary *)>);
static_assert(same<CFArrayCreateMutableFn, OpaqueCFArray *(*)(
    const void *, int, const OpaqueCFLArrayCallbacks *)>);
static_assert(same<CFArrayAppendValueFn, int (*)(OpaqueCFArray *, const OpaqueCFObject *)>);
static_assert(same<CFDictionarySetValueFn, int (*)(
    OpaqueCFDictionary *, const OpaqueCFString *, const OpaqueCFObject *)>);
static_assert(same<CFStringCreateWithCStringFn, const OpaqueCFString *(*)(
    const void *, const char *, unsigned int)>);
static_assert(same<CFRetainFn, const OpaqueCFObject *(*)(const OpaqueCFObject *)>);
static_assert(same<CFReleaseFn, void (*)(const OpaqueCFObject *)>);
extern "C" int CFLiteAbiHeaderConsumer() { return sizeof(CFLInt64); }
