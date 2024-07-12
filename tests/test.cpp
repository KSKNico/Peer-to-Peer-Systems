#include "gtest/gtest.h"

#include "../message.hpp"
#include "../resultHandler.hpp"

#include "Poco/FIFOBuffer.h"
#include "Poco/Buffer.h"

TEST(MessageTests, FromString) {
    Message m2("OK");
    ASSERT_EQ(m2.data[0], 'O');
    ASSERT_EQ(m2.data[1], 'K');

    for (int i = 2; i < Message::FIXED_MESSAGE_SIZE; i++) {
        m2.data[i] = '\0';
    }
}

TEST(MessageTests, ToAndFromBuffer) {
    Message m1("OK");
    auto buffer = Message::toBuffer(m1);
    auto fifoBuffer = Poco::BasicFIFOBuffer<char>(Message::FIXED_MESSAGE_SIZE);

    // fill fifo buffer 
    fifoBuffer.write(buffer.begin(), buffer.size());

    auto m2 = Message::fromBuffer(fifoBuffer);

    for (int i = 0; i < Message::FIXED_MESSAGE_SIZE; i++) {
        ASSERT_EQ(m1.data[i], m2.data[i]);
    }
}

TEST(MessageTests, IsEmpty) {
    auto msg = Message();

    printf("msg.data[0]: %d\n", msg.data[0]);

    ASSERT_TRUE(msg.isEmpty());
}

TEST(ResultTests, PrimeCalculation) {
    ResultHandler resultHandler = ResultHandler();
    resultHandler.submitCalculation(0);
    resultHandler.submitCalculation(1000);
    resultHandler.submitCalculation(2000);
    resultHandler.submitCalculation(3000);
    resultHandler.submitCalculation(4000);

    // sleep a while
    std::this_thread::sleep_for(std::chrono::seconds(1));

    resultHandler.updateResultsWithFutures();

    ASSERT_TRUE(resultHandler.hasResults(0));
    ASSERT_TRUE(resultHandler.hasResults(1000));
    ASSERT_TRUE(resultHandler.hasResults(2000));
    ASSERT_TRUE(resultHandler.hasResults(3000));
    ASSERT_TRUE(resultHandler.hasResults(4000));

    auto results0 = resultHandler.getResults(0).value();
    auto results1000 = resultHandler.getResults(1000).value();
    auto results2000 = resultHandler.getResults(2000).value();
    auto results3000 = resultHandler.getResults(3000).value();

    ASSERT_EQ(results0.size(), 168);
}