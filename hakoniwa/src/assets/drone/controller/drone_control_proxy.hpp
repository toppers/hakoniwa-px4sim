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
    void do_event()
    {
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
                state.land();
                in.target_pos_z = -cmd_land.height;
                in.target_pos_x = this->drone->get_drone_dynamics().get_pos().data.x;
                in.target_pos_y = this->drone->get_drone_dynamics().get_pos().data.y;
                in.target_velocity = cmd_land.speed;
            }
            else if (read_cmd(HAKO_AVATOR_CHANNEL_ID_CMD_MOVE, cmd_move) && cmd_move.header.request) {
                state.move();
                in.target_pos_z = this->drone->get_drone_dynamics().get_pos().data.z;
                in.target_pos_x = cmd_move.x;
                in.target_pos_y = cmd_move.y;
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

public:
    mi_drone_control_in_t in = {};
    void setup()
    {
        std::cout << "Setup pdu data" << std::endl;
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


    void run()
    {
        do_event();
        do_control();
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
            mi_drone_control_out_t out;
            DronePositionType pos = container.drone->get_drone_dynamics().get_pos();
            DroneEulerType angle = container.drone->get_drone_dynamics().get_angle();
            hako::assets::drone::DroneVelocityBodyFrameType velocity = container.drone->get_drone_dynamics().get_vel_body_frame();
            hako::assets::drone::DroneAngularVelocityBodyFrameType angular_velocity = container.drone->get_gyro().sensor_value();
            proxy.run();
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

            if (container.control_module.controller != nullptr) {
                out = container.control_module.controller->run(&proxy.in);
            }
            else {
                out = container.controller->run(proxy.in);
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