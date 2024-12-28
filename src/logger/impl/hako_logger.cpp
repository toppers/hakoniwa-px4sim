#include "hako_logger.hpp"

using namespace hako::logger;

std::atomic<bool> hako::logger::HakoLogger::enable_flag{false};
std::atomic<uint64_t> hako::logger::HakoLogger::time_usec{0};