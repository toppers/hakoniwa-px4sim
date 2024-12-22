#include "logger.hpp"

using namespace hako::logger;

std::atomic<bool> Logger::enable_flag{false};
std::atomic<uint64_t> Logger::time_usec{0};