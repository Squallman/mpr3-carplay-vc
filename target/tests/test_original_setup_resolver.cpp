#include "test_support.hpp"
using namespace target_test;

void test_resolver_exact(TestRun &test) {
  FakeLookup lookup;
  lookup.candidate = &stock;
  OriginalSetupResolver resolver(lookup);
  const auto result = resolver.resolve(&wrapper);
  CHECK(result.status == ResolutionStatus::Resolved);
  CHECK(result.original == &stock);
  CHECK(lookup.calls == 1);
}
void test_resolver_missing(TestRun &test) {
  FakeLookup lookup;
  OriginalSetupResolver resolver(lookup);
  const auto result = resolver.resolve(&wrapper);
  CHECK(result.status == ResolutionStatus::MissingSymbol);
  CHECK(result.original == nullptr);
  CHECK(lookup.calls == 1);
}
void test_resolver_self(TestRun &test) {
  FakeLookup lookup;
  lookup.candidate = &wrapper;
  OriginalSetupResolver resolver(lookup);
  const auto result = resolver.resolve(&wrapper);
  CHECK(result.status == ResolutionStatus::SelfReference);
  CHECK(result.original == nullptr);
  CHECK(lookup.calls == 1);
}
void test_resolver_repeated(TestRun &test) {
  FakeLookup lookup;
  OriginalSetupResolver resolver(lookup);
  lookup.candidate = &stock;
  CHECK(resolver.resolve(&wrapper).original == &stock);
  lookup.candidate = nullptr;
  CHECK(resolver.resolve(&wrapper).status == ResolutionStatus::MissingSymbol);
  lookup.candidate = &wrapper;
  CHECK(resolver.resolve(&wrapper).status == ResolutionStatus::SelfReference);
  lookup.candidate = &stock;
  CHECK(resolver.resolve(&wrapper).original == &stock);
  CHECK(lookup.calls == 4);
  FakeLookup other;
  OriginalSetupResolver independent(other);
  CHECK(independent.resolve(&wrapper).original == nullptr);
  CHECK(other.calls == 1);
  CHECK(lookup.calls == 4);
}
