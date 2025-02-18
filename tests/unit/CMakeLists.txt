find_package(Qt6 REQUIRED COMPONENTS Test) 

cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0135 NEW)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(TEST_SOURCES
    serialization_test.cpp
    ${APP_SOURCES}
)

enable_testing()

add_executable(
    app_test
    ${TEST_SOURCES}
)
target_include_directories(app_test PRIVATE ${APP_SOURCE})

target_link_libraries(app_test
  Qt6::Quick
  Qt6::Core
  Qt6::Test
  GTest::gtest_main
  GTest::gmock_main
  ChatClient::Core
  ChatClient::Network
)
include(GoogleTest)
add_test(NAME app_test COMMAND app_test)