#ifndef _DRONE_CONTROL_PROXY_HPP_
#define _DRONE_CONTROL_PROXY_HPP_

#include "utils/main_status.hpp"
#include "hako/pdu/hako_pdu_accessor.hpp"

namespace hako::assets::drone {

class DroneControlProxy {
private:
    double home_pos_x = 0;
    double home_pos_y = 0;
    double home_pos_z = 0;
    MainStatus state;
    Hako_HakoDroneCmdTakeoff cmd_takeoff = {};
    Hako_HakoDroneCmdLand cmd_land = {};
    Hako_HakoDroneCmdMove cmd_move = {};

    hako::assets::drone::IAirCraft *drone;
    template<typename PacketType>
    bool read_cmd(int channel_id, PacketType& packet)
    {
        return do_io_read_cmd(drone, channel_id, packet);
    }
    template<typename PacketType>
    bool write_cmd(int channel_id, const PacketType& packet)
    {
        return do_io_write_cmd(drone, channel_id, packet);
    }

    void do_reply()
    {
        switch (state.get_status())
        {
            case MAIN_STATUS_TAKINGOFF:
                cmd_takeoff.header.request = false;
                cmd_takeoff.header.result = true;
                cmd_takeoff.header.result_code = 0;
                write_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_TAKEOFF, cmd_takeoff);

                //for test
                //cmd_move.header.request = true;
                //cmd_move.x = 5.0;
                //cmd_move.y = 5.0;
                //cmd_move.speed = 5.0;
                //write_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, cmd_move);
                break;
            case MAIN_STATUS_LANDING:
                cmd_land.header.request = false;
                cmd_land.header.result = true;
                cmd_land.header.result_code = 0;
                write_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_LAND, cmd_land);
                break;
            case MAIN_STATUS_MOVING:
                cmd_move.header.request = false;
                cmd_move.header.result = true;
                cmd_move.header.result_code = 0;
                write_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, cmd_move);
                break;
            default:
                break;
        }
    }


public:
    mi_drone_control_in_t in = {};
    void setup()
    {
        std::cout << "Setup pdu data" << std::endl;
        //for test
        //cmd_takeoff.header.request = true;
        //cmd_takeoff.height = 5.0;
        //cmd_takeoff.speed = 5.0;
        write_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_TAKEOFF, cmd_takeoff);
        write_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, cmd_move);
        write_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_LAND, cmd_land);
    }

    DroneControlProxy(hako::assets::drone::IAirCraft *obj) 
    {
        this->drone = obj;
        home_pos_x = this->drone->get_drone_dynamics().get_pos().data.x;
        home_pos_y = this->drone->get_drone_dynamics().get_pos().data.y;
        home_pos_z = this->drone->get_drone_dynamics().get_pos().data.z;
    }
    virtual ~DroneControlProxy() {}

    bool need_control()
    {
        if (state.get_status() == MAIN_STATUS_LANDED) {
            return false;
        }
        return true;
    }
    void do_event()
    {
        DroneFlightControllerContextType *drone_context = (DroneFlightControllerContextType*)in.context;
        if (state.get_status() == MAIN_STATUS_LANDED) {
            if (read_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_TAKEOFF, cmd_takeoff) && cmd_takeoff.header.request) {
                state.takeoff();
                in.target_pos_z = -cmd_takeoff.height;
                in.target_pos_x = home_pos_x;
                in.target_pos_y = home_pos_y;
                in.target_velocity = cmd_takeoff.speed;
            }
        }
        else if (state.get_status() == MAIN_STATUS_HOVERING) {
            if (read_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_LAND, cmd_land) && cmd_land.header.request) {
                if (drone_context->drone_control_mode != DRONE_CONTROL_MODE_NONE) {
                    return;
                }
                state.land();
                in.target_pos_z = -cmd_land.height;
                in.target_pos_x = this->drone->get_drone_dynamics().get_pos().data.x;
                in.target_pos_y = this->drone->get_drone_dynamics().get_pos().data.y;
                in.target_velocity = cmd_land.speed;
            }
            else if (read_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, cmd_move) && cmd_move.header.request) {
                if (drone_context->drone_control_mode != DRONE_CONTROL_MODE_NONE) {
                    return;
                }
                std::cout << "START MOVE" << std::endl;
                state.move();
                in.target_pos_z = this->drone->get_drone_dynamics().get_pos().data.z;
                in.target_pos_x = cmd_move.x;
                in.target_pos_y = cmd_move.y;
                std::cout << "move: z = " << in.target_pos_z << std::endl;
                std::cout << "move: x = " << in.target_pos_x << std::endl;
                std::cout << "move: y = " << in.target_pos_y << std::endl;
            }
        }
        else {
            //TODO            
        }
    }
    void do_control()
    {
        DroneFlightControllerContextType *drone_context = (DroneFlightControllerContextType*)in.context;
        switch (state.get_status())
        {
            case MAIN_STATUS_TAKINGOFF:
            case MAIN_STATUS_LANDING:
            case MAIN_STATUS_MOVING:
                if (drone_context->drone_control_mode == DRONE_CONTROL_MODE_NONE) {
                    do_reply();
                    state.done();
                }
                break;
            case MAIN_STATUS_CANCELING:
                //TODO not supported
                break;
            case MAIN_STATUS_LANDED:
            case MAIN_STATUS_HOVERING:
            default:
                //nothing to do
                break;
        }
    }    

};

class DroneControlProxyManager {
private:
    AircraftSystemTaskManager task_manager;
    std::vector<DroneControlProxy> drone_control_proxies;
public:
    void init(Hako_uint64 microseconds, Hako_uint64 dt_usec)
    {
        task_manager.init(microseconds, dt_usec);
        for (auto& container : task_manager.aircraft_system_container) {
            DroneControlProxy proxy(container.drone);
            proxy.in.context = (void*)&container.context;
            proxy.in.mass = container.drone->get_drone_dynamics().get_mass();
            proxy.in.drag = container.drone->get_drone_dynamics().get_drag();
            drone_control_proxies.push_back(proxy);
        }
    }
    void setup()
    {
        for (auto& proxy : drone_control_proxies) {
            proxy.setup();
        }
    }
    void do_task()
    {
        task_manager.do_task();
    }

    void run()
    {
        int index = 0;
        for (auto& container : task_manager.aircraft_system_container) {
            DroneControlProxy& proxy = drone_control_proxies[index];
            hako::assets::drone::DroneDynamicsInputType drone_input = {};
            mi_drone_control_out_t out = {};
            DronePositionType pos = container.drone->get_drone_dynamics().get_pos();
            DroneEulerType angle = container.drone->get_drone_dynamics().get_angle();
            hako::assets::drone::DroneVelocityBodyFrameType velocity = container.drone->get_drone_dynamics().get_vel_body_frame();
            hako::assets::drone::DroneAngularVelocityBodyFrameType angular_velocity = container.drone->get_gyro().sensor_value();
            proxy.do_event();
            proxy.in.pos_x = pos.data.x;
            proxy.in.pos_y = pos.data.y;
            proxy.in.pos_z = pos.data.z;
            proxy.in.euler_x = angle.data.x;
            proxy.in.euler_y = angle.data.y;
            proxy.in.euler_z = angle.data.z;
            proxy.in.u = velocity.data.x;
            proxy.in.v = velocity.data.y;
            proxy.in.w = velocity.data.z;
            proxy.in.p = angular_velocity.data.x;
            proxy.in.q = angular_velocity.data.y;
            proxy.in.r = angular_velocity.data.z;

            if (proxy.need_control()) {
                out = container.control_module.controller->run(&proxy.in);
                proxy.do_control();
            }

            DroneThrustType thrust;
            DroneTorqueType torque;
            thrust.data = out.thrust;
            torque.data.x = out.torque_x;
            torque.data.y = out.torque_y;
            torque.data.z = out.torque_z;

            drone_input.no_use_actuator = true;
            drone_input.manual.control = false;
            drone_input.thrust = thrust;
            drone_input.torque = torque;
            if (container.drone->get_drone_dynamics().has_collision_detection()) {
                do_io_read_collision(container.drone, drone_input.collision);
            }
            if (container.drone->is_enabled_disturbance()) {
                do_io_read_disturb(container.drone, drone_input.disturbance);
            }
            container.drone->run(drone_input);
            calculate_simple_controls(container, thrust);
            do_io_write(container.drone, container.controls);
            index++;
        }
    }
};

}

#endif /* _DRONE_CONTROL_PROXY_HPP_ */