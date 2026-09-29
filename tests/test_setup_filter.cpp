#include "test_support.hpp"
void test_stock_110_preserved(); void test_secondary_111_extracted(); void test_multiple_descriptors();
void test_no_111(); void test_original_failure(); void test_secondary_failure_fail_open(); void test_state_lifecycle();
int main() {
  test_stock_110_preserved(); test_secondary_111_extracted(); test_multiple_descriptors();
  test_no_111(); test_original_failure(); test_secondary_failure_fail_open(); test_state_lifecycle();
  std::cout << "mpr3 offline prototype: 7 test groups passed\n";
}
