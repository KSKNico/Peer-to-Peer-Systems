#include <memory>
#include <unordered_map>

#include "../src/hash.hpp"
#include "../src/networking/acceptor.hpp"
#include "../src/networking/connection.hpp"
#include "../src/networking/connectionManager.hpp"
#include "../src/networking/connector.hpp"
#include "../src/networking/message.hpp"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Thread.h"
#include "gtest/gtest.h"

TEST(TestMessage, IDMessageTest) {
    IDMessage message(Poco::Net::SocketAddress("127.0.0.1:1234"));
    std::string str = message.toString();
    IDMessage newMessage = IDMessage::fromString(str);

    EXPECT_EQ(message.toString(), newMessage.toString());
}

TEST(TestHash, SocketAddress) {
    Poco::Net::SocketAddress addr("127.0.0.1:1234");

    std::unordered_map<Poco::Net::SocketAddress, int, Hash::SocketAddressHasher> map;
    map.insert({addr, 1});

    EXPECT_EQ(map.at(addr), 1);
}

TEST(ConnectionTest, ConnectionTest) {
    Acceptor acceptor(1234);
    Connector connector;

    // start this in a separate thread
    std::unique_ptr<Connection> connection1;
    std::thread thread([&] {
        connection1 =
            connector.connect(Poco::Net::SocketAddress("127.0.0.1:1234"));
    });
    auto connection2 = acceptor.accept();

    thread.join();
    ASSERT_TRUE(connection1 != nullptr);
    EXPECT_TRUE(connection1->isConnected());
    EXPECT_TRUE(connection2->isConnected());
}