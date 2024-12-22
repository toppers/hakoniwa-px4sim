#ifndef _ILOG_HPP_
#define _ILOG_HPP_

#include <string>
#include <vector>
#include <cstdint>

namespace hako::logger {

typedef enum {
    LOG_TYPE_BOOL,
    LOG_TYPE_INT8,
    LOG_TYPE_UINT8,
    LOG_TYPE_INT16,
    LOG_TYPE_UINT16,
    LOG_TYPE_INT32,
    LOG_TYPE_UINT32,
    LOG_TYPE_INT64,
    LOG_TYPE_UINT64,
    LOG_TYPE_FLOAT,
    LOG_TYPE_DOUBLE,
    LOG_TYPE_STRING
} LogType;

typedef struct {
    std::string name;
    LogType type;
} LogHeaderType;

#include <variant>

typedef std::variant<bool, int8_t, uint8_t, int16_t, uint16_t, 
                     int32_t, uint32_t, int64_t, uint64_t, 
                     float, double, std::string> LogDataType;

class ILog {
public:
    virtual ~ILog() {}
    virtual const std::vector<LogHeaderType>& log_head() = 0;
    virtual const std::vector<LogDataType>& log_data() = 0;
};

} // namespace hako::logger

#endif /* _ILOG_HPP_ */