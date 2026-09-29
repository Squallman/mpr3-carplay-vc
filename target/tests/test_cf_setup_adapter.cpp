#include "fake_cflite.hpp"
#include "mpr3/target/cf_setup_adapter.hpp"
#include "test_support.hpp"
#include <cstring>
#include <initializer_list>
#include <limits>
#include <utility>

using namespace cflite_test;
using target_test::TestRun;

namespace {
struct Fixture {
  FakeCFLite runtime;
  CFLiteSetupContextRef context = CFLiteSetupContext::create(runtime.api()).context;
  OpaqueCFDictionary *request = runtime.dictionary();
  const OpaqueCFArray *originalArray = nullptr;
  std::vector<const OpaqueCFDictionary *> dictionaries;
  const OpaqueCFObject *opaque = nullptr;
  explicit Fixture(std::initializer_list<std::int64_t> types = {110, 111}) {
    auto *array = runtime.array();
    originalArray = array;
    for (const auto type : types) {
      auto *dictionary = runtime.dictionary();
      const auto value = runtime.integer(type);
      runtime.put(dictionary, "type", value);
      runtime.release(value);
      runtime.append(array, asObject(dictionary));
      dictionaries.push_back(dictionary);
      runtime.release(asObject(dictionary));
    }
    runtime.put(request, "streams", asObject(array));
    runtime.release(asObject(array));
    opaque = asObject(runtime.string("opaque data"));
    runtime.put(request, "opaque", opaque);
    runtime.release(opaque);
  }
  ~Fixture() { releaseOriginal(); context.reset(); }
  void releaseOriginal() {
    if (request) runtime.release(asObject(request));
    request = nullptr;
  }
  std::unique_ptr<CFSetupRequest> wrapped() { return CFSetupRequest::borrowed(context, request); }
};
class NonTargetDescriptor final : public mpr3::ISetupDescriptor {
 public:
  explicit NonTargetDescriptor(const void *identity) : identity_(identity) {}
  std::optional<std::int64_t> type() const override { return 110; }
  const void *identity() const noexcept override { return identity_; }
 private:
  const void *identity_;
};
void checkOriginal(TestRun &test, Fixture &f) {
  CHECK(f.runtime.get(f.request, "streams") == asObject(f.originalArray));
  CHECK(f.runtime.get(f.request, "opaque") == f.opaque);
  const auto entries = f.runtime.elements(f.originalArray);
  CHECK(entries.size() == f.dictionaries.size());
  for (std::size_t i = 0; i < entries.size(); ++i) CHECK(entries[i] == asObject(f.dictionaries[i]));
}
void checkFiltered(TestRun &test, Fixture &f, const TargetFilterResult &result,
                   std::initializer_list<unsigned> stock, std::initializer_list<unsigned> secondary) {
  CHECK(!result.parseFailed);
  CHECK(result.selection() == TargetRequestSelection::Filtered);
  CHECK(result.selectedRawRequest() != f.request);
  const auto array = as<OpaqueCFArray>(f.runtime.get(result.selectedRawRequest(), "streams"));
  const auto entries = f.runtime.elements(array);
  CHECK(entries.size() == stock.size());
  std::size_t i = 0;
  for (const auto index : stock) CHECK(entries[i++] == asObject(f.dictionaries[index]));
  CHECK(result.secondaryDescriptors.size() == secondary.size());
  i = 0;
  for (const auto index : secondary) {
    CHECK(result.secondaryDescriptors[i]->identity() == f.dictionaries[index]);
    CHECK(result.secondaryDescriptors[i++]->type() == 111);
  }
  CHECK(f.runtime.get(result.selectedRawRequest(), "opaque") == f.opaque);
  CHECK(f.runtime.argumentsValid);
  checkOriginal(test, f);
}
void context_ownership(TestRun &test) {
  FakeCFLite runtime;
  const auto creation = CFLiteSetupContext::create(runtime.api());
  CHECK(creation.failure == CFLiteContextFailure::None);
  CHECK(creation.context);
  CHECK(runtime.counters.string == 2);
  CHECK(runtime.text(creation.context->streamsKey()) == "streams");
  CHECK(runtime.text(creation.context->typeKey()) == "type");
  CHECK(creation.context->arrayTypeID() == 1);
  CHECK(creation.context->dictionaryTypeID() == 5);
  CHECK(runtime.argumentsValid);
  const auto streams = creation.context->streamsKey();
  const auto type = creation.context->typeKey();
  auto context = creation.context;
  // Drop both shared owners before inspecting the fake's dead-object counters.
  context.reset();
  CHECK(runtime.refs(asObject(streams)) == 1);
  CHECK(runtime.refs(asObject(type)) == 1);
}
void context_release(TestRun &test) {
  FakeCFLite runtime;
  const OpaqueCFString *streams, *type;
  {
    auto context = CFLiteSetupContext::create(runtime.api()).context;
    streams = context->streamsKey(); type = context->typeKey();
    CHECK(runtime.refs(asObject(streams)) == 1);
    CHECK(runtime.refs(asObject(type)) == 1);
  }
  CHECK(runtime.refs(asObject(streams)) == 0);
  CHECK(runtime.refs(asObject(type)) == 0);
  CHECK(runtime.releases(asObject(streams)) == 1);
  CHECK(runtime.releases(asObject(type)) == 1);
  CHECK(runtime.liveObjects() == 0);
}
void context_failures(TestRun &test) {
  FakeCFLite runtime;
  const auto empty = CFLiteSetupContext::create(CFLiteApi{});
  CHECK(!empty.context);
  CHECK(empty.failure == CFLiteContextFailure::IncompleteApi);
  CHECK(runtime.counters.string == 0);
  for (unsigned occurrence : {1u, 2u}) {
    runtime.fail(Operation::String, occurrence);
    const auto creation = CFLiteSetupContext::create(runtime.api());
    CHECK(!creation.context);
    CHECK(creation.failure == (occurrence == 1 ? CFLiteContextFailure::StreamsKeyCreation :
                                                 CFLiteContextFailure::TypeKeyCreation));
    CHECK(runtime.liveObjects() == 0);
  }
  for (unsigned allocations : {1u, 2u}) {
    runtime.failHostAllocationAfter(Operation::String, 2, allocations);
    const auto creation = CFLiteSetupContext::create(runtime.api());
    CHECK(!creation.context);
    CHECK(creation.failure == CFLiteContextFailure::HostAllocation);
    CHECK(runtime.liveObjects() == 0);
  }
}
void borrowed_lifetime(TestRun &test) {
  Fixture f;
  const auto counters = f.runtime.counters;
  {
    auto request = f.wrapped();
    CHECK(request);
    CHECK(request->rawDictionary() == f.request);
    CHECK(f.runtime.refs(asObject(f.request)) == 1);
    CHECK(f.runtime.counters.retain == counters.retain);
  }
  CHECK(f.runtime.counters.release == counters.release);
  CHECK(f.runtime.refs(asObject(f.request)) == 1);
  CHECK(!CFSetupRequest::borrowed({}, f.request));
  CHECK(!CFSetupRequest::borrowed(f.context, nullptr));
}
void descriptor_lifetime(TestRun &test) {
  Fixture f{100, 110, 111};
  const auto counters = f.runtime.counters;
  {
    auto streams = f.wrapped()->streams();
    CHECK(streams);
    CHECK(streams->size() == 3);
    CHECK(f.runtime.counters.retain == counters.retain + 3);
    for (unsigned i = 0; i < 3; ++i) {
      CHECK((*streams)[i]->identity() == f.dictionaries[i]);
      CHECK(f.runtime.refs(asObject(f.dictionaries[i])) == 2);
    }
  }
  CHECK(f.runtime.counters.release == counters.release + 3);
  for (const auto dictionary : f.dictionaries) CHECK(f.runtime.refs(asObject(dictionary)) == 1);
}
void descriptor_int64(TestRun &test) {
  Fixture f{0, -1, 0x10000006fLL, std::numeric_limits<std::int64_t>::min(),
            std::numeric_limits<std::int64_t>::max()};
  const auto streams = f.wrapped()->streams();
  CHECK(streams);
  CHECK((*streams)[0]->type() == 0);
  CHECK((*streams)[1]->type() == -1);
  CHECK((*streams)[2]->type() == 0x10000006fLL);
  CHECK((*streams)[2]->type() != 111);
  CHECK((*streams)[3]->type() == std::numeric_limits<std::int64_t>::min());
  CHECK((*streams)[4]->type() == std::numeric_limits<std::int64_t>::max());
  f.runtime.fail(Operation::Int64);
  CHECK(!(*streams)[0]->type());
  CHECK((*streams)[0]->type() == 0);
}
void missing_and_wrong_streams(TestRun &test) {
  FakeCFLite runtime;
  const auto context = CFLiteSetupContext::create(runtime.api()).context;
  auto *raw = runtime.dictionary();
  {
    const auto request = CFSetupRequest::borrowed(context, raw);
    CHECK(!request->streams());
    const auto value = runtime.integer(0);
    runtime.put(raw, "streams", value);
    runtime.release(value);
    CHECK(!request->streams());
  }
  runtime.release(asObject(raw));
}
void malformed_element(TestRun &test) {
  Fixture f{110};
  auto *array = const_cast<OpaqueCFArray *>(f.originalArray);
  const auto value = f.runtime.integer(111);
  f.runtime.append(array, value);
  f.runtime.release(value);
  const auto refs = f.runtime.totalReferences();
  CHECK(!f.wrapped()->streams());
  CHECK(f.runtime.totalReferences() == refs);
  CHECK(f.runtime.refs(asObject(f.dictionaries[0])) == 1);
}
void lookup_failures(TestRun &test) {
  Fixture f{110, 111};
  const auto refs = f.runtime.totalReferences();
  for (const auto operation : {Operation::TypedLookup, Operation::IndexLookup, Operation::Retain}) {
    f.runtime.fail(operation);
    CHECK(!f.wrapped()->streams());
    CHECK(f.runtime.totalReferences() == refs);
  }
  f.runtime.fail(Operation::IndexLookup, 2);
  CHECK(!f.wrapped()->streams());
  CHECK(f.runtime.totalReferences() == refs);
}
void descriptor_missing_type(TestRun &test) {
  FakeCFLite runtime;
  auto context = CFLiteSetupContext::create(runtime.api()).context;
  auto *dictionary = runtime.dictionary();
  const auto descriptor = CFSetupDescriptor::retaining(context, dictionary);
  CHECK(descriptor);
  CHECK(!descriptor->type());
  const auto wrong = asObject(runtime.string("not an integer in this fake"));
  runtime.put(dictionary, "type", wrong);
  runtime.release(wrong);
  CHECK(!descriptor->type());
  runtime.release(asObject(dictionary));
}
void copy_failures(TestRun &test) {
  for (const auto operation : {Operation::Copy, Operation::Array, Operation::Append, Operation::Set}) {
    Fixture f{100, 110};
    const auto request = f.wrapped();
    auto streams = request->streams();
    const auto refs = f.runtime.totalReferences();
    const auto live = f.runtime.liveObjects();
    f.runtime.fail(operation);
    CHECK(!request->withStreams(*streams));
    CHECK(f.runtime.totalReferences() == refs);
    CHECK(f.runtime.liveObjects() == live);
    CHECK(f.runtime.argumentsValid);
    if (f.runtime.lastCopy) CHECK(f.runtime.refs(asObject(f.runtime.lastCopy)) == 0);
    if (f.runtime.lastArray) CHECK(f.runtime.refs(asObject(f.runtime.lastArray)) == 0);
    checkOriginal(test, f);
  }
}
void later_append_failure(TestRun &test) {
  Fixture f{100, 110, 112};
  const auto request = f.wrapped();
  auto streams = request->streams();
  const auto refs = f.runtime.totalReferences();
  const auto appendCount = f.runtime.counters.append;
  f.runtime.fail(Operation::Append, 2);
  CHECK(!request->withStreams(*streams));
  CHECK(f.runtime.counters.append == appendCount + 2);
  CHECK(f.runtime.totalReferences() == refs);
  CHECK(f.runtime.refs(asObject(f.runtime.lastArray)) == 0);
  CHECK(f.runtime.refs(asObject(f.runtime.lastCopy)) == 0);
  checkOriginal(test, f);
}
void copy_success(TestRun &test) {
  Fixture f{100, 110, 111};
  const auto request = f.wrapped();
  const auto streams = request->streams();
  const auto refs = f.runtime.totalReferences();
  const auto copyCount = f.runtime.counters.copy;
  const auto arrayCount = f.runtime.counters.array;
  const auto appendCount = f.runtime.counters.append;
  const auto setCount = f.runtime.counters.set;
  const OpaqueCFDictionary *copied;
  const OpaqueCFArray *array;
  {
    const auto result = request->withStreams({(*streams)[1], (*streams)[0]});
    CHECK(result);
    const auto target = dynamic_cast<CFSetupRequest *>(result.get());
    CHECK(target);
    copied = target->rawDictionary();
    array = as<OpaqueCFArray>(f.runtime.get(copied, "streams"));
    const auto entries = f.runtime.elements(array);
    CHECK(entries.size() == 2);
    CHECK(entries[0] == asObject(f.dictionaries[1]));
    CHECK(entries[1] == asObject(f.dictionaries[0]));
    CHECK(f.runtime.get(copied, "opaque") == f.opaque);
    CHECK(f.runtime.refs(asObject(copied)) == 1);
    CHECK(f.runtime.refs(asObject(array)) == 1);
    CHECK(f.runtime.releases(asObject(array)) == 1); // Local reference dropped after SetValue.
    CHECK(f.runtime.counters.copy == copyCount + 1);
    CHECK(f.runtime.counters.array == arrayCount + 1);
    CHECK(f.runtime.counters.append == appendCount + 2);
    CHECK(f.runtime.counters.set == setCount + 1);
    CHECK(f.runtime.argumentsValid);
    checkOriginal(test, f);
  }
  CHECK(f.runtime.refs(asObject(copied)) == 0);
  CHECK(f.runtime.releases(asObject(copied)) == 1);
  CHECK(f.runtime.refs(asObject(array)) == 0);
  CHECK(f.runtime.releases(asObject(array)) == 2);
  CHECK(f.runtime.totalReferences() == refs);
}
void empty_copy(TestRun &test) {
  Fixture f;
  const auto request = f.wrapped();
  const auto refs = f.runtime.totalReferences();
  {
    const auto result = request->withStreams({});
    CHECK(result);
    const auto target = dynamic_cast<CFSetupRequest *>(result.get());
    const auto array = as<OpaqueCFArray>(f.runtime.get(target->rawDictionary(), "streams"));
    CHECK(f.runtime.elements(array).empty());
    CHECK(f.runtime.get(target->rawDictionary(), "opaque") == f.opaque);
    checkOriginal(test, f);
  }
  CHECK(f.runtime.totalReferences() == refs);
}
void incompatible_descriptors(TestRun &test) {
  Fixture f;
  const auto request = f.wrapped();
  const auto other = CFLiteSetupContext::create(f.runtime.api()).context;
  const auto descriptor = CFSetupDescriptor::retaining(other, f.dictionaries[0]);
  const auto nonTarget = std::make_shared<NonTargetDescriptor>(f.dictionaries[0]);
  const auto copies = f.runtime.counters.copy;
  CHECK(!request->withStreams({descriptor}));
  CHECK(!request->withStreams({nonTarget}));
  CHECK(!request->withStreams({nullptr}));
  CHECK(f.runtime.counters.copy == copies);
  checkOriginal(test, f);
}
void descriptor_allocation_exception(TestRun &test) {
  Fixture f{110, 111, 112};
  const auto request = f.wrapped();
  const auto refs = f.runtime.totalReferences();
  for (unsigned allocations : {1u, 2u}) {
    for (unsigned occurrence : {1u, 2u, 3u}) {
      f.runtime.failHostAllocationAfter(Operation::Retain, occurrence, allocations);
      CHECK(!request->streams());
      CHECK(f.runtime.totalReferences() == refs);
      for (const auto dictionary : f.dictionaries) CHECK(f.runtime.refs(asObject(dictionary)) == 1);
    }
  }
  failNextHostAllocation(); // Descriptor vector reserve, before any retains.
  CHECK(!request->streams());
  CHECK(f.runtime.totalReferences() == refs);
  CHECK(request->streams());
}
void copy_wrapper_allocation_exception(TestRun &test) {
  Fixture f{110, 112};
  const auto request = f.wrapped();
  const auto streams = request->streams();
  const auto refs = f.runtime.totalReferences();
  f.runtime.failHostAllocationAfter(Operation::Set);
  CHECK(!request->withStreams(*streams));
  CHECK(f.runtime.totalReferences() == refs);
  CHECK(f.runtime.refs(asObject(f.runtime.lastCopy)) == 0);
  CHECK(f.runtime.refs(asObject(f.runtime.lastArray)) == 0);
  checkOriginal(test, f);
}
void integration_stock(TestRun &test) {
  Fixture f{110};
  const auto copies = f.runtime.counters.copy;
  const auto result = filterTargetSetupRequest(f.context, f.request);
  CHECK(!result.parseFailed);
  CHECK(result.selection() == TargetRequestSelection::Original);
  CHECK(result.selectedRawRequest() == f.request);
  CHECK(result.secondaryDescriptors.empty());
  CHECK(f.runtime.counters.copy == copies);
  CHECK(f.runtime.refs(asObject(f.dictionaries[0])) == 1);
}
void integration_filter_order(TestRun &test) {
  {
    Fixture f{110, 111};
    const auto result = filterTargetSetupRequest(f.context, f.request);
    checkFiltered(test, f, result, {0}, {1});
  }
  {
    Fixture f{100, 110, 111};
    const auto result = filterTargetSetupRequest(f.context, f.request);
    checkFiltered(test, f, result, {0, 1}, {2});
  }
  {
    Fixture f{111, 110, 111};
    const auto result = filterTargetSetupRequest(f.context, f.request);
    checkFiltered(test, f, result, {1}, {0, 2});
  }
  {
    Fixture f{111};
    const auto result = filterTargetSetupRequest(f.context, f.request);
    checkFiltered(test, f, result, {}, {0});
  }
}
void integration_exact_int64(TestRun &test) {
  Fixture f{-2, 0, 109, 110, 111, 112, 0x10000006fLL};
  const auto result = filterTargetSetupRequest(f.context, f.request);
  checkFiltered(test, f, result, {0, 1, 2, 3, 5, 6}, {4});
}
void integration_fail_open(TestRun &test) {
  for (const auto operation : {Operation::TypedLookup, Operation::IndexLookup, Operation::Int64,
      Operation::Retain, Operation::Copy, Operation::Array, Operation::Append, Operation::Set}) {
    Fixture f;
    const auto refs = f.runtime.totalReferences();
    f.runtime.fail(operation);
    const auto result = filterTargetSetupRequest(f.context, f.request);
    CHECK(result.parseFailed);
    CHECK(result.selection() == TargetRequestSelection::Original);
    CHECK(result.selectedRawRequest() == f.request);
    CHECK(result.secondaryDescriptors.empty());
    CHECK(f.runtime.totalReferences() == refs);
    checkOriginal(test, f);
  }
  Fixture f;
  const auto result = filterTargetSetupRequest({}, f.request);
  CHECK(result.parseFailed);
  CHECK(result.selectedRawRequest() == f.request);
  const auto nullRequest = filterTargetSetupRequest(f.context, nullptr);
  CHECK(nullRequest.parseFailed);
  CHECK(!nullRequest.selectedRawRequest());
  failNextHostAllocation();
  const auto allocation = filterTargetSetupRequest(f.context, f.request);
  CHECK(allocation.parseFailed);
  CHECK(allocation.selectedRawRequest() == f.request);
}
void integration_descriptor_lifetime(TestRun &test) {
  Fixture f{111, 110, 111};
  auto result = filterTargetSetupRequest(f.context, f.request);
  CHECK(f.runtime.refs(asObject(f.dictionaries[0])) == 2);
  CHECK(f.runtime.refs(asObject(f.dictionaries[2])) == 2);
  f.releaseOriginal();
  f.context.reset();
  CHECK(f.runtime.refs(asObject(f.dictionaries[0])) == 1);
  CHECK(f.runtime.refs(asObject(f.dictionaries[2])) == 1);
  CHECK(result.secondaryDescriptors[0]->type() == 111);
  CHECK(result.secondaryDescriptors[1]->type() == 111);
  CHECK(result.secondaryDescriptors[0]->identity() == f.dictionaries[0]);
  result.filteredRequest.reset();
  CHECK(f.runtime.refs(asObject(f.dictionaries[1])) == 0);
  CHECK(result.secondaryDescriptors[0]->type() == 111);
  result.secondaryDescriptors.clear();
  CHECK(f.runtime.refs(asObject(f.dictionaries[0])) == 0);
  CHECK(f.runtime.refs(asObject(f.dictionaries[2])) == 0);
  CHECK(f.runtime.liveObjects() == 0);
}
void integration_malformed(TestRun &test) {
  FakeCFLite runtime;
  const auto context = CFLiteSetupContext::create(runtime.api()).context;
  auto *request = runtime.dictionary();
  auto check = [&] {
    const auto refs = runtime.totalReferences();
    const auto result = filterTargetSetupRequest(context, request);
    CHECK(result.parseFailed);
    CHECK(result.selectedRawRequest() == request);
    CHECK(result.selection() == TargetRequestSelection::Original);
    CHECK(result.secondaryDescriptors.empty());
    CHECK(runtime.totalReferences() == refs);
  };
  check(); // Missing streams.
  const auto wrong = runtime.integer(0);
  runtime.put(request, "streams", wrong);
  runtime.release(wrong);
  check(); // Wrong streams type.
  auto *array = runtime.array();
  auto *descriptor = runtime.dictionary();
  runtime.append(array, asObject(descriptor));
  runtime.put(request, "streams", asObject(array));
  runtime.release(asObject(array));
  runtime.release(asObject(descriptor));
  check(); // Missing type in an otherwise valid descriptor dictionary.
  runtime.release(asObject(request));
}
void integration_empty_and_repeated(TestRun &test) {
  {
    Fixture f(std::initializer_list<std::int64_t>{});
    const auto result = filterTargetSetupRequest(f.context, f.request);
    CHECK(!result.parseFailed);
    CHECK(result.selectedRawRequest() == f.request);
    CHECK(result.secondaryDescriptors.empty());
  }
  Fixture f{110, 111};
  const auto refs = f.runtime.totalReferences();
  for (unsigned call = 0; call < 3; ++call) {
    {
      const auto result = filterTargetSetupRequest(f.context, f.request);
      checkFiltered(test, f, result, {0}, {1});
    }
    CHECK(f.runtime.totalReferences() == refs);
  }
}
} // namespace

void test_cf_symbol_resolution(TestRun &);
#define GROUPS(X) X(context_ownership) X(context_release) X(context_failures) X(borrowed_lifetime) \
  X(descriptor_lifetime) X(descriptor_int64) X(missing_and_wrong_streams) X(malformed_element) \
  X(lookup_failures) X(descriptor_missing_type) X(copy_failures) X(later_append_failure) \
  X(copy_success) X(empty_copy) X(incompatible_descriptors) X(descriptor_allocation_exception) \
  X(copy_wrapper_allocation_exception) X(integration_stock) X(integration_filter_order) \
  X(integration_exact_int64) X(integration_fail_open) X(integration_descriptor_lifetime) \
  X(integration_malformed) X(integration_empty_and_repeated)
int main() {
  TestRun test;
  unsigned groups = 0;
#define RUN(name) name(test); ++groups;
  GROUPS(RUN)
#undef RUN
  test_cf_symbol_resolution(test); ++groups;
  std::cout << "mpr3 CFLite adapter: " << groups << " test groups / " << test.assertions << " assertions passed\n";
}
