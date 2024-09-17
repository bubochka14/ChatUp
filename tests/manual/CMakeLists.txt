find_package(Qt6 REQUIRED COMPONENTS Test) 

cmake_minimum_required(VERSION 3.14)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
list(REMOVE_ITEM APP_SOURCES "${APP_SOURCE}/main.cpp")
set(TEST_SOURCES
    chatwindow_manual.cpp
    authentication_manual.cpp
    main.cpp
    ${APP_SOURCES}
)

enable_testing()
add_executable(
    manual_test
    ${TEST_SOURCES}
)
target_include_directories(manual_test PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    BUNDLE DESTINATION CMAKE_INSTALL_BINDIR
    RUNTIME DESTINATION CMAKE_INSTALL_BINDIR
)
target_include_directories(manual_test PRIVATE ${APP_SOURCE} ../)
target_link_libraries(manual_test PRIVATE
	Qt6::Core 
	Qt6::Qml 
	Qt6::Quick 
	ChatClient	
	quickfuture
	QWindowKit::Quick
	QWindowKit::Core
    GTest::gtest_main
    GTest::gmock_main
)
include(GoogleTest)
add_test(NAME manual_test COMMAND manual_test)