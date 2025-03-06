#pragma once

#include <cstdint>

using resultType = std::uint64_t;
using resultContainer = std::vector<resultType>;

constexpr unsigned int MAX_MESSAGE_SIZE = 10240;
constexpr char MESSAGE_DELIMITER = ',';
constexpr char MESSAGE_TERMINATOR = '\n';
constexpr char RESULT_DELIMITER = ';';