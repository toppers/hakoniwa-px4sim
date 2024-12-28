#ifndef _HAKONIWA_PDU_ACCESSOR_HPP_
#define _HAKONIWA_PDU_ACCESSOR_HPP_

#include "service/iservice_pdu_types.hpp"
#include "include/hako_asset_pdu.hpp"
#include <map>
#include <string>
#include <tuple>

using namespace hako::service;

namespace hako::drone::impl {

struct CompositeKey {
    std::string strKey;
    int intKey;

    bool operator<(const CompositeKey& other) const {
        return std::tie(strKey, intKey) < std::tie(other.strKey, other.intKey);
    }
};

class HakoniwaPduAccessor {
public:
    virtual ~HakoniwaPduAccessor() {}

    static bool init();

    /*
     * Writers
     */
    static bool write(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data);
    /*
     * Readers
     */
    static bool read(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data);

    static std::vector<hako::asset::PduReader> get_pdu_readers(std::string& robot_name)
    {
        for (auto robot : robots_) {
            if (robot.name == robot_name) {
                return robot.pdu_readers;
            }
        }
        throw std::runtime_error("Failed to get pdu readers: robot_name = " + robot_name);
    }
    static std::vector<hako::asset::PduWriter> get_pdu_writers(std::string& robot_name)
    {
        for (auto robot : robots_) {
            if (robot.name == robot_name) {
                return robot.pdu_writers;
            }
        }
        throw std::runtime_error("Failed to get pdu writers: robot_name = " + robot_name);
    }
private:
    HakoniwaPduAccessor() {}
    static void create_map();
    
    static std::map<CompositeKey, int> pdu_map_;
    static std::vector<hako::asset::Robot> robots_;
};
} // namespace hako::pdu

#endif /* _HAKONIWA_PDU_ACCESSOR_HPP_ */