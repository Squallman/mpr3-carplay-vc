#pragma once

// Project-owned opaque types and recovered P3695 register shapes, not Apple headers.
namespace mpr3::target {
struct OpaqueCFObject;
struct OpaqueCFDictionary;
struct OpaqueCFArray;
struct OpaqueCFString;
struct OpaqueCFLArrayCallbacks;

using CFLInt32 = int;
using CFLUInt32 = unsigned int;
using CFLInt64 = long long;
static_assert(sizeof(CFLInt32) == 4 && sizeof(CFLUInt32) == 4, "P3695 w-register ABI");
static_assert(sizeof(CFLInt64) == 8, "P3695 signed Int64 extraction");
static_assert(sizeof(void *) == 8, "This contract models the recovered 64-bit target");

using CFArrayGetTypeIDFn = CFLUInt32 (*)();
using CFDictionaryGetTypeIDFn = CFLUInt32 (*)();
using CFDictionaryGetTypedValueFn = const OpaqueCFObject *(*)(
    const OpaqueCFDictionary *, const OpaqueCFString *, CFLUInt32, CFLInt32 *);
using CFDictionaryGetInt64Fn = CFLInt64 (*)(
    const OpaqueCFDictionary *, const OpaqueCFString *, CFLInt32 *);
using CFArrayGetCountFn = CFLInt32 (*)(const OpaqueCFArray *);
using CFArrayGetTypedValueAtIndexFn = const OpaqueCFObject *(*)(
    const OpaqueCFArray *, CFLInt32, CFLUInt32, CFLInt32 *);
using CFDictionaryCreateMutableCopyFn = OpaqueCFDictionary *(*)(
    const void *, CFLInt32, const OpaqueCFDictionary *);
using CFArrayCreateMutableFn = OpaqueCFArray *(*)(
    const void *, CFLInt32, const OpaqueCFLArrayCallbacks *);
using CFArrayAppendValueFn = CFLInt32 (*)(OpaqueCFArray *, const OpaqueCFObject *);
using CFDictionarySetValueFn = CFLInt32 (*)(
    OpaqueCFDictionary *, const OpaqueCFString *, const OpaqueCFObject *);
using CFStringCreateWithCStringFn = const OpaqueCFString *(*)(
    const void *, const char *, CFLUInt32);
using CFRetainFn = const OpaqueCFObject *(*)(const OpaqueCFObject *);
using CFReleaseFn = void (*)(const OpaqueCFObject *);

inline constexpr CFLUInt32 recoveredCStringEncodingSelector = 0x08000100;
} // namespace mpr3::target
