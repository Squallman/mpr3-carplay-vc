cmake_minimum_required(VERSION 3.16)
project(mpr3_offline_prototype LANGUAGES CXX)

option(MPR3_BUILD_TARGET_CONTRACTS "Build offline target contracts, tests and entry OBJECT" OFF)
option(MPR3_ENABLE_POSIX_TARGET_RESOLVER "Build opt-in POSIX RTLD_NEXT lookup" OFF)
option(MPR3_ENABLE_POSIX_CFLITE_RESOLVER "Build opt-in POSIX RTLD_DEFAULT CFLite lookup" OFF)
if(MPR3_ENABLE_TARGET_LOADER)
  message(FATAL_ERROR "MPR3_ENABLE_TARGET_LOADER was replaced by explicit target-contract options")
endif()
if((MPR3_ENABLE_POSIX_TARGET_RESOLVER OR MPR3_ENABLE_POSIX_CFLITE_RESOLVER)
    AND NOT MPR3_BUILD_TARGET_CONTRACTS)
  message(FATAL_ERROR "POSIX resolver requires MPR3_BUILD_TARGET_CONTRACTS=ON")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mpr3_core
  sidecar/secondary_session.cpp
  sidecar/secondary_controller.cpp
  sidecar/runtime_config.cpp
  hook/airplay_setup_hook.cpp)
target_include_directories(mpr3_core PUBLIC include)

add_library(mpr3_mocks
  mocks/mock_corefoundation.cpp
  mocks/mock_airplay.cpp
  mocks/mock_screen_stream.cpp
  mocks/mock_displayable.cpp
  mocks/mock_pipeline_factory.cpp
  mocks/mock_video_encoding.cpp)
target_include_directories(mpr3_mocks PUBLIC mocks/include)
target_link_libraries(mpr3_mocks PUBLIC mpr3_core)

add_executable(mpr3_tests
  tests/test_setup_filter.cpp
  tests/test_stock_110_preserved.cpp
  tests/test_secondary_111_extracted.cpp
  tests/test_multiple_descriptors.cpp
  tests/test_no_111.cpp
  tests/test_original_failure.cpp
  tests/test_secondary_failure_fail_open.cpp
  tests/test_state_lifecycle.cpp
  tests/test_secondary_controller.cpp
  tests/test_events.cpp)
target_link_libraries(mpr3_tests PRIVATE mpr3_core mpr3_mocks)
target_include_directories(mpr3_tests PRIVATE include tests)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  foreach(target mpr3_core mpr3_mocks mpr3_tests)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endforeach()
endif()
enable_testing()
add_test(NAME mpr3_tests COMMAND mpr3_tests)

if(MPR3_BUILD_TARGET_CONTRACTS)
  # Force STATIC even with BUILD_SHARED_LIBS; no loadable hook is produced.
  add_library(mpr3_target_contracts STATIC
    target/airplay/original_setup_resolver.cpp target/airplay/setup_bridge.cpp)
  target_include_directories(mpr3_target_contracts PUBLIC target/include include)
  if(MPR3_ENABLE_POSIX_TARGET_RESOLVER)
    if(NOT UNIX)
      message(FATAL_ERROR "POSIX target lookup requires a POSIX host")
    endif()
    target_sources(mpr3_target_contracts PRIVATE target/airplay/posix_setup_symbol_lookup.cpp)
    target_compile_definitions(mpr3_target_contracts PUBLIC MPR3_ENABLE_POSIX_TARGET_RESOLVER=1)
    target_link_libraries(mpr3_target_contracts PRIVATE ${CMAKE_DL_LIBS})
  endif()
  add_library(mpr3_target_entry_object OBJECT target/abi/airplay_setup_entry.cpp)
  target_include_directories(mpr3_target_entry_object PRIVATE target/include include)
  add_library(mpr3_target_header_object OBJECT target/tests/abi_header_consumer.cpp)
  target_include_directories(mpr3_target_header_object PRIVATE target/include include)
  add_executable(mpr3_target_tests
    target/tests/test_original_setup_resolver.cpp
    target/tests/test_setup_bridge.cpp target/tests/test_target_contracts.cpp)
  target_include_directories(mpr3_target_tests PRIVATE target/tests)
  target_link_libraries(mpr3_target_tests PRIVATE mpr3_target_contracts mpr3_core)
  add_library(mpr3_target_cf_adapter STATIC
    target/cflite/cflite_api.cpp target/cflite/cf_setup_context.cpp
    target/cflite/cf_setup_adapter.cpp target/cflite/target_setup_filter.cpp)
  target_include_directories(mpr3_target_cf_adapter PUBLIC target/include)
  target_link_libraries(mpr3_target_cf_adapter PUBLIC mpr3_core)
  if(MPR3_ENABLE_POSIX_CFLITE_RESOLVER)
    if(NOT UNIX)
      message(FATAL_ERROR "POSIX CFLite lookup requires a POSIX host")
    endif()
    target_sources(mpr3_target_cf_adapter PRIVATE target/cflite/posix_cflite_symbol_lookup.cpp)
    target_compile_definitions(mpr3_target_cf_adapter PUBLIC MPR3_ENABLE_POSIX_CFLITE_RESOLVER=1)
    target_link_libraries(mpr3_target_cf_adapter PRIVATE ${CMAKE_DL_LIBS})
  endif()
  add_library(mpr3_target_cflite_header_object OBJECT target/tests/cflite_abi_header_consumer.cpp)
  target_include_directories(mpr3_target_cflite_header_object PRIVATE target/include)
  add_executable(mpr3_target_cf_tests
    target/tests/fake_cflite.cpp target/tests/host_allocation_failure.cpp
    target/tests/test_cflite_api.cpp target/tests/test_cf_setup_adapter.cpp)
  target_include_directories(mpr3_target_cf_tests PRIVATE target/tests)
  target_link_libraries(mpr3_target_cf_tests PRIVATE mpr3_target_cf_adapter)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    foreach(target mpr3_target_contracts mpr3_target_entry_object mpr3_target_header_object
        mpr3_target_tests mpr3_target_cf_adapter mpr3_target_cflite_header_object mpr3_target_cf_tests)
      target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endforeach()
  endif()
  add_test(NAME mpr3_target_tests COMMAND mpr3_target_tests)
  add_test(NAME mpr3_target_cf_tests COMMAND mpr3_target_cf_tests)
  if(UNIX)
    add_test(NAME mpr3_target_dependency_boundary
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/check-target-dependencies.sh"
              "$<TARGET_FILE:mpr3_target_contracts>")
    add_test(NAME mpr3_cf_adapter_dependency_boundary
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/check-cf-adapter-dependencies.sh"
              "$<TARGET_FILE:mpr3_target_cf_adapter>")
  endif()
endif()
