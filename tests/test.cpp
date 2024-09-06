#include <cmath>
#include <list>
#include <memory>
#include <unordered_map>
#include <chrono>

#include "../src/fingerTable.hpp"
#include "../src/globalDefinitions.hpp"
#include "../src/hash.hpp"
#include "../src/networking/acceptor.hpp"
#include "../src/networking/connection.hpp"
#include "../src/networking/connectionManager.hpp"
#include "../src/networking/connector.hpp"
#include "../src/networking/message.hpp"
#include "../src/peer.hpp"
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

TEST(Message, FindMessageTest) {
    FindMessage msg = FindMessage(Hash::hashInterval(123));
    ASSERT_EQ(msg.target, Hash::hashInterval(123));

    // compare strings
    auto str = msg.toString();
    auto newMsg = FindMessage::fromString(str);

    ASSERT_EQ(msg.toString(), newMsg.toString());
}

TEST(Message, FindResponseMessageTest) {
    auto msg = FindResponseMessage(Hash::hashInterval(123), Poco::Net::SocketAddress("127.0.0.1:1234"));
    ASSERT_EQ(msg.target, Hash::hashInterval(123));
    ASSERT_EQ(msg.referenceAddress, Poco::Net::SocketAddress("127.0.0.1:1234"));

    // compare strings
    auto str = msg.toString();
    auto newMsg = FindResponseMessage::fromString(str);

    ASSERT_EQ(msg.toString(), newMsg.toString());
}

TEST(Hash, SocketAddress) {
    Poco::Net::SocketAddress addr("127.0.0.1:1234");

    std::unordered_map<Poco::Net::SocketAddress, int, Hash::SocketAddressHasher> map;
    map.insert({addr, 1});

    EXPECT_EQ(map.at(addr), 1);

    auto addr2 = Poco::Net::SocketAddress();
    ASSERT_EQ(addr2.toString(), "0.0.0.0:0");

    ASSERT_NE(Poco::Net::SocketAddress("127.0.0.1:1234"), Poco::Net::SocketAddress("127.0.0.1:1235"));
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

    {
        auto address1 = Poco::Net::SocketAddress("127.0.0.1:1234");
        auto address2 = Poco::Net::SocketAddress("127.0.0.1:1235");

        auto address1Hash = Hash(address1);
        auto address2Hash = Hash(address2);

        std::cout << "Address 1: " << address1.toString() << " Hash: " << address1Hash.toString() << std::endl;
        std::cout << "Address 2: " << address2.toString() << " Hash: " << address2Hash.toString() << std::endl;

        ASSERT_TRUE(address2Hash.isBetween(address1Hash, address2Hash));
        ASSERT_FALSE(address1Hash.isBetween(address1Hash, address2Hash));
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

        ASSERT_TRUE(message->isComplete() && !message->isErrored());
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
        if (msg->isComplete()) {
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

    std::unique_ptr<Message> message;
    if (connection2->isReadable()) {
        message = connection2->receiveMessage();
        std::cout << "Message received" << std::endl;

        EXPECT_TRUE(!message->isComplete());
    } else {
        std::cout << "Connection 2 is not readable" << std::endl;
    }

    if (connection1->isWritable()) {
        connection1->_sendString("1.2.3.4:1111\n");
    }

    if (connection2->isReadable()) {
        message = connection2->receiveMessage();
        EXPECT_TRUE(message->isComplete());
    }
}

TEST(Connection, Blocking) {
    Acceptor acceptor(1234);
    Connector connector;

    // start this in a separate thread
    std::unique_ptr<Connection> connection1;
    std::cout << "Starting thread" << std::endl;
    std::thread thread1([&] {
        connection1 =
            connector.connect(Poco::Net::SocketAddress("127.0.0.1:1234"), false);
    });
    thread1.join();
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

TEST(FingerTable, Update) {
    FingerTable ft(Poco::Net::SocketAddress("127.0.0.1:1234"));
    for (int i = 0; i < FingerTable::FINGER_TABLE_SIZE; ++i) {
        ASSERT_EQ(ft.getFinger(i), Poco::Net::SocketAddress("127.0.0.1:1234"));
    }

    ft.updateWithAddress(Poco::Net::SocketAddress("127.0.0.1:1235"));
    ft.updateWithAddress(Poco::Net::SocketAddress("127.0.0.1:1236"));
    ft.updateWithAddress(Poco::Net::SocketAddress("127.0.0.1:1237"));
    ft.updateWithAddress(Poco::Net::SocketAddress("127.0.0.1:1238"));

    for (int i = 0; i < FingerTable::FINGER_TABLE_SIZE; ++i) {
        std::cout << i << ": " << ft.getFinger(i).toString() << std::endl;
    }
}

TEST(FingerTable, Remove) {
    FingerTable ft1(Poco::Net::SocketAddress("127.0.0.1:1234"));
    auto ft2 = ft1;

    ft1.updateWithAddress(Poco::Net::SocketAddress("127.0.0.1:1235"));
    ft1.updateWithAddress(Poco::Net::SocketAddress("127.0.0.1:1236"));
    ft1.removeAddress(Poco::Net::SocketAddress("127.0.0.1:1236"));
    ft1.removeAddress(Poco::Net::SocketAddress("127.0.0.1:1235"));

    for (unsigned int i = 0; i < FingerTable::FINGER_TABLE_SIZE; i++) {
        ASSERT_EQ(ft1.getFinger(i), ft2.getFinger(i));
    }
}

/*
TEST(Peer, JoinThread) {
    std::thread thread1([] {
        Peer peer1(Poco::Net::SocketAddress("127.0.0.1:1234"));
        auto start = std::chrono::steady_clock::now();
        while (true) {
            peer1.update();

            // quit after 5 seconds
            if (std::chrono::steady_clock::now() - start > std::chrono::seconds(5)) {
                break;
            }
        }
    });

    // sleep
    Poco::Thread::sleep(1000);

    auto peer2 = Peer(Poco::Net::SocketAddress("127.0.0.1:1235"), Poco::Net::SocketAddress("127.0.0.1:1234"));

    for (int i = 0; i < 100; ++i) {
        peer2.update();
    }

    peer2.printConnections();

    ASSERT_EQ(peer2.getConnectionsCount(), 1);

    thread1.join();
}
*/

TEST(Peer, Join) {
    Peer peer1(Poco::Net::SocketAddress("127.0.0.1:1234"), spdlog::level::debug);
    Peer peer2(Poco::Net::SocketAddress("127.0.0.1:1235"), Poco::Net::SocketAddress("127.0.0.1:1234"), spdlog::level::critical);

    for (int i = 0; i < 100; ++i) {
        peer1.update();
        peer2.update();
    }

    peer1.printFingerTable();
    peer2.printFingerTable();
}

TEST(Peer, MassJoin) {
    std::list<Peer> peers;
    peers.emplace_back(Poco::Net::SocketAddress("127.0.0.1:1234"), spdlog::level::debug);

    for (int i = 0; i < 30; ++i) {
        // set own port to 1234
        auto port = 1235 + i;

        auto addr = Poco::Net::SocketAddress("127.0.0.1:1234");
        addr = Poco::Net::SocketAddress(addr.host().toString() + ":" + std::to_string(port));
        peers.emplace_back(addr, Poco::Net::SocketAddress("127.0.0.1:1234"), spdlog::level::off);
    }

    for (int i = 0; i < 100; ++i) {
        for (auto &peer : peers) {
            peer.update();
        }
    }

    for (auto it = peers.begin(); it != peers.end(); ++it) {
        it->printFingerTable();
    }
}

TEST(Peer, PeriodicTasks) {
    auto peer1 = Peer(Poco::Net::SocketAddress("127.0.0.1:1234"), spdlog::level::debug);
    auto peer2 = Peer(Poco::Net::SocketAddress("127.0.0.1:1235"), Poco::Net::SocketAddress("127.0.0.1:1234"), spdlog::level::debug);

    auto start = std::chrono::steady_clock::now();
    while (Timing::since(start) < std::chrono::seconds(10)) {
        peer1.update();
        peer2.update();
    }

    peer1.printFingerTable();
    peer2.printFingerTable();
}
