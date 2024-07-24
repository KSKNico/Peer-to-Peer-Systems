#include <cmath>
#include <memory>
#include <unordered_map>

#include "../src/fingerTable.hpp"
#include "../src/hash.hpp"
#include "../src/networking/acceptor.hpp"
#include "../src/networking/connection.hpp"
#include "../src/networking/connectionManager.hpp"
#include "../src/networking/connector.hpp"
#include "../src/networking/message.hpp"
#include "Poco/FIFOBuffer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Thread.h"
#include "gtest/gtest.h"

TEST(Message, IDMessageTest) {
    IDMessage message(Poco::Net::SocketAddress("127.0.0.1:1234"));
    std::string str = message.toString();
    IDMessage newMessage = IDMessage::fromString(str);

    EXPECT_EQ(message.toString(), newMessage.toString());
}

TEST(Hash, SocketAddress) {
    Poco::Net::SocketAddress addr("127.0.0.1:1234");

    std::unordered_map<Poco::Net::SocketAddress, int, Hash::SocketAddressHasher> map;
    map.insert({addr, 1});

    EXPECT_EQ(map.at(addr), 1);

    auto addr2 = Poco::Net::SocketAddress();
    ASSERT_EQ(addr2.toString(), "0.0.0.0:0");
}

TEST(Hash, Functions) {
    const auto hash1 = Hash(Poco::Net::SocketAddress("1.1.1.1:1234"));
    const auto hash2 = Hash(Poco::Net::SocketAddress("1.1.1.1:1235"));

    ASSERT_NE(hash1, hash2);

    {
        auto hash3 = hash1 + hash2;
        auto hash4 = hash3 - hash2;

        ASSERT_EQ(hash1, hash4);
    }

    {
        auto hash = Hash::fromExponent(3);
        ASSERT_EQ(hash.getHashValue(), 8);
    }

    {
        auto hash4 = Hash::fromExponent(63);
        auto hash3 = Hash::fromExponent(0);

        ASSERT_NE(hash3, hash4);

        auto distanceHash = hash3.distance(hash4);
        ASSERT_EQ((long double)distanceHash.getHashValue(), std::exp2l(63) - 1.0);
    }
}

TEST(Connection, ConnectionWithMessage) {
    Acceptor acceptor(1234);
    Connector connector;

    // start this in a separate thread
    std::unique_ptr<Connection> connection1;
    std::cout << "Starting thread" << std::endl;
    std::thread thread([&] {
        connection1 =
            connector.connect(Poco::Net::SocketAddress("127.0.0.1:1234"));
    });
    auto connection2 = acceptor.accept();

    thread.join();
    ASSERT_TRUE(connection1 != nullptr);
    EXPECT_TRUE(connection1->isConnected());
    EXPECT_TRUE(connection2->isConnected());

    std::cout << "Connection 1 is connected to " << connection1->getPeerAddress().toString() << std::endl;
    std::cout << "Connection 2 is connected to " << connection2->getPeerAddress().toString() << std::endl;

    if (connection1->isWritable()) {
        IDMessage msg = IDMessage(Poco::Net::SocketAddress("1.1.1.1:2222"));
        connection1->sendMessage(msg);
        std::cout << "Message sent" << std::endl;
    } else {
        std::cout << "Connection 1 is not writable" << std::endl;
    }

    if (connection2->isReadable()) {
        auto message = connection2->receiveMessage();
        std::cout << "Message received" << std::endl;

        ASSERT_TRUE(message.isComplete() && !message.isError());
    } else {
        std::cout << "Connection 2 is not readable" << std::endl;
    }
}

TEST(Connection, StressTest) {
    Acceptor acceptor(1234);
    Connector connector;

    // start this in a separate thread
    std::unique_ptr<Connection> connection1;
    std::cout << "Starting thread" << std::endl;
    std::thread thread1([&] {
        connection1 =
            connector.connect(Poco::Net::SocketAddress("127.0.0.1:1234"));
    });
    auto connection2 = acceptor.accept();

    thread1.join();
    ASSERT_TRUE(connection1 != nullptr);
    EXPECT_TRUE(connection1->isConnected());
    EXPECT_TRUE(connection2->isConnected());

    std::cout << "Connection 1 is connected to " << connection1->getPeerAddress().toString() << std::endl;
    std::cout << "Connection 2 is connected to " << connection2->getPeerAddress().toString() << std::endl;
    constexpr int numMessages = 1000;
    // thread1 always sends messages from connection1 to connection2
    std::thread thread2([&] {
        for (int i = 0; i < numMessages; ++i) {
            if (connection1->isWritable()) {
                connection1->sendMessage(IDMessage(Poco::Net::SocketAddress("1.1.1.1:2222")));
            } else {
                std::cout << "Can't send message." << std::endl;
            }
        }
    });

    Poco::Thread::sleep(100);

    int received = 0;
    while (received < numMessages) {
        auto msg = connection2->receiveMessage();
        if (msg.isComplete()) {
            ++received;
        } else {
            break;
        }
    }
    thread2.join();

    EXPECT_EQ(received, numMessages);

    std::cout << "Received " << std::to_string(received) << " messages." << std::endl;
}

TEST(Connection, StreamTest) {
    Acceptor acceptor(1234);
    Connector connector;

    // start this in a separate thread
    std::unique_ptr<Connection> connection1;
    std::cout << "Starting thread" << std::endl;
    std::thread thread1([&] {
        connection1 =
            connector.connect(Poco::Net::SocketAddress("127.0.0.1:1234"));
    });
    auto connection2 = acceptor.accept();

    thread1.join();
    ASSERT_TRUE(connection1 != nullptr);
    EXPECT_TRUE(connection1->isConnected());
    EXPECT_TRUE(connection2->isConnected());

    std::cout << "Connection 1 is connected to " << connection1->getPeerAddress().toString() << std::endl;
    std::cout << "Connection 2 is connected to " << connection2->getPeerAddress().toString() << std::endl;

    connection1->_sendString("ID,");

    Message message;
    if (connection2->isReadable()) {
        message = connection2->receiveMessage();
        std::cout << "Message received" << std::endl;

        EXPECT_TRUE(!message.isComplete());
    } else {
        std::cout << "Connection 2 is not readable" << std::endl;
    }

    if (connection1->isWritable()) {
        connection1->_sendString("1.2.3.4:1111\n");
    }

    if (connection2->isReadable()) {
        message = connection2->receiveMessage();
        EXPECT_TRUE(message.isComplete());
    }
}

TEST(FingerTable, Hashing) {
    auto ft = FingerTable(Poco::Net::SocketAddress("1.1.1.1:1234"));
    ASSERT_EQ(Hash(Poco::Net::SocketAddress("1.1.1.1:1234")), ft.getOwnHash());
}

TEST(FingerTable, InitialFingerTable) {
    auto addr = Poco::Net::SocketAddress("127.0.0.1:1000");
    auto ft = FingerTable(addr);

    // print all fingers
    /*
    for (std::size_t i = 0; i < ft.getSize(); i++) {
        std::cout << i << "\t" << ft.getFinger(i).toString() << "\t" << Hash(ft.getFinger(i)).toString() << std::endl;
    }
    */
}