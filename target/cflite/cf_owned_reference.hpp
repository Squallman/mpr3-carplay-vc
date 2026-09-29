#pragma once
#include "mpr3/target/cflite_api.hpp"

namespace mpr3::target::detail {
template<class T> const OpaqueCFObject *object(const T *value) noexcept {
  return reinterpret_cast<const OpaqueCFObject *>(value);
}
// References are scoped before any host allocation that might throw.
class OwnedReference final {
 public:
  OwnedReference(const CFLiteApi &api, const OpaqueCFObject *value) noexcept
      : api_(api), value_(value) {}
  ~OwnedReference() { if (value_) api_.release(value_); }
  OwnedReference(const OwnedReference &) = delete;
  OwnedReference &operator=(const OwnedReference &) = delete;
  void relinquish() noexcept { value_ = nullptr; }
 private:
  const CFLiteApi &api_;
  const OpaqueCFObject *value_;
};
} // namespace mpr3::target::detail
