#pragma once

#include "mpr3/airplay_types.hpp"
#include <memory>

namespace mpr3 {

enum class SecondaryState { Idle, DescriptorReceived, Starting, Running, Stopping, Failed };

class IScreenStream { public: virtual ~IScreenStream() = default; virtual bool start() = 0; virtual void stop() = 0; };
class IDecoder { public: virtual ~IDecoder() = default; virtual bool start() = 0; virtual void stop() = 0; };
class IDisplayable { public: virtual ~IDisplayable() = default; virtual bool valid() const = 0; };

class SecondarySessionOwner {
 public:
  SecondarySessionOwner(std::unique_ptr<IScreenStream> stream,
                        std::unique_ptr<IDecoder> decoder,
                        std::unique_ptr<IDisplayable> displayable);
  bool start();
  void stop();
  SecondaryState state() const { return state_; }

 private:
  SecondaryState state_ = SecondaryState::Idle;
  std::unique_ptr<IScreenStream> stream_;
  std::unique_ptr<IDecoder> decoder_;
  std::unique_ptr<IDisplayable> displayable_;
};

} // namespace mpr3
