#include "test_support.hpp"
void test_state_lifecycle() {
  using namespace mpr3;
  auto stream=std::make_unique<MockScreenStream>(); auto *sp=stream.get();
  auto decoder=std::make_unique<MockDecoder>(); auto *dp=decoder.get();
  auto display=std::make_unique<MockDisplayable>();
  SecondarySessionOwner owner(std::move(stream), std::move(decoder), std::move(display));
  CHECK(owner.state()==SecondaryState::Idle); CHECK(owner.start()); CHECK(owner.state()==SecondaryState::Running);
  owner.stop(); CHECK(owner.state()==SecondaryState::Idle); CHECK(sp->starts==1 && sp->stops==1 && dp->starts==1 && dp->stops==1);
  auto badDisplay=std::make_unique<MockDisplayable>(); badDisplay->validResult=false;
  SecondarySessionOwner failed(std::make_unique<MockScreenStream>(), std::make_unique<MockDecoder>(), std::move(badDisplay));
  CHECK(!failed.start()); CHECK(failed.state()==SecondaryState::Failed);
}
