#ifndef _HAKONIWA_PDU_ACCESSOR_HPP_
#define _HAKONIWA_PDU_ACCESSOR_HPP_

#include "service/iservice_pdu_types.hpp"

using namespace hako::service;

namespace hako::pdu {
class HakoniwaPduAssessor {
public:
    HakoniwaPduAssessor() {}
    virtual ~HakoniwaPduAssessor() {}

    /*
     * Writers
     */
    bool write(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data);
    /*
     * Readers
     */
    bool read(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data);

};
} // namespace hako::pdu

#endif /* _HAKONIWA_PDU_ACCESSOR_HPP_ */