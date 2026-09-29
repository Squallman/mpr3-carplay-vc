#include "fake_cflite.hpp"
#include <cstdlib>
#include <new>

namespace {
thread_local unsigned failAfter = 0;
void *allocate(std::size_t size) {
  if (failAfter && --failAfter == 0) throw std::bad_alloc();
  if (void *value = std::malloc(size ? size : 1)) return value;
  throw std::bad_alloc();
}
}
namespace cflite_test {
void failNextHostAllocation(unsigned allocationsUntilFailure) noexcept { failAfter = allocationsUntilFailure; }
void cancelHostAllocationFailure() noexcept { failAfter = 0; }
}
// Single-threaded host test instrumentation. No target sources use these hooks.
void *operator new(std::size_t size) { return allocate(size); }
void *operator new[](std::size_t size) { return allocate(size); }
void operator delete(void *value) noexcept { std::free(value); }
void operator delete[](void *value) noexcept { std::free(value); }
void operator delete(void *value, std::size_t) noexcept { std::free(value); }
void operator delete[](void *value, std::size_t) noexcept { std::free(value); }
