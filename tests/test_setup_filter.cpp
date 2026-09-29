#include "test_support.hpp"

#define TEST_GROUPS(X) \
  X(stock_110_preserved) X(secondary_111_extracted) X(multiple_descriptors) \
  X(no_111) X(original_failure) X(secondary_failure_fail_open) X(state_lifecycle) \
  X(invalid_requests) X(full_start) X(displayable_failure) X(factory_failure) \
  X(stream_start_failure) X(decoder_start_failure) X(video_unavailable) \
  X(activation_failure) X(missing_endpoint) X(missing_displayable_id) \
  X(config_validation) X(resolved_handle) X(multi_policy) X(controller_invalid_request) \
  X(secondary_exceptions) X(descriptor_lifetime) X(capability_placeholders) \
  X(pipeline_raii) X(destructor_cleanup) X(retry_after_failure) \
  X(success_event_order) X(failure_event_order) X(observer_failure) \
  X(state_observation) X(stock_completed_first) X(observer_reentry) \
  X(alternative_request_adapter)

#define DECLARE(name) void test_##name(mpr3_test::TestRun &);
TEST_GROUPS(DECLARE)
#undef DECLARE

int main() {
  mpr3_test::TestRun test;
  std::size_t groups = 0;
#define RUN(name) test_##name(test); ++groups;
  TEST_GROUPS(RUN)
#undef RUN
  std::cout << "mpr3 offline prototype v2: " << groups << " test groups / "
            << test.assertions << " assertions passed\n";
}
