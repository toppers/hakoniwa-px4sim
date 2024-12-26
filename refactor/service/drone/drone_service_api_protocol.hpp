#ifndef _DRONE_SERVICE_API_PROTOCOL_HPP_
#define _DRONE_SERVICE_API_PROTOCOL_HPP_

#include "service/drone/idrone_service_container.hpp"

namespace hako::service {

inline double radian_to_degree(double radian) {
    return radian * (180.0 / M_PI);
}
typedef struct {
    double x;
    double y;
    double z;
} Vector3Type;
class DroneServiceApiProtocol {
private:
    IDroneServiceContainer& drone_service_container_;
    void set_header(HakoniwaDronePduDataType& pdu, bool request, bool result, int result_code)
    {
        pdu.pdu.takeoff.header.request = request;
        pdu.pdu.takeoff.header.result = result;
        pdu.pdu.takeoff.header.result_code = result_code;
    }
    void wait_response(int index, HakoniwaDronePduDataType& pdu)
    {
        while (true) {
            drone_service_container_.peek_pdu(index, pdu);
            if (!pdu.pdu.takeoff.header.request) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            auto pos = get_position(index);
            std::cout << "INFO: position x=" << std::fixed << std::setprecision(1) << pos.x << " y=" << pos.y << " z=" << pos.z << std::endl;


        }
    }
public:
    DroneServiceApiProtocol(IDroneServiceContainer& drone_service_container) : drone_service_container_(drone_service_container) {}
    virtual ~DroneServiceApiProtocol() = default;
    bool takeoff(int index, double height) 
    {
        HakoniwaDronePduDataType pdu = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_TAKEOFF };
        //prepare
        set_header(pdu, true, false, 0);
        pdu.pdu.takeoff.height = height;
        pdu.pdu.takeoff.speed = 5.0;
        pdu.pdu.takeoff.yaw_deg = get_yaw_deg(index);
        drone_service_container_.write_pdu(index, pdu);

        std::cout << "INFO: takeoff write done" << std::endl;
        //do wait
        wait_response(index, pdu);
        //done
        pdu.pdu.takeoff.header.request = false;
        drone_service_container_.write_pdu(index, pdu);
        return true;
    }
    bool land(int index)
    {
        HakoniwaDronePduDataType pdu = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_LAND };
        //prepare
        set_header(pdu, true, false, 0);
        pdu.pdu.land.height = 0.0;
        pdu.pdu.land.speed = 5.0;
        pdu.pdu.land.yaw_deg = get_yaw_deg(index);
        drone_service_container_.write_pdu(index, pdu);
        //do wait
        wait_response(index, pdu);
        //done
        pdu.pdu.land.header.request = false;
        drone_service_container_.write_pdu(index, pdu);
        return true;
    }
    bool move(int index, float x, float y, float z)
    {
        HakoniwaDronePduDataType pdu = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MOVE };
        //prepare
        set_header(pdu, true, false, 0);
        pdu.pdu.move.x = x;
        pdu.pdu.move.y = y;
        pdu.pdu.move.z = z;
        pdu.pdu.move.yaw_deg = get_yaw_deg(index);
        pdu.pdu.move.speed = 5.0;
        drone_service_container_.write_pdu(index, pdu);
        //do wait
        wait_response(index, pdu);
        //done
        pdu.pdu.move.header.request = false;
        drone_service_container_.write_pdu(index, pdu);
        
        return true;
    }
    double get_yaw_deg(int index)
    {
        HakoniwaDronePduDataType pdu = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_POSITION };
        drone_service_container_.peek_pdu(index, pdu);
        return radian_to_degree(pdu.pdu.position.angular.z);
    }

    Vector3Type get_position(int index)
    {
        HakoniwaDronePduDataType pdu = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_POSITION };
        drone_service_container_.peek_pdu(index, pdu);
        return { pdu.pdu.position.linear.x, pdu.pdu.position.linear.y, pdu.pdu.position.linear.z };
    }
    Vector3Type get_attitude(int index)
    {
        HakoniwaDronePduDataType pdu = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_POSITION };
        drone_service_container_.peek_pdu(index, pdu);
        return { pdu.pdu.position.angular.x, pdu.pdu.position.angular.y, pdu.pdu.position.angular.z };
    }   
};
} // namespace hako::service

#endif /* _DRONE_SERVICE_API_PROTOCOL_HPP_ */
