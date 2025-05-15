#pragma once

#include <cstdint>
#include <map>
#include <vector>

using resultType = std::uint64_t;
using resultContainer = std::vector<resultType>;
using resultMap = std::map<resultType, resultContainer>;

constexpr resultType INTERVAL_SIZE = 1000;
constexpr unsigned int MAX_MESSAGE_SIZE = 10240;
constexpr char MESSAGE_DELIMITER = ',';
constexpr char MESSAGE_TERMINATOR = '\n';
constexpr char RESULT_DELIMITER = ';';