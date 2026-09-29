#pragma once
#include "mpr3/secondary_status.hpp"
#include <memory>
namespace mpr3 {
// Adapters must release construction resources in their destructors and permit
// noexcept stop after a partially successful start. Only attempted starts are
// stopped; objects that were never started are simply destroyed.
class IScreenStream {
 public:
  virtual ~IScreenStream() = default;
  virtual bool start() = 0;
  virtual void stop() noexcept = 0;
};
class IDecoder {
 public:
  virtual ~IDecoder() = default;
  virtual bool start() = 0;
  virtual void stop() noexcept = 0;
};
class IRenderContext { public: virtual ~IRenderContext() = default; };
// Pipeline ownership only; the controller keeps the displayable alive until
// after stream/decoder/render context destruction.
class SecondarySessionOwner {
 public:
  SecondarySessionOwner(std::unique_ptr<IScreenStream>, std::unique_ptr<IDecoder>,
                        std::unique_ptr<IRenderContext>);
  ~SecondarySessionOwner();
  SecondarySessionOwner(const SecondarySessionOwner &) = delete;
  SecondarySessionOwner &operator=(const SecondarySessionOwner &) = delete;
  SecondaryFailure start();
  void stop() noexcept;
 private:
  std::unique_ptr<IScreenStream> stream_;
  std::unique_ptr<IDecoder> decoder_;
  std::unique_ptr<IRenderContext> context_;
  bool streamAttempted_ = false;
  bool decoderAttempted_ = false;
};
} // namespace mpr3
