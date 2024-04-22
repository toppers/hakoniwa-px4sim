#ifndef _RADIO_CONTROLLER_HPP_
#define _RADIO_CONTROLLER_HPP_

#include "pid_control.hpp"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define RPM2EULER_RATE(v)   ( ((v) * 2 * M_PI) / 60.0 )
#define TWO_PI (2.0 * M_PI)
#define NORMALIZE_RADIAN(x) (fmod((x), TWO_PI))

typedef struct {
    double target;
    double current;
} PidControlInputType;
typedef struct {
    double p;
    double q;
    double r;
} PidRateControlOutputType;

class RadioController {
private:
    double delta_time;
    double angular_rate_cycle;
    double angular_cycle;
    double angular_rate_simulation_time;
    double angular_simulation_time;
    PidRateControlOutputType prev_out = {};
    PidControl *angular_rate_roll = nullptr;
    PidControl *angular_rate_pitch = nullptr;
    PidControl *angular_rate_yaw = nullptr;

public:
    RadioController(PidControl *rate_roll, PidControl *rate_pitch, PidControl *rate_yaw, double dt)
        : angular_rate_roll(rate_roll), angular_rate_pitch(rate_pitch), angular_rate_yaw(rate_yaw), delta_time(dt) {}
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

    PidRateControlOutputType run_angular_rate_control(PidControlInputType roll, PidControlInputType pitch, PidControlInputType yaw)
    {
        PidRateControlOutputType out = prev_out;
        if (angular_rate_simulation_time >= angular_rate_cycle) {
            out.p = get_target_rate_value(this->angular_rate_roll, 1, M_PI/10.0, roll.target, roll.current);
            out.q = get_target_rate_value(this->angular_rate_pitch, 1, M_PI/10.0, pitch.target, pitch.current);
            out.r = get_target_rate_value(this->angular_rate_yaw, 20, M_PI/4.0, yaw.target, yaw.current);
            angular_rate_simulation_time = 0;
            prev_out = out;
        }
        this->angular_rate_simulation_time += this->delta_time;
        return out;
    }

    void run_angular_control()
    {
        if (angular_simulation_time >= angular_cycle) {
            run_angular_control();
            angular_simulation_time = 0;
        }
        this->angular_simulation_time += this->delta_time;
    }

};

#endif /* _RADIO_CONTROLLER_HPP_ */