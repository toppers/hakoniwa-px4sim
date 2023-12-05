#ifndef _THRUST_DYNAMICS_HPP_
#define _THRUST_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "ithrust_dynamics.hpp"
#include <glm/glm.hpp>

namespace hako::assets::drone {

class ThrustDynamics : public hako::assets::drone::IThrustDynamics {
private:
    double delta_time_sec;
    double param_A;
    double param_B;
    double param_Jr;
    DroneThrustType thrust;
    DroneTorqueType torque;
    double counter_torque[ROTOR_NUM];
    double T[ROTOR_NUM];
    DroneRotorSpeedType prev_rotor_speed[ROTOR_NUM];
    RotorConfigType rotor_config[ROTOR_NUM];
    void run_thrust(const DroneRotorSpeedType rotor_speed[ROTOR_NUM])
    {
        double u = 0;
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->T[i] = this->param_A * (rotor_speed[i].data * rotor_speed[i].data);
            u += this->T[i];
        }
        this->thrust.data = u;
    }
    void run_counter_torque(const DroneRotorSpeedType rotor_speed[ROTOR_NUM])
    {
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->counter_torque[i] = (this->param_B * rotor_speed[i].data * rotor_speed[i].data) 
                                    + (this->param_Jr * (rotor_speed[i].data - this->prev_rotor_speed[i].data) / this->delta_time_sec );
        }
    }
    void run_torque(const DroneRotorSpeedType rotor_speed[ROTOR_NUM])
    {
        glm::dvec3 total_torque = glm::dvec3(0.0, 0.0, 0.0);
        for (int i = 0; i < ROTOR_NUM; i++) {
            // ローターによる推力ベクトルを生成（Z方向にのみ作用する）
            // 航空座標系では重力の反対方向はマイナス
            glm::dvec3 thrust_vector = glm::dvec3(0.0, 0.0, -this->T[i]);
            
            // ローターの位置ベクトルと推力ベクトルとの外積
            glm::dvec3 torque_vector = glm::cross(this->rotor_config[i].data, thrust_vector);
            
            // 反トルク
            // CCW:　時計回り（Z軸方向の回転に対してプラス方向）
            //  CW:反時計回り（Z軸方向の回転に対してマイナス方向）
            torque_vector += glm::dvec3(0.0, 0.0, this->rotor_config[i].ccw * this->counter_torque[i]);
            // 合計トルクを計算
            total_torque += torque_vector;
        }
        this->torque.data = total_torque;
    }
public:
    ThrustDynamics(double dt)
    {
        this->delta_time_sec = dt;
        this->param_A = 1.0;
        this->param_B = 0.1;
        this->param_Jr = 0.1;

        this->rotor_config[0].ccw = -1;
        this->rotor_config[0].data = { 0.3, 0.0, 0 };
        this->rotor_config[1].ccw = 1;
        this->rotor_config[1].data = { 0.0, -0.3, 0 };
        this->rotor_config[2].ccw = -1;
        this->rotor_config[2].data = { -0.3, 0.0, 0 };
        this->rotor_config[3].ccw = 1;
        this->rotor_config[3].data = { 0.0, 0.3, 0 };
    }
    virtual ~ThrustDynamics() {}

    void set_params(double a, double b, double jr)
    {
        this->param_A = a;
        this->param_B = b;
        this->param_Jr = jr;
    }

    void set_rotor_config(const RotorConfigType rotor_config[ROTOR_NUM]) override
    {
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->rotor_config[i] = rotor_config[i];
        }
    }
    void set_thrust(const DroneThrustType &thrust) override 
    {
        this->thrust = thrust;
    }
    void set_torque(const DroneTorqueType &torque) override
    {
        this->torque = torque;
    }

    DroneThrustType get_thrust() const override
    {
        return this->thrust;
    }
    DroneTorqueType get_torque() const override
    {
        return this->torque;
    }

    void run(const DroneRotorSpeedType rotor_speed[ROTOR_NUM]) override
    {
        this->run_thrust(rotor_speed);
        this->run_counter_torque(rotor_speed);
        this->run_torque(rotor_speed);
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->prev_rotor_speed[i] = rotor_speed[i];
        }
    }
};

}
//https://github.com/PX4/jMAVSim/blob/66b764ada522893c05224950aa6268c809f8e48a/src/me/drton/jmavsim/vehicle/AbstractMulticopter.java

#endif /* _THRUST_DYNAMICS_HPP_ */
