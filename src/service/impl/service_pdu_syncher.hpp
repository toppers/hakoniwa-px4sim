#ifndef _SERVICE_PDU_SYNCHER_HPP_
#define _SERVICE_PDU_SYNCHER_HPP_

#include "service/iservice_pdu_syncher.hpp"
#include <atomic>
#include <thread>

using namespace hako::service;

namespace hako::service::impl {

typedef struct {
    std::atomic<bool> is_busy;
    std::atomic<bool> is_dirty;
    ServicePduDataType data;
} HakoniwaDronePduDataControlType;

class ServicePduSyncher : public IServicePduSyncher {
public:
    ServicePduSyncher() {}
    virtual ~ServicePduSyncher() {}
    virtual bool flush(uint32_t index, ServicePduDataType& pdu)
    {
        (void)index;
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::out_of_range("flush id out of range");
        }
        while (pdu_data_[pdu.id].is_busy.exchange(true)) {
            std::this_thread::yield();
        }
        pdu_data_[pdu.id].data.pdu = pdu.pdu;
        pdu_data_[pdu.id].is_dirty.store(true);
        pdu_data_[pdu.id].is_busy.store(false);
        return true;
    }
    virtual bool load(uint32_t index, ServicePduDataType& pdu)
    {
        (void)index;
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::out_of_range("load id out of range");
        }
        while (pdu_data_[pdu.id].is_busy.exchange(true)) {
            std::this_thread::yield();
        }
        pdu.pdu = pdu_data_[pdu.id].data.pdu;
        pdu_data_[pdu.id].is_dirty.store(false);
        pdu_data_[pdu.id].is_busy.store(false);
        return true;
    }
private:
    std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM> pdu_data_ = {};

};
} // namespace hako::service::drone::impl

#endif /* _SERVICE_PDU_SYNCHER_HPP_ */