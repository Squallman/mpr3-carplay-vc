#pragma once
#include "mpr3/target/cflite_api.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cflite_test {
using namespace mpr3::target;
enum class Operation { String, Copy, Array, Append, Set, TypedLookup, IndexLookup, Int64, Retain };
struct Counters {
  unsigned retain = 0, release = 0, string = 0, copy = 0, array = 0, append = 0, set = 0;
};
// Test model only. The real opaque CFLite object layout is intentionally undefined.
class FakeCFLite final {
 public:
  FakeCFLite();
  ~FakeCFLite();
  FakeCFLite(const FakeCFLite &) = delete;
  FakeCFLite &operator=(const FakeCFLite &) = delete;
  CFLiteApi api() const;
  void fail(Operation, unsigned occurrence = 1);
  void clearFailure() noexcept;
  // Arm the TEST executable's next host allocation failure at an ABI boundary.
  void failHostAllocationAfter(Operation, unsigned occurrence = 1, unsigned allocationsUntilFailure = 1);
  Counters counters;
  const OpaqueCFString *string(const std::string &);
  const OpaqueCFObject *integer(std::int64_t);
  OpaqueCFDictionary *dictionary();
  OpaqueCFArray *array();
  void put(OpaqueCFDictionary *, const std::string &, const OpaqueCFObject *);
  void append(OpaqueCFArray *, const OpaqueCFObject *);
  const OpaqueCFObject *get(const OpaqueCFDictionary *, const std::string &) const;
  std::vector<const OpaqueCFObject *> elements(const OpaqueCFArray *) const;
  std::string text(const OpaqueCFString *) const;
  void release(const OpaqueCFObject *);
  unsigned refs(const OpaqueCFObject *) const;
  unsigned releases(const OpaqueCFObject *) const;
  unsigned liveObjects() const;
  unsigned totalReferences() const;
  bool argumentsValid = true;
  const OpaqueCFDictionary *lastCopy = nullptr;
  const OpaqueCFArray *lastArray = nullptr;
 private:
  struct Object;
  std::vector<std::unique_ptr<Object>> objects_;
  struct Injection { Operation operation; unsigned remaining; unsigned allocationsUntilFailure = 1; };
  std::unique_ptr<Injection> failure_, hostFailure_;
  static thread_local FakeCFLite *active_;
  char callbacksToken_{};
  Object *create(unsigned type);
  static Object *object(const void *);
  bool fails(Operation);
  void boundary(Operation);
  const OpaqueCFObject *retainObject(const OpaqueCFObject *);
  static CFLUInt32 arrayType();
  static CFLUInt32 dictionaryType();
  static const OpaqueCFObject *typedValue(const OpaqueCFDictionary *, const OpaqueCFString *, CFLUInt32, CFLInt32 *);
  static CFLInt64 int64Value(const OpaqueCFDictionary *, const OpaqueCFString *, CFLInt32 *);
  static CFLInt32 count(const OpaqueCFArray *);
  static const OpaqueCFObject *typedIndex(const OpaqueCFArray *, CFLInt32, CFLUInt32, CFLInt32 *);
  static OpaqueCFDictionary *copy(const void *, CFLInt32, const OpaqueCFDictionary *);
  static OpaqueCFArray *createArray(const void *, CFLInt32, const OpaqueCFLArrayCallbacks *);
  static CFLInt32 appendValue(OpaqueCFArray *, const OpaqueCFObject *);
  static CFLInt32 setValue(OpaqueCFDictionary *, const OpaqueCFString *, const OpaqueCFObject *);
  static const OpaqueCFString *createString(const void *, const char *, CFLUInt32);
  static const OpaqueCFObject *retainValue(const OpaqueCFObject *);
  static void releaseValue(const OpaqueCFObject *);
};
template<class T> const OpaqueCFObject *asObject(const T *value) {
  return reinterpret_cast<const OpaqueCFObject *>(value);
}
template<class T> const T *as(const OpaqueCFObject *value) {
  return reinterpret_cast<const T *>(value);
}
// Defined by the test-only allocation injector, never a production object.
void failNextHostAllocation(unsigned allocationsUntilFailure = 1) noexcept;
void cancelHostAllocationFailure() noexcept;
} // namespace cflite_test
