#ifndef _ICSV_LOG_HPP_
#define _ICSV_LOG_HPP_

#include <string>
#include <vector>

class ICsvLog {
public:
    virtual ~ICsvLog() {}
    virtual const std::vector<std::string> log_head() = 0;
    virtual const std::vector<std::string> log_data() = 0;
};

#endif /* _ICSV_LOG_HPP_ */