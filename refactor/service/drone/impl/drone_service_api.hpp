#ifndef _DRONE_SERVICE_API_HPP_
#define _DRONE_SERVICE_API_HPP_

#include "service/drone/impl/idrone_service_operation.hpp"
#include "service/drone/impl/drone_service.hpp"
#include "service/drone/impl/main_status.hpp"
#include "aircraft/iaircraft.hpp"
#include <cstdint>
#include <thread>

using namespace hako::aircraft;
#define ALMOST_EQUAL(target, current, range) ( ( (current) >= ((target) - (range)) ) &&  ( (current) <= ((target) + (range)) ) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )

namespace hako::service::impl {


class DroneServiceAPI: public IDroneServiceOperation {
private:
    std::shared_ptr<IAirCraft> aircraft_;
    MainStatus state;
    MainStatusType prev_status;

    double home_pos_x = 0;
    double home_pos_y = 0;
    double home_pos_z = 0;

    double target_pos_x = 0;
    double target_pos_y = 0;
    double target_pos_z = 0;
    double target_yaw_deg = 0;

    HakoniwaDronePduDataType drone_pos = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_POSITION };
    HakoniwaDronePduDataType cmd_takeoff = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_TAKEOFF };
    HakoniwaDronePduDataType cmd_land = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_LAND };
    HakoniwaDronePduDataType cmd_move = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MOVE };
    HakoniwaDronePduDataType cmd_magnet = { HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MAGNET };

public:
    DroneServiceAPI(std::shared_ptr<IAirCraft> aircraft): aircraft_(aircraft)
    {
        home_pos_x = aircraft_->get_drone_dynamics().get_pos().data.x;
        home_pos_y = aircraft_->get_drone_dynamics().get_pos().data.y;
        home_pos_z = aircraft_->get_drone_dynamics().get_pos().data.z;
        reset();
    }
    ~DroneServiceAPI() {}
    bool can_advanceTimeStep_for_controller() override 
    { 
        return (state.get_status() != MAIN_STATUS_LANDED);
    }
    void reset() override
    {
        drone_pos.id = HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_POSITION;
        drone_pos.pdu = {};
        cmd_takeoff.id = HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_TAKEOFF;
        cmd_takeoff.pdu = {};
        cmd_land.id = HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_LAND;
        cmd_land.pdu = {};
        cmd_move.id = HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MOVE;
        cmd_move.pdu = {};
        cmd_magnet.id = HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MAGNET;
        cmd_magnet.pdu = {};
        cmd_magnet.pdu.magnet.header.request = true;
        cmd_magnet.pdu.magnet.magnet_on = false;
        state.reset();
        prev_status = MAIN_STATUS_LANDED;
    }
    void setup_controller_inputs(mi_aircraft_control_in_t& in, std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data) override
    {
        (void)read_cmd(drone_pos, pdu_data);
        if (state.get_status() == MAIN_STATUS_LANDED) {
            if (read_cmd(cmd_takeoff, pdu_data) && cmd_takeoff.pdu.takeoff.header.request) {
                state.takeoff();
                prev_status = state.get_status();
                in.target_pos_z = -cmd_takeoff.pdu.takeoff.height;
                in.target_pos_x = home_pos_x;
                in.target_pos_y = home_pos_y;
                in.target_velocity = cmd_takeoff.pdu.takeoff.speed;
                in.target_yaw_deg = -cmd_takeoff.pdu.takeoff.yaw_deg;
                std::cout << "takeoff: z = " << in.target_pos_z << std::endl;
                std::cout << "takeoff: x = " << in.target_pos_x << std::endl;
                std::cout << "takeoff: y = " << in.target_pos_y << std::endl;
            }
        }
        else if (state.get_status() == MAIN_STATUS_HOVERING) {
            if (read_cmd(cmd_land, pdu_data) && cmd_land.pdu.land.header.request) {
                state.land();
                prev_status = state.get_status();
                in.target_pos_z = -cmd_land.pdu.land.height;
                in.target_pos_x = aircraft_->get_drone_dynamics().get_pos().data.x;
                in.target_pos_y = aircraft_->get_drone_dynamics().get_pos().data.y;
                in.target_velocity = cmd_land.pdu.land.speed;
                in.target_yaw_deg = -cmd_land.pdu.land.yaw_deg;
                std::cout << "land: z = " << in.target_pos_z << std::endl;
                std::cout << "land: x = " << in.target_pos_x << std::endl;
                std::cout << "land: y = " << in.target_pos_y << std::endl;
            }
            else if (read_cmd(cmd_move, pdu_data) && cmd_move.pdu.move.header.request) {
                std::cout << "START MOVE" << std::endl;
                state.move();
                prev_status = state.get_status();
                in.target_pos_x = cmd_move.pdu.move.x;
                in.target_pos_y = -cmd_move.pdu.move.y;
                in.target_pos_z = -cmd_move.pdu.move.z;
                in.target_velocity = cmd_move.pdu.move.speed;
                in.target_yaw_deg = -cmd_move.pdu.move.yaw_deg;
                std::cout << "move: z = " << in.target_pos_z << std::endl;
                std::cout << "move: x = " << in.target_pos_x << std::endl;
                std::cout << "move: y = " << in.target_pos_y << std::endl;
            }
            else {
                // nothing to do
            }
        }
        else if (state.get_status() == MAIN_STATUS_MOVING) {
            if (read_cmd(cmd_move, pdu_data) && !cmd_move.pdu.move.header.request) {
                std::cout << "Move event is canceled" << std::endl;
                in.target_pos_x = drone_pos.pdu.position.linear.x;
                in.target_pos_y = -drone_pos.pdu.position.linear.y;
                in.target_pos_z = -drone_pos.pdu.position.linear.z;
                std::cout << "cancel: z = " << in.target_pos_z << std::endl;
                std::cout << "cancel: x = " << in.target_pos_x << std::endl;
                std::cout << "cancel: y = " << in.target_pos_y << std::endl;
                in.target_velocity = 0;
                in.target_yaw_deg = drone_pos.pdu.position.angular.z;
                state.cancel();
            }
        }
        else {
            // nothing to do
        }
        target_pos_x = in.target_pos_x;
        target_pos_y = in.target_pos_y;
        target_pos_z = in.target_pos_z;
        target_yaw_deg = in.target_yaw_deg;
    }

    void write_controller_pdu(std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data) override 
    {
        auto status = state.get_status();
        switch (status)
        {
            case MAIN_STATUS_TAKINGOFF:
            case MAIN_STATUS_LANDING:
            case MAIN_STATUS_MOVING:
                if (is_operation_done()) {
                    write_back(status, 0, pdu_data);
                    state.done();
                }
                break;
            case MAIN_STATUS_CANCELING:
                //cancel event is done.
                write_back(prev_status, -1, pdu_data);
                state.done();
                break;
            case MAIN_STATUS_LANDED:
            case MAIN_STATUS_HOVERING:
            default:
                //nothing to do
                break;
        }
    }

private:
    void write_back(MainStatusType status, int errcode, std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data) 
    {
        if (status == MAIN_STATUS_TAKINGOFF) {
            cmd_takeoff.pdu.takeoff.header.request = false;
            cmd_takeoff.pdu.takeoff.header.result = true;
            cmd_takeoff.pdu.takeoff.header.result_code = errcode;
            write_cmd(pdu_data, cmd_takeoff);
        }
        else if (status == MAIN_STATUS_LANDING) {
            cmd_land.pdu.land.header.request = false;
            cmd_land.pdu.land.header.result = true;
            cmd_land.pdu.land.header.result_code = errcode;
            write_cmd(pdu_data, cmd_land);
        }
        else if (status == MAIN_STATUS_MOVING) {
            cmd_move.pdu.move.header.request = false;
            cmd_move.pdu.move.header.result = true;
            cmd_move.pdu.move.header.result_code = errcode;
            write_cmd(pdu_data, cmd_move);
        }
        else {
            // nothing to do
        }        
    }
    int count = 0;
    int max_count = 100;
    bool almost_equal(double target_pos_x, double target_pos_y, double target_pos_z, double target_yaw_deg,
                        double pos_x, double pos_y, double pos_z, double yaw_deg)
    {
        if (ALMOST_EQUAL(target_pos_x, pos_x, 0.1) == false) {
            count = 0;
            return false;
        }
        if (ALMOST_EQUAL(target_pos_y, pos_y, 0.1) == false) {
            count = 0;
            return false;
        }
        if (ALMOST_EQUAL(target_pos_z, pos_z, 0.1) == false) {
            count = 0;
            return false;
        }
        if (ALMOST_EQUAL(target_yaw_deg, yaw_deg, 0.5) == false) {
            count = 0;
            return false;
        }
        count++;
        if (count < max_count) {
            return false;
        }
        count = 0;
        return true;

    }

    bool is_operation_done()
    {
        return almost_equal(
            target_pos_x, 
            target_pos_y, 
            target_pos_z, 
            -target_yaw_deg, 
            drone_pos.pdu.position.linear.x, 
            -drone_pos.pdu.position.linear.y, 
            -drone_pos.pdu.position.linear.z, 
            RADIAN2DEGREE(drone_pos.pdu.position.angular.z));
    }
    bool read_cmd(HakoniwaDronePduDataType& dest, std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data)
    {
        auto& pdu_entry = pdu_data[dest.id];

        // 排他制御を行いながらデータにアクセス
        while (pdu_entry.is_busy.exchange(true)) {
            std::this_thread::yield(); // CPU負荷を軽減
        }
        // 更新がなければ処理をスキップ
        if (!pdu_entry.is_dirty) {
            pdu_entry.is_busy.store(false);
            return false;
        }
        // データを取得
        const auto& source_pdu = pdu_entry.data;

        // deep copy
        bool ret = drone_pdu_data_deep_copy(source_pdu, dest);
        if (!ret) {
            throw std::runtime_error("read_cmd: deep copy failed");
        }

        // フラグをリセット
        pdu_entry.is_dirty.store(false);
        pdu_entry.is_busy.store(false);

        return true;
    }
    void write_cmd(std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data, const HakoniwaDronePduDataType& src)
    {
        auto& pdu_entry = pdu_data[src.id];

        // 排他制御を行いながらデータにアクセス
        while (pdu_entry.is_busy.exchange(true)) {
            std::this_thread::yield(); // CPU負荷を軽減
        }

        // deep copy
        bool ret = drone_pdu_data_deep_copy(src, pdu_entry.data);
        if (!ret) {
            throw std::runtime_error("write_cmd: deep copy failed");
        }

        // フラグをセット
        pdu_entry.is_dirty.store(true);
        pdu_entry.is_busy.store(false);
    }
};

}
#endif /* _DRONE_SERVICE_API_HPP_ */