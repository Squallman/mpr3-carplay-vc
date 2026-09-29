#include "mpr3/target/cflite_api.hpp"
#include <dlfcn.h>

namespace mpr3::target {
UntypedCFLiteFunction PosixCFLiteSymbolLookup::lookupFunction(const char *name) noexcept {
  // POSIX permits this conversion; target symbol presence/binding is unverified.
  return reinterpret_cast<UntypedCFLiteFunction>(dlsym(RTLD_DEFAULT, name));
}
const void *PosixCFLiteSymbolLookup::lookupData(const char *name) noexcept {
  return dlsym(RTLD_DEFAULT, name);
}
} // namespace mpr3::target
