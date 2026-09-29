#pragma once
#include <memory>
#include <optional>
#include <string>
namespace mpr3 {
struct DisplayableHandle {
  std::string configuredName;
  std::optional<int> resolvedId;
};
class IDisplayable {
 public:
  // Adapter destructor releases the created displayable. No target destroy ABI
  // or safe production displayable is assumed by this host contract.
  virtual ~IDisplayable() = default;
  virtual const DisplayableHandle &handle() const noexcept = 0;
};
enum class DisplayableCreateStatus { Created, Failed };
struct DisplayableCreateResult {
  DisplayableCreateStatus status = DisplayableCreateStatus::Failed;
  std::unique_ptr<IDisplayable> displayable;
};
class IDisplayableProvider {
 public:
  virtual ~IDisplayableProvider() = default;
  virtual DisplayableCreateResult createByConfiguredName(const DisplayableHandle &) = 0;
};
} // namespace mpr3
