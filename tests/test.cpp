#include "gtest/gtest.h"

#include "../message.hpp"

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