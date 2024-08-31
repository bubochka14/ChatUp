find_package(Qt6 REQUIRED COMPONENTS Test) 

cmake_minimum_required(VERSION 3.14)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(TEST_SOURCES
    chatwindow_manual.cpp
    authentication_manual.cpp
    main.cpp
C:/Users/Denis/source/repos/QtChatClient/lib/ChatClient/res.qrc
${APP_SOURCE}/app.h
    ${APP_SOURCE}/app.cpp
)

enable_testing()
add_executable(
    manual_test
    ${TEST_SOURCES}
)
target_include_directories(manual_test PRIVATE ${APP_SOURCE} ../)
target_link_libraries(
  manual_test
  Qt6::Quick
  Qt6::Core
  Qt6::Test
  GTest::gtest_main
  GTest::gmock_main
  ChatClient
)
include(GoogleTest)
add_test(NAME manual_test COMMAND manual_test)