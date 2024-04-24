#ifndef _RADIO_CONTROLLER_HPP_
#define _RADIO_CONTROLLER_HPP_

#include "pid_control.hpp"

#define ALMOST_EQUAL(target, current, range) ( ( (current) >= ((target) - (range)) ) &&  ( (current) <= ((target) + (range)) ) )

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define RPM2EULER_RATE(v)   ( ((v) * 2 * M_PI) / 60.0 )
#define TWO_PI (2.0 * M_PI)
#define NORMALIZE_RADIAN(x) (fmod((x), TWO_PI))

struct PidControlInputType {
    double target;
    double current;
    PidControlInputType() : target(0), current(0) {}
    PidControlInputType(double current_val, double target_val)
        : current(current_val), target(target_val) {}
};
struct PidRateControlOutputType {
    double p;
    double q;
    double r;
    PidRateControlOutputType() : p(0), q(0), r(0) {}
    PidRateControlOutputType(double p_val, double q_val, double r_val)
        : p(p_val), q(q_val), r(r_val) {}
};
struct PidControlOutputType {
    double roll_rate;
    double pitch_rate;
    PidControlOutputType() : roll_rate(0), pitch_rate(0) {}
    PidControlOutputType(double roll_rate_val, double pitch_rate_val)
        : roll_rate(roll_rate_val), pitch_rate(pitch_rate_val) {}
};

typedef struct {
    double Kp;
    double Ki;
    double Kd;
} RadioControllerPidParamType;
typedef struct {
    double delta_time;
    double angular_cycle;
    double angular_rate_cycle;
    double throttle_gain;
    RadioControllerPidParamType roll;
    RadioControllerPidParamType pitch;
    RadioControllerPidParamType roll_rate;
    RadioControllerPidParamType pitch_rate;
    RadioControllerPidParamType yaw_rate;
} RadioControllerParamType;

class RadioController {
private:
    double delta_time;
    double throttle_gain;
    //angle control
    double angular_cycle;
    double angular_simulation_time;
    PidControl *angular_roll = nullptr;
    PidControl *angular_pitch = nullptr;

    //rate control
    double angular_rate_cycle;
    double angular_rate_simulation_time;
    PidRateControlOutputType prev_rate_out;
    PidControlOutputType prev_angle_out;
    PidControl *angular_rate_roll = nullptr;
    PidControl *angular_rate_pitch = nullptr;
    PidControl *angular_rate_yaw = nullptr;


    double get_limit_value(double input_value, double base_value, double min_value, double max_value)
    {
        double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
        return limited_value;
    }
    double get_target_rate_value(PidControl *ctrl, double rpm_max, double max_angle_rate, double target, double current)
    {
        double target_rate_max = RPM2EULER_RATE(rpm_max);
        double target_rate = get_limit_value(target, 0, -target_rate_max, target_rate_max);
        double out_value = ctrl->calculate(target, NORMALIZE_RADIAN(current));
        return get_limit_value(out_value, 0, -max_angle_rate, max_angle_rate);
    }
    double get_target_value(PidControl *ctrl, double max_angle, double target, double current)
    {
        double target_angle = target * DEGREE2RADIAN(max_angle);
        double out_value = ctrl->calculate(target_angle, NORMALIZE_RADIAN(current));
        return out_value;
    }


public:
    RadioController(PidControl *roll, PidControl *pitch, PidControl *rate_roll, PidControl *rate_pitch, PidControl *rate_yaw, double dt, double throttle_gain)
        : angular_roll(roll), angular_pitch(pitch), 
            angular_rate_roll(rate_roll), angular_rate_pitch(rate_pitch), angular_rate_yaw(rate_yaw), delta_time(dt), throttle_gain(throttle_gain) {}
    virtual ~RadioController() {}

    void set_angular_rate_cycle(double cycle)
    {
        angular_rate_cycle = cycle;
        angular_rate_simulation_time = 0;
    }
    void set_angular_cycle(double cycle)
    {
        angular_cycle = cycle;
        angular_simulation_time = 0;
    }


    PidRateControlOutputType run_angular_rate_control(PidControlInputType roll, PidControlInputType pitch, PidControlInputType yaw)
    {
        PidRateControlOutputType out = prev_rate_out;
        if (angular_rate_simulation_time >= angular_rate_cycle) {
            out.p = get_target_rate_value(this->angular_rate_roll, 1, M_PI/10.0, roll.target, roll.current);
            out.q = get_target_rate_value(this->angular_rate_pitch, 1, M_PI/10.0, pitch.target, pitch.current);
            out.r = get_target_rate_value(this->angular_rate_yaw, 20, M_PI/4.0, yaw.target, yaw.current);
            angular_rate_simulation_time = 0;
            prev_rate_out = out;
        }
        this->angular_rate_simulation_time += this->delta_time;
        return out;
    }

    PidControlOutputType run_angular_control(PidControlInputType roll, PidControlInputType pitch)
    {
        PidControlOutputType out = prev_angle_out;
        if (angular_simulation_time >= angular_cycle) {
            out.roll_rate = get_target_value(this->angular_roll, 20, roll.target, roll.current);
            out.pitch_rate = get_target_value(this->angular_pitch, 20, pitch.target, pitch.current);
            angular_simulation_time = 0;

            //std::cout << "target(roll_rate): " << out.roll_rate << " target: " << RADIAN2DEGREE(roll.target) << " current:  " << RADIAN2DEGREE(roll.current) << std::endl;
        }
        this->angular_simulation_time += this->delta_time;
        return out;
    }
    double run_thrust_control(double mass, double gravity, double power)
    {
        return (mass * gravity) - (this->throttle_gain * power);
    }

};

static inline RadioController* create_radio_controller(const RadioControllerParamType& param)
{

    PidControl *roll = new PidControl(param.roll.Kp, param.roll.Ki, param.roll.Kd, 0, param.delta_time);
    if (roll == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        return nullptr;
    }
    PidControl *pitch = new PidControl(param.pitch.Kp, param.pitch.Ki, param.pitch.Kd, 0, param.delta_time);
    if (pitch == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        return nullptr;
    }
    PidControl *roll_rate = new PidControl(param.roll_rate.Kp, param.roll_rate.Ki, param.roll_rate.Kd, 0, param.delta_time);
    if (roll_rate == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        return nullptr;
    }
    PidControl *pitch_rate = new PidControl(param.pitch_rate.Kp, param.pitch_rate.Ki, param.pitch_rate.Kd, 0, param.delta_time);
    if (pitch_rate == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        return nullptr;
    }
    PidControl *yaw_rate = new PidControl(param.yaw_rate.Kp, param.yaw_rate.Ki, param.yaw_rate.Kd, 0, param.delta_time);
    if (yaw_rate == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        return nullptr;
    }
    RadioController *rc = new RadioController(roll, pitch, roll_rate, pitch_rate, yaw_rate, param.delta_time, param.throttle_gain);
    if (rc == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        return nullptr;
    }
    rc->set_angular_cycle(param.angular_cycle);
    rc->set_angular_rate_cycle(param.angular_rate_cycle);
    return rc;
}

#endif /* _RADIO_CONTROLLER_HPP_ */