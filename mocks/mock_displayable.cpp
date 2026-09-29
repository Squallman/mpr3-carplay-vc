#include "mpr3/mocks.hpp"
#include <stdexcept>

namespace mpr3 {
MockDisplayable::MockDisplayable(DisplayableHandle handle, std::shared_ptr<MockLifecycle> lifecycle)
    : handle_(std::move(handle)), lifecycle_(std::move(lifecycle)) { ++lifecycle_->displayablesCreated; }
MockDisplayable::~MockDisplayable() {
  ++lifecycle_->displayablesDestroyed;
  lifecycle_->record(CleanupStep::DisplayableDestroy);
}
DisplayableCreateResult MockDisplayableProvider::createByConfiguredName(const DisplayableHandle &config) {
  ++calls;
  lastConfig = config;
  if (throwOnCreate) throw std::runtime_error("mock displayable create failure");
  if (!createResult) return {};
  if (returnNull) return {DisplayableCreateStatus::Created, nullptr};
  DisplayableHandle handle = config;
  if (overrideId) handle.resolvedId = resolvedId;
  if (returnedName) handle.configuredName = *returnedName;
  return {DisplayableCreateStatus::Created,
          std::make_unique<MockDisplayable>(std::move(handle), lifecycle)};
}
} // namespace mpr3
