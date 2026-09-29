#pragma once
#include "mpr3/setup_filter.hpp"
#include "mpr3/mocks.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

namespace mpr3_test {
inline void check(bool value, const char *expr, const char *file, int line) {
  if (!value) { std::cerr << file << ':' << line << ": failed: " << expr << '\n'; std::exit(1); }
}
#define CHECK(x) ::mpr3_test::check((x), #x, __FILE__, __LINE__)

inline std::shared_ptr<mpr3::MockCFDictionary> descriptor(int type, int marker = 0) {
  auto d = std::make_shared<mpr3::MockCFDictionary>();
  d->integers["type"] = type; d->integers["unknown_marker"] = marker;
  d->strings["opaque_field"] = "preserve-me"; return d;
}
inline mpr3::MockCFRequest request(std::initializer_list<std::shared_ptr<mpr3::MockCFDictionary>> ds) {
  mpr3::MockCFRequest r; auto a = std::make_shared<mpr3::MockCFArray>();
  a->values = ds; r.arrays["streams"] = std::move(a); return r;
}
inline mpr3::OriginalSetupFn recorder(std::vector<std::shared_ptr<mpr3::MockCFDictionary>> *seen, int result = 0) {
  return [seen, result](mpr3::AirPlayReceiverSessionPrivate *, const mpr3::MockCFRequest &r, mpr3::MockSetupResponse *out) {
    auto a = r.arrays.at("streams"); *seen = a->values; if (out) out->status = result; return result;
  };
}
} // namespace mpr3_test
