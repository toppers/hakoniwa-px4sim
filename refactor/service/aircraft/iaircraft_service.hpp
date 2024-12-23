#ifndef _IAIRCRAFT_SERVICE_HPP_
#define _IAIRCRAFT_SERVICE_HPP_

#include <cstdint>

namespace hako::service {

class IAircraftService {
public:
    virtual ~IAircraftService() = default;
    virtual void startService(bool lockStep, uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep() = 0;
    virtual void stopService() = 0;
    virtual void resetService() = 0;

    virtual bool write() = 0;
    virtual bool read() = 0;
};

} // namespace hako::service::aircraft

#endif /* _IAIRCRAFT_SERVICE_HPP_ */