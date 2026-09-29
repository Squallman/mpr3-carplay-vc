cmake_minimum_required(VERSION 3.16)
project(mpr3_offline_prototype LANGUAGES CXX)

option(MPR3_ENABLE_TARGET_LOADER "Compile optional host dlopen/dlsym resolver" OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mpr3_core
  sidecar/secondary_session.cpp
  sidecar/displayable.cpp
  sidecar/video_encoding.cpp
  hook/airplay_setup_hook.cpp
  hook/original_resolver.cpp
  mocks/mock_corefoundation.cpp
  mocks/mock_airplay.cpp
  mocks/mock_screen_stream.cpp
  mocks/mock_displayable.cpp
  mocks/mock_video_encoding.cpp)
target_include_directories(mpr3_core PUBLIC include)
target_compile_definitions(mpr3_core PUBLIC MPR3_ENABLE_TARGET_LOADER=$<BOOL:${MPR3_ENABLE_TARGET_LOADER}>)
if(MPR3_ENABLE_TARGET_LOADER)
  target_link_libraries(mpr3_core PRIVATE ${CMAKE_DL_LIBS})
endif()

add_executable(mpr3_tests
  tests/test_setup_filter.cpp
  tests/test_stock_110_preserved.cpp
  tests/test_secondary_111_extracted.cpp
  tests/test_multiple_descriptors.cpp
  tests/test_no_111.cpp
  tests/test_original_failure.cpp
  tests/test_secondary_failure_fail_open.cpp
  tests/test_state_lifecycle.cpp)
target_link_libraries(mpr3_tests PRIVATE mpr3_core)
target_include_directories(mpr3_tests PRIVATE include tests)
enable_testing()
add_test(NAME mpr3_tests COMMAND mpr3_tests)
