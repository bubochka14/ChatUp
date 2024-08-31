#include <gtest/gtest.h>
#include "servermethodcaller.h"
#include <chrono>
#include <thread>
using namespace testing;
using namespace std::chrono_literals;

class CallerFixture : public Test
{
protected:
    void SetUp() override
    {
        caller = new ServerMethodCaller;
    }
    void TearDown() override
    {
        delete caller;
    }
    ServerMethodCaller* caller;
};
TEST_F(CallerFixture, NoResponseTest)
{
    caller->setTimeout(100);
    EXPECT_NO_FATAL_FAILURE(
    caller->sendChatMessage(0, "")
        .then([](void*) {
         FAIL();
            })
        .onFailed([](const MethodCallFailure& f)
            {
                EXPECT_EQ(f.error, QNetworkReply::TimeoutError);
            });
    caller->registerUser({})
        .then([](void*) {
        FAIL();
        })
        .onFailed([](const MethodCallFailure& f)
            {
                EXPECT_EQ(f.error, QNetworkReply::TimeoutError);
            });
    caller->loginUser({})
        .then([](void*) {
        FAIL();
            })
        .onFailed([](const MethodCallFailure& f)
            {
                FAIL();
                EXPECT_EQ(f.error, QNetworkReply::TimeoutError);
            });
    caller->getUserInfo(0)
        .then([](void*) {
        FAIL();
            })
        .onFailed([](const MethodCallFailure& f)
            {
                EXPECT_EQ(f.error, QNetworkReply::TimeoutError);
            });
    caller->getRoomHistory(0)
        .then([](MessageList) {
        FAIL();
            })
        .onFailed([](const MethodCallFailure& f)
            {
                EXPECT_EQ(f.error, QNetworkReply::TimeoutError);
            });
    caller->getUserRooms(0)  
        .then([](RoomList) {
        FAIL();
            })
        .onFailed([](const MethodCallFailure& f)
            {
                EXPECT_EQ(f.error, QNetworkReply::TimeoutError);
            });
    caller->getCurrentUserInfo().then([](void*) {
        FAIL();
        })
        .onFailed([](const MethodCallFailure& f)
            {
                EXPECT_EQ(f.error, QNetworkReply::TimeoutError);
            });
        );
    std::this_thread::sleep_for(1s);
}