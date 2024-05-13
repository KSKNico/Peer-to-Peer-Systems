#include <gtest/gtest.h>

#include "../peer.hpp"
#include "../peerConfig.hpp"
#include <vector>
#include <thread>
#include <chrono>

void createPeerAndSendMessage(std::vector<sockaddr_in> peer1_addr_v, std::vector<sockaddr_in> peer2_addr_v) {
    Peer peer1(peer1_addr_v[0].sin_port, peer2_addr_v);
    peer1.connectTo(peer2_addr_v[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    peer1.sendMessage("Hello", peer2_addr_v[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST(PeerTest, SendAndReceive) {
    // create two peers that both know each other

    auto peer2_addr = PeerConfig::convertToAddress("127.0.0.1:2002");
    auto peer1_addr = PeerConfig::convertToAddress("127.0.0.1:2001");
    std::vector<sockaddr_in> peer1_addr_v = {peer1_addr};
    std::vector<sockaddr_in> peer2_addr_v = {peer2_addr};


    Peer peer2(2002, {});
    std::thread t1(createPeerAndSendMessage, peer1_addr_v, peer2_addr_v);

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