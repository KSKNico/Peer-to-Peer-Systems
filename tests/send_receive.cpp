#include <gtest/gtest.h>

#include "../peer.hpp"
#include "../peerConfig.hpp"
#include <vector>
#include <thread>

void createPeerAndSendMessage(std::vector<sockaddr_in> peer2_addr_v) {
    // create a peer that listens on port 2001
    auto peer1_addr = PeerConfig::convertToAddress("127.0.0.1:2001");
    std::vector<sockaddr_in> peer1_addr_v = {peer1_addr};

    Peer peer1(2001, peer2_addr_v);
    // acceptConnection();
}

TEST(PeerTest, SendAndReceive) {
    // create two peers that both know each other

    auto peer2_addr = PeerConfig::convertToAddress("127.0.0.1:2002");
    std::vector<sockaddr_in> peer2_addr_v = {peer2_addr};

    // std::thread t1(createPeerAndSendMessage);



   //  Peer peer2(2002, peer1_addr_v);

    // send a message from peer1 to peer2
    // peer1.sendMessage("HELLO", peer2_addr);

    // peer2.pollConnections();

    // TODO: 
    // auto messages = peer2.getMessages();
    // ASSERT_EQ(messages.size(), 1);
}