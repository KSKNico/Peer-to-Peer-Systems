#pragma once

#include <unordered_map>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Poco/Net/NetException.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketConnector.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Timespan.h"
#include "fingerTable.hpp"
#include "networking/connectionManager.hpp"
#include "taskManager.hpp"

class Peer {
   public:
    // Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses);
    Peer() = delete;
    Peer(const Poco::Net::SocketAddress& ownAddress,
         const spdlog::level::level_enum logLevel = spdlog::level::info);
    Peer(const Poco::Net::SocketAddress& ownAddress,
         const Poco::Net::SocketAddress& remoteAddress,
         const spdlog::level::level_enum logLevel = spdlog::level::info);

    void update();

    void printConnections() const;
    void printFingerTable() const;

    void createLogger(const spdlog::level::level_enum logLevel);

    std::size_t getConnectionsCount() const;

    // delete copy constructor
    Peer(const Peer&) = delete;
    Peer& operator=(const Peer&) = delete;
    Peer(Peer&&) = delete;
    Peer& operator=(Peer&&) = delete;

   private:
    /* messages to consider processing
        - FIND
        - GSUC
        - GPRE
        - SSUC
        - SPRE
        all other message need to be processed by the task manager
    */
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message);

    const Poco::Net::SocketAddress ownAddress;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> sink;
    ConnectionManager connectionManager;
    FingerTable fingerTable;
    TaskManager taskManager;
};
