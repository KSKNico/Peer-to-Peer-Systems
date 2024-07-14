#include "gtest/gtest.h"

#include "../message.hpp"
#include "../resultHandler.hpp"
#include "../timing.hpp"
#include "../mySocketConnector.hpp"
#include "../myConnectionHandler.hpp"
#include "../mySocketAcceptor.hpp"
#include "../hash.hpp"

#include "Poco/FIFOBuffer.h"
#include "Poco/Buffer.h"

#include <iostream>

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
    ASSERT_EQ(results1000.size(), 135);
    ASSERT_EQ(results2000.size(), 127);
    ASSERT_EQ(results3000.size(), 120);
}

TEST(TimingTests, IntervalTiming) {
    Timing timing = Timing();
    timing.updateIntervalMessageTime();
    ASSERT_FALSE(timing.intervalMessageTimePassed());
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ASSERT_TRUE(timing.intervalMessageTimePassed());
}


TEST(ConnectionTest, AcceptorAndConnector) {
    Poco::Net::SocketAddress address1("127.0.0.1:2000");
    Poco::Net::SocketAddress address2("127.0.0.1:2001");

    auto hash1 = Hash::hashSocketAddress(address1);
    auto hash2 = Hash::hashSocketAddress(address2);

    Poco::Net::SocketReactor reactor1;
    Poco::Net::SocketReactor reactor2;

    Poco::Net::ServerSocket socket1(address1);
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher> connections1;
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher> connections2;

    std::mutex connectionsMutex1;
    std::mutex connectionsMutex2;

    Poco::Thread reactorThread1;
    reactorThread1.start(reactor1);

    Poco::Thread reactorThread2;
    reactorThread2.start(reactor2);
    
    MySocketAcceptor acceptor(socket1, reactor1, connections1, connectionsMutex1);

    MySocketConnector connector(address1, reactor2, connections2, connectionsMutex2);




    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::unique_lock<std::mutex> lock1(connectionsMutex1);
    std::unique_lock<std::mutex> lock2(connectionsMutex2);
    std::cout << "Sending message to address1" << std::endl;
    while (connections1.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // there should be one connection now
    ASSERT_EQ(connections1.size(), 1);
    
    // the hash of the address should be the key
    ASSERT_EQ(connections1.begin()->first, hash2);

    connections1.at(Hash::hashSocketAddress(address2))->ioInterface.queueOutgoingMessage(Message("OK"));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    while (connections2.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto message = connections2.at(Hash::hashSocketAddress(address1))->ioInterface.dequeueIncomingMessage();

    ASSERT_EQ(message.data[0], 'O');
    ASSERT_EQ(message.data[1], 'K');
}
