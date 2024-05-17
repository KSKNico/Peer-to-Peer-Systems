#include <gtest/gtest.h>

#include "../peer.hpp"
#include "../peerConfig.hpp"
#include <vector>
#include <thread>
#include <chrono>

void createPeerAndSendMessage(sockaddr_in peer1_addr, sockaddr_in peer2_addr) {
    Peer peer1(peer1_addr.sin_port);
    peer1.connectTo(peer2_addr);
    peer1.sendMessage("Hello", peer2_addr);
}

TEST(PeerTest, SendAndReceive) {
    // create two peers that both know each other

    auto peer2_addr = PeerConfig::convertToAddress("127.0.0.1:2002");
    auto peer1_addr = PeerConfig::convertToAddress("127.0.0.1:2001");


    Peer peer2(2002);
    std::thread t1(createPeerAndSendMessage, peer1_addr, peer2_addr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    peer2.acceptConnections();

    t1.join();

    auto messages = peer2.getMessages();
    ASSERT_EQ(messages.size(), 1);
}

TEST(PeerTest, SerializeAndDeserializeAddress) {
    auto address = PeerConfig::convertToAddress("110.120.130.140:3333");
    auto serialized = Message::serializeAddress(address);
    auto deserialized = Message::deserializeAddress(serialized);

    ASSERT_EQ(address.sin_addr.s_addr, deserialized.sin_addr.s_addr);
    ASSERT_EQ(address.sin_port, deserialized.sin_port);
}