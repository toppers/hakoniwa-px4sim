#ifndef _HAKONIWA_PDU_ACCESSOR_HPP_
#define _HAKONIWA_PDU_ACCESSOR_HPP_

#include "service/iservice_pdu_types.hpp"
#include "include/hako_asset_pdu.hpp"
#include <map>
#include <string>
#include <tuple>

using namespace hako::service;

namespace hako::drone::pdu {

struct CompositeKey {
    std::string strKey;
    int intKey;

    bool operator<(const CompositeKey& other) const {
        return std::tie(strKey, intKey) < std::tie(other.strKey, other.intKey);
    }
};

class HakoniwaPduAccessor {
public:
    HakoniwaPduAccessor() {}
    virtual ~HakoniwaPduAccessor() {}

    bool init();

    /*
     * Writers
     */
    bool write(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data);
    /*
     * Readers
     */
    bool read(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data);

private:
    void create_map();
    
    std::map<CompositeKey, int> pdu_map_;
    std::vector<hako::asset::Robot> robots_;
};
} // namespace hako::pdu

#endif /* _HAKONIWA_PDU_ACCESSOR_HPP_ */