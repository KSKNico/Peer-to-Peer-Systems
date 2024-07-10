#include "message.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>

Message::Message(MessageData data) : data(data) {}

Message::Message(std::string str) {
    assert (str.size() <= FIXED_MESSAGE_SIZE);
    std::copy(str.begin(), str.end(), data.begin());
}

Message Message::fromBuffer(Poco::BasicFIFOBuffer<char> &buffer) {
    MessageData data;
    buffer.read(data.data(), buffer.used());
    return Message(data);
}

Poco::Buffer<char> Message::toBuffer(const Message &message) {
    Poco::Buffer<char> buffer(Message::FIXED_MESSAGE_SIZE);
    std::copy(message.data.begin(), message.data.end(), buffer.begin());
    return buffer;
}

bool Message::isEmpty() const {
    return data[0] == '\0';
}

Message::get_message Message::decode_get_message() {
    get_message info;
    std::string str(data.data(), data.size());

    size_t pos = str.find(',');

    if(pos == std::string::npos){
        std::cout << "No ',' in GET message" << std::endl;
        return info;
    }
    info.IP_address = str.substr(0, pos);
    info.start_of_interval = std::stoi(str.substr(pos+1));

    return info;
}

Message::put_message Message::decode_put_message() {
    put_message msg;
    std::string str1(data.data(), data.size());
    std::string str = str1;
    if (!str1.starts_with("1")){
         str = str1.substr(4,str1.size()-4);
    }
    // find the first comma to separate IP_address and the rest
    size_t pos = str.find(',');
    if (pos != std::string::npos) {
        msg.IP_address = str.substr(0, pos);

        // get the rest of the string after the IP_address
        std::string rest = str.substr(pos + 1);

        // use a string stream to parse the rest of the string
        std::istringstream ss(rest);
        std::string token;

        // Get the start_of_interval
        if (std::getline(ss, token, ',')) {
            msg.start_of_interval = std::stoull(token);
        }

        // Get the primes
        while (std::getline(ss, token, ',')) {
            msg.primes.push_back(std::stoull(token));
        }
    }

    return msg;
}

Message::join_message Message::decode_join_message() {
    join_message msg;
    std::string str(data.data(), data.size());

    msg.IP_address = str;

    return msg;
}

Message::joinack_message Message::decode_joinack_message() {
    joinack_message msg;
    std::string str(data.data(), data.size());

    size_t pos = str.find(',');

    if(pos == std::string::npos){
        std::cout << "No ',' in JOINACK message" << std::endl;
        return msg;
    }

    msg.IP_address = str.substr(0, pos);
    msg.ClosestKnownIP = str.substr(pos+1);

    return msg;
}

Message::succ_message Message::decode_succ_message() {
    succ_message msg;
    std::string str(data.data(), data.size());

    msg.IP_address = str;

    return msg;
}

Message::succack_message Message::decode_succack_message() {
    succack_message msg;
    std::string str(data.data(), data.size());

    size_t pos = str.find(',');

    if(pos == std::string::npos){
        std::cout << "No ',' in SUCCACK message" << std::endl;
        return msg;
    }

    msg.IP_address = str.substr(0, pos);
    msg.ClosestKnownIP = str.substr(pos+1);

    return msg;
}

Message::pred_message Message::decode_pred_message() {
    pred_message msg;
    std::string str(data.data(), data.size());

    msg.IP_address = str;

    return msg;
}

Message::predack_message Message::decode_predack_message() {
    predack_message msg;
    std::string str(data.data(), data.size());

    size_t pos = str.find(',');

    if(pos == std::string::npos){
        std::cout << "No ',' in PREDACK message" << std::endl;
        return msg;
    }

    msg.IP_address = str.substr(0, pos);
    msg.currentPred = str.substr(pos+1);

    return msg;
}


Message::fing_message Message::decode_fing_message() {
    fing_message msg;
    std::string str(data.data(), data.size());

    msg.IP_address = str;

    return msg;
}

Message::fingack_message Message::decode_fingack_message() {
    fingack_message msg;
    std::string str(data.data(), data.size());

    size_t pos = str.find(',');

    if(pos == std::string::npos){
        std::cout << "No ',' in FINGACK message" << std::endl;
        return msg;
    }

    msg.IP_address = str.substr(0, pos);
    msg.SuccessorIP = str.substr(pos+1);

    return msg;
}

Message::getack_message Message::decode_getack_message() {
    getack_message msg;
    std::string str(data.data(), data.size());

    size_t pos = str.find(',');

    if(pos == std::string::npos){
        std::cout << "No ',' in GETACK message" << std::endl;
        return msg;
    }

    msg.IP_address = str.substr(0, pos);
    std::string rest = str.substr(pos+1);

    pos = str.find(',');
    msg.start_of_interval = std::stoull(str.substr(0, pos));
    msg.RoutingIP = str.substr(pos+1);

    return msg;
}