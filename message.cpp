#include "message.hpp"

Message::Message(sockaddr_in from, sockaddr_in to, unsigned int id, std::string message)
{
    this->from = from;
    this->to = to;
    this->id = id;
    this->message = message;
}

std::string Message::serialize() {
    std::string serializedMessage = "";
    serializedMessage += inet_ntoa(from.sin_addr);
    serializedMessage += ":";
    serializedMessage += std::to_string(ntohs(from.sin_port));
    serializedMessage += ":";
    serializedMessage += inet_ntoa(to.sin_addr);
    serializedMessage += ":";
    serializedMessage += std::to_string(ntohs(to.sin_port));
    serializedMessage += ":";
    serializedMessage += std::to_string(id);
    serializedMessage += ":";
    serializedMessage += message;
    return serializedMessage;
}

Message Message::deserialize(std::string messageString) {
    std::string delimiter = ":";
    size_t pos = 0;
    std::string token;
    std::vector<std::string> tokens;
    while ((pos = messageString.find(delimiter)) != std::string::npos) {
        token = messageString.substr(0, pos);
        tokens.push_back(token);
        messageString.erase(0, pos + delimiter.length());
    }
    tokens.push_back(messageString);
    sockaddr_in from;
    from.sin_family = AF_INET;
    from.sin_port = htons(std::stoi(tokens[1]));
    inet_pton(AF_INET, tokens[0].c_str(), &from.sin_addr);
    sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_port = htons(std::stoi(tokens[3]));
    inet_pton(AF_INET, tokens[2].c_str(), &to.sin_addr);
    unsigned int id = std::stoi(tokens[4]);
    std::string message = tokens[5];
    return Message(from, to, id, message);  
}