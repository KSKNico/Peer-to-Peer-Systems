#include "message.hpp"

Message::Message(sockaddr_in from, sockaddr_in to, unsigned int id, std::string payload)
{
    this->from = from;
    this->to = to;
    this->id = id;
    this->payload = payload;
    this->length = sizeof(from) + sizeof(to) + sizeof(id) + payload.length();
}

std::vector<std::byte> Message::serialize() {
    std::vector<std::byte> serialized;

    // reserve space for the message
    serialized.reserve(this->length);

    // a message always starts with the length of the message
    serialized.push_back(std::byte(this->length & 0xFF));
    serialized.push_back(std::byte((this->length >> 8) & 0xFF));
    serialized.push_back(std::byte((this->length >> 16) & 0xFF));
    serialized.push_back(std::byte((this->length >> 24) & 0xFF));


    // Serialize the from address
    auto fromData = serializeAddress(this->from);
    serialized.insert(serialized.end(), fromData.begin(), fromData.end());

    // Serialize the to address
    auto toData = serializeAddress(this->to);
    serialized.insert(serialized.end(), toData.begin(), toData.end());

    // Serialize the id
    serialized.push_back(std::byte(this->id & 0xFF));
    serialized.push_back(std::byte((this->id >> 8) & 0xFF));
    serialized.push_back(std::byte((this->id >> 16) & 0xFF));
    serialized.push_back(std::byte((this->id >> 24) & 0xFF));

    // Serialize the payload
    for (char c : this->payload)
    {
        serialized.push_back(std::byte(c));
    }

    return serialized;
}

Message Message::deserialize(std::vector<std::byte> messageData) {
    sockaddr_in from = deserializeAddress(std::vector<std::byte>(messageData.begin() + 4, messageData.begin() + 10));
    sockaddr_in to = deserializeAddress(std::vector<std::byte>(messageData.begin() + 10, messageData.begin() + 16));
    unsigned int id = 0;
    id |= std::to_integer<int>(messageData[16]);
    id |= std::to_integer<int>(messageData[17]) << 8;
    id |= std::to_integer<int>(messageData[18]) << 16;
    id |= std::to_integer<int>(messageData[19]) << 24;
    std::string payload;
    for (size_t i = 20; i < messageData.size(); i++)
    {
        payload.push_back(static_cast<char>(std::to_integer<int>(messageData[i])));
    }
    return Message(from, to, id, payload);
}

std::vector<std::byte> Message::serializeAddress(sockaddr_in address) {
    std::vector<std::byte> serialized;
    serialized.reserve(4 + 2);
    // Serialize the address
    serialized.push_back(std::byte(address.sin_addr.s_addr & 0xFF));
    serialized.push_back(std::byte((address.sin_addr.s_addr >> 8) & 0xFF));
    serialized.push_back(std::byte((address.sin_addr.s_addr >> 16) & 0xFF));
    serialized.push_back(std::byte((address.sin_addr.s_addr >> 24) & 0xFF));
    // Serialize the port
    serialized.push_back(std::byte(address.sin_port & 0xFF));
    serialized.push_back(std::byte((address.sin_port >> 8) & 0xFF));
    return serialized;
}

sockaddr_in Message::deserializeAddress(std::vector<std::byte> addressData) {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = 0;
    address.sin_port = 0;
    // Deserialize the address
    address.sin_addr.s_addr |= static_cast<uint32_t>(std::to_integer<int>(addressData[0]));
    address.sin_addr.s_addr |= static_cast<uint32_t>(std::to_integer<int>(addressData[1])) << 8;
    address.sin_addr.s_addr |= static_cast<uint32_t>(std::to_integer<int>(addressData[2])) << 16;
    address.sin_addr.s_addr |= static_cast<uint32_t>(std::to_integer<int>(addressData[3])) << 24;
    // Deserialize the port
    address.sin_port |= static_cast<uint16_t>(std::to_integer<int>(addressData[4]));
    address.sin_port |= static_cast<uint16_t>(std::to_integer<int>(addressData[5])) << 8;
    return address;
}

sockaddr_in Message::getReceiver() const {
    return this->to;
}