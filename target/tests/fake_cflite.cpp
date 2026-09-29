#include "fake_cflite.hpp"
#include <algorithm>
#include <cstdlib>
#include <limits>

namespace cflite_test {
struct FakeCFLite::Object {
  unsigned type = 0, refs = 1, releases = 0;
  std::string text;
  std::int64_t integer = 0;
  std::vector<std::pair<const OpaqueCFString *, const OpaqueCFObject *>> dictionary;
  std::vector<const OpaqueCFObject *> array;
};
thread_local FakeCFLite *FakeCFLite::active_ = nullptr;
FakeCFLite::FakeCFLite() { if (active_) std::abort(); active_ = this; }
FakeCFLite::~FakeCFLite() {
  cancelHostAllocationFailure();
  if (liveObjects() != 0) std::abort();
  active_ = nullptr;
}
FakeCFLite::Object *FakeCFLite::object(const void *value) {
  return const_cast<Object *>(reinterpret_cast<const Object *>(value));
}
FakeCFLite::Object *FakeCFLite::create(unsigned type) {
  auto value = std::make_unique<Object>();
  value->type = type;
  auto *raw = value.get();
  objects_.push_back(std::move(value));
  return raw;
}
CFLiteApi FakeCFLite::api() const {
  return {arrayType, dictionaryType, typedValue, int64Value, count, typedIndex, copy,
      createArray, appendValue, setValue, createString, retainValue, releaseValue,
      reinterpret_cast<const OpaqueCFLArrayCallbacks *>(&callbacksToken_)};
}
void FakeCFLite::fail(Operation operation, unsigned occurrence) {
  failure_ = std::make_unique<Injection>(Injection{operation, occurrence});
}
void FakeCFLite::clearFailure() noexcept { failure_.reset(); }
bool FakeCFLite::fails(Operation operation) {
  if (failure_ && failure_->operation == operation && --failure_->remaining == 0) {
    failure_.reset();
    return true;
  }
  return false;
}
void FakeCFLite::failHostAllocationAfter(Operation operation, unsigned occurrence, unsigned allocationsUntilFailure) {
  hostFailure_ = std::make_unique<Injection>(Injection{operation, occurrence, allocationsUntilFailure});
}
void FakeCFLite::boundary(Operation operation) {
  if (hostFailure_ && hostFailure_->operation == operation && --hostFailure_->remaining == 0) {
    const auto allocations = hostFailure_->allocationsUntilFailure;
    hostFailure_.reset();
    failNextHostAllocation(allocations);
  }
}
const OpaqueCFString *FakeCFLite::string(const std::string &text) {
  auto *value = create(7);
  value->text = text;
  return reinterpret_cast<const OpaqueCFString *>(value);
}
const OpaqueCFObject *FakeCFLite::integer(std::int64_t integer) {
  auto *value = create(6);
  value->integer = integer;
  return reinterpret_cast<const OpaqueCFObject *>(value);
}
OpaqueCFDictionary *FakeCFLite::dictionary() { return reinterpret_cast<OpaqueCFDictionary *>(create(5)); }
OpaqueCFArray *FakeCFLite::array() { return reinterpret_cast<OpaqueCFArray *>(create(1)); }
const OpaqueCFObject *FakeCFLite::retainObject(const OpaqueCFObject *value) {
  if (!value || !object(value)->refs) std::abort();
  ++counters.retain;
  ++object(value)->refs;
  return value;
}
void FakeCFLite::release(const OpaqueCFObject *value) {
  if (!value) return;
  auto *raw = object(value);
  if (!raw->refs) std::abort();
  ++counters.release;
  ++raw->releases;
  if (--raw->refs) return;
  for (const auto &entry : raw->dictionary) {
    release(asObject(entry.first));
    release(entry.second);
  }
  for (const auto entry : raw->array) release(entry);
  raw->dictionary.clear();
  raw->array.clear();
}
unsigned FakeCFLite::refs(const OpaqueCFObject *value) const { return object(value)->refs; }
unsigned FakeCFLite::releases(const OpaqueCFObject *value) const { return object(value)->releases; }
unsigned FakeCFLite::liveObjects() const {
  return static_cast<unsigned>(std::count_if(objects_.begin(), objects_.end(),
      [](const auto &value) { return value->refs != 0; }));
}
unsigned FakeCFLite::totalReferences() const {
  unsigned result = 0;
  for (const auto &value : objects_) result += value->refs;
  return result;
}
std::string FakeCFLite::text(const OpaqueCFString *value) const { return object(value)->text; }
const OpaqueCFObject *FakeCFLite::get(const OpaqueCFDictionary *dictionary, const std::string &key) const {
  if (!dictionary || object(dictionary)->type != 5) return nullptr;
  for (const auto &entry : object(dictionary)->dictionary)
    if (object(entry.first)->text == key) return entry.second;
  return nullptr;
}
std::vector<const OpaqueCFObject *> FakeCFLite::elements(const OpaqueCFArray *array) const {
  return object(array)->array;
}
void FakeCFLite::put(OpaqueCFDictionary *dictionary, const std::string &key, const OpaqueCFObject *value) {
  const auto name = string(key);
  if (setValue(dictionary, name, value) != 0) std::abort();
  release(asObject(name));
}
void FakeCFLite::append(OpaqueCFArray *array, const OpaqueCFObject *value) {
  if (appendValue(array, value) != 0) std::abort();
}
CFLUInt32 FakeCFLite::arrayType() { return 1; }
CFLUInt32 FakeCFLite::dictionaryType() { return 5; }
const OpaqueCFObject *FakeCFLite::typedValue(const OpaqueCFDictionary *dictionary,
    const OpaqueCFString *key, CFLUInt32 type, CFLInt32 *error) {
  const auto value = active_->get(dictionary, object(key)->text);
  const auto failed = active_->fails(Operation::TypedLookup) || !value || object(value)->type != type;
  if (error) *error = failed ? -1 : 0; // Fake-local status; not an AirPlay status.
  return failed ? nullptr : value;
}
CFLInt64 FakeCFLite::int64Value(const OpaqueCFDictionary *dictionary,
    const OpaqueCFString *key, CFLInt32 *error) {
  const auto value = active_->get(dictionary, object(key)->text);
  const auto failed = active_->fails(Operation::Int64) || !value || object(value)->type != 6;
  if (error) *error = failed ? -1 : 0;
  return failed ? 0 : object(value)->integer;
}
CFLInt32 FakeCFLite::count(const OpaqueCFArray *array) {
  if (object(array)->array.size() > static_cast<std::size_t>(std::numeric_limits<CFLInt32>::max())) std::abort();
  return static_cast<CFLInt32>(object(array)->array.size());
}
const OpaqueCFObject *FakeCFLite::typedIndex(const OpaqueCFArray *array,
    CFLInt32 index, CFLUInt32 type, CFLInt32 *error) {
  const auto &entries = object(array)->array;
  const auto inRange = index >= 0 && static_cast<std::size_t>(index) < entries.size();
  const auto value = inRange ? entries[static_cast<std::size_t>(index)] : nullptr;
  const auto failed = active_->fails(Operation::IndexLookup) || !value || object(value)->type != type;
  if (error) *error = failed ? -1 : 0;
  return failed ? nullptr : value;
}
OpaqueCFDictionary *FakeCFLite::copy(const void *allocator, CFLInt32 capacity,
    const OpaqueCFDictionary *source) {
  auto &runtime = *active_;
  ++runtime.counters.copy;
  runtime.argumentsValid &= !allocator && capacity == 0;
  if (runtime.fails(Operation::Copy)) return nullptr;
  auto *result = runtime.dictionary();
  object(result)->dictionary = object(source)->dictionary;
  for (const auto &entry : object(result)->dictionary) {
    runtime.retainObject(asObject(entry.first));
    runtime.retainObject(entry.second);
  }
  runtime.lastCopy = result;
  runtime.boundary(Operation::Copy);
  return result;
}
OpaqueCFArray *FakeCFLite::createArray(const void *allocator, CFLInt32 capacity,
    const OpaqueCFLArrayCallbacks *callbacks) {
  auto &runtime = *active_;
  ++runtime.counters.array;
  runtime.argumentsValid &= !allocator && capacity == 0 && callbacks == runtime.api().typeArrayCallbacks;
  if (runtime.fails(Operation::Array)) return nullptr;
  auto *result = runtime.array();
  runtime.lastArray = result;
  runtime.boundary(Operation::Array);
  return result;
}
CFLInt32 FakeCFLite::appendValue(OpaqueCFArray *array, const OpaqueCFObject *value) {
  auto &runtime = *active_;
  ++runtime.counters.append;
  if (runtime.fails(Operation::Append)) return -1;
  object(array)->array.push_back(value);
  runtime.retainObject(value);
  runtime.boundary(Operation::Append);
  return 0;
}
CFLInt32 FakeCFLite::setValue(OpaqueCFDictionary *dictionary,
    const OpaqueCFString *key, const OpaqueCFObject *value) {
  auto &runtime = *active_;
  ++runtime.counters.set;
  if (runtime.fails(Operation::Set)) return -1;
  auto &entries = object(dictionary)->dictionary;
  auto existing = std::find_if(entries.begin(), entries.end(), [&](const auto &entry) {
    return object(entry.first)->text == object(key)->text;
  });
  if (existing != entries.end()) {
    runtime.retainObject(value);
    runtime.release(existing->second);
    existing->second = value;
  } else {
    entries.emplace_back(key, value);
    runtime.retainObject(asObject(key));
    runtime.retainObject(value);
  }
  runtime.boundary(Operation::Set);
  return 0;
}
const OpaqueCFString *FakeCFLite::createString(const void *allocator,
    const char *text, CFLUInt32 encoding) {
  auto &runtime = *active_;
  ++runtime.counters.string;
  runtime.argumentsValid &= !allocator && encoding == recoveredCStringEncodingSelector;
  if (runtime.fails(Operation::String)) return nullptr;
  const auto result = runtime.string(text);
  runtime.boundary(Operation::String);
  return result;
}
const OpaqueCFObject *FakeCFLite::retainValue(const OpaqueCFObject *value) {
  if (active_->fails(Operation::Retain)) return nullptr;
  const auto result = active_->retainObject(value);
  active_->boundary(Operation::Retain);
  return result;
}
void FakeCFLite::releaseValue(const OpaqueCFObject *value) { active_->release(value); }
} // namespace cflite_test
