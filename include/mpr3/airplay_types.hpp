#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mpr3 {

// PROVEN target shape (libairplay.so:0x58dc0), but intentionally opaque here.
struct AirPlayReceiverSessionPrivate;

// MOCKED CoreFoundation stand-ins. They preserve identity and unknown fields;
// they are not an Apple/CoreFoundation ABI implementation.
struct MockCFDictionary {
  std::unordered_map<std::string, std::int64_t> integers;
  std::unordered_map<std::string, std::string> strings;
};

struct MockCFArray {
  std::vector<std::shared_ptr<MockCFDictionary>> values;
};

struct MockCFRequest {
  std::unordered_map<std::string, std::shared_ptr<MockCFArray>> arrays;
};

using CFDictionaryRef = MockCFDictionary*;
using CFArrayRef = MockCFArray*;

struct MockSetupResponse { int status = 0; };

struct DescriptorRef {
  std::shared_ptr<MockCFDictionary> value;
  explicit DescriptorRef(std::shared_ptr<MockCFDictionary> d = {}) : value(std::move(d)) {}
  int type() const;
};

} // namespace mpr3
