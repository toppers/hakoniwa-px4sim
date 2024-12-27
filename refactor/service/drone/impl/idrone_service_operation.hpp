#ifndef _IDRONE_SERVICE_OPERATION_HPP_
#define _IDRONE_SERVICE_OPERATION_HPP_

#include "service/drone/impl/drone_service.hpp"
#include "service/drone/idrone_service.hpp"
#include "service/iservice_pdu_syncher.hpp"
#include <array>

namespace hako::service::impl {
typedef struct {
    std::atomic<bool> is_busy;
    std::atomic<bool> is_dirty;
    ServicePduDataType data;
} HakoniwaDronePduDataControlType;

class IDroneServiceOperation {
public:
    virtual ~IDroneServiceOperation() = default;

    // サービスのリセット
    virtual void reset() = 0;

    // コントローラー入力のセットアップ
    virtual void setup_controller_inputs(mi_aircraft_control_in_t& in, std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data) = 0;

    virtual bool can_advanceTimeStep_for_controller() = 0;

    // コントローラーのPDUを書き込む
    virtual void write_controller_pdu(std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data) = 0;

    virtual void setServicePduSyncher(std::shared_ptr<IServicePduSyncher> pdu_syncher) = 0;
};

}

#endif /* _IDRONE_SERVICE_OPERATION_HPP_ */
