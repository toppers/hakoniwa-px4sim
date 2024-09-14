#ifndef _RADIO_CONTROLLER_HPP_
#define _RADIO_CONTROLLER_HPP_

#include "flight_controller_types.hpp"
#include "speed_controller.hpp"
#include "altitude_controller.hpp"
#include "hako_controller_param_loader.hpp"

struct PidRateControlOutputType {
    double p;
    double q;
    double r;
    PidRateControlOutputType() : p(0), q(0), r(0) {}
    PidRateControlOutputType(double p_val, double q_val, double r_val)
        : p(p_val), q(q_val), r(r_val) {}
};

struct RadioControlInputType {
    FlightControllerInputEulerType euler;
    FlightControllerInputAngularRateType angular_rate;
    double target_thrust;
    double target_roll;
    double target_pitch;
    double target_angular_rate_r;
    RadioControlInputType() : euler(), angular_rate(), target_roll(0), target_pitch(0), target_thrust(0), target_angular_rate_r(0) {}
    RadioControlInputType(
        FlightControllerInputEulerType e, FlightControllerInputAngularRateType rate, 
        double t_t, double t_r, double t_p, double t_arr) : 
        euler(e), angular_rate(rate), 
        target_thrust(t_t), target_roll(t_r), target_pitch(t_p), target_angular_rate_r(t_arr) {}    
};

struct RadioControlPidControlOutputType {
    double roll_rate;
    double pitch_rate;
    RadioControlPidControlOutputType() : roll_rate(0), pitch_rate(0) {}
    RadioControlPidControlOutputType(double roll_rate_val, double pitch_rate_val)
        : roll_rate(roll_rate_val), pitch_rate(pitch_rate_val) {}
};

typedef struct {
    double Kp;
    double Ki;
    double Kd;
} RadioControllerPidParamType;

typedef struct {
    double mass;
    double gravity;
    double delta_time;
    double angular_cycle;
    double angular_rate_cycle;
    double alt_control_cycle;
    double throttle_gain;
    RadioControllerPidParamType roll;
    RadioControllerPidParamType pitch;
    RadioControllerPidParamType roll_rate;
    RadioControllerPidParamType pitch_rate;
    RadioControllerPidParamType yaw_rate;
    double pid_param_max_roll;
    double pid_param_max_pitch;
    double pid_param_h_rpm_max;
    double pid_param_h_angle_rate_max;
    double pid_param_yaw_rpm_max;
    double pid_param_yaw_angle_rate_max;
    bool use_spd_ctrl;
} RadioControllerParamType;

class RadioController {
private:
    double throttle_gain;
    double mass;
    double gravity;
    // angle control
    double angular_cycle;
    double angular_simulation_time;
    PidControl *angular_roll = nullptr;
    PidControl *angular_pitch = nullptr;

    // rate control
    double angular_rate_cycle;
    double angular_rate_simulation_time;
    PidRateControlOutputType prev_rate_out;
    RadioControlPidControlOutputType prev_angle_out;
    PidControl *angular_rate_roll = nullptr;
    PidControl *angular_rate_pitch = nullptr;
    PidControl *angular_rate_yaw = nullptr;

    double alt_control_cycle;
    double pid_param_h_rpm_max;
    double pid_param_h_angle_rate_max;
    double pid_param_yaw_rpm_max;
    double pid_param_yaw_angle_rate_max;
    double alt_delta_value_m = ALT_DELTA_VALUE_M;

    void initializeFromLoader(HakoControllerParamLoader* loader) {
        if (loader) {
            delta_time = loader->getParameter("SIMULATION_DELTA_TIME");
            throttle_gain = loader->getParameter("THROTTLE_GAIN");
            mass = loader->getParameter("RADIO_CONTROL_MASS");
            gravity = loader->getParameter("RADIO_CONTROL_GRAVITY");
            angular_cycle = loader->getParameter("ANGULAR_CYCLE");
            angular_rate_cycle = loader->getParameter("ANGULAR_RATE_CYCLE");
            alt_control_cycle = loader->getParameter("ALT_CONTROL_CYCLE");

            pid_param_max_roll = loader->getParameter("PID_PARAM_MAX_ROLL");
            pid_param_max_pitch = loader->getParameter("PID_PARAM_MAX_PITCH");

            pid_param_h_rpm_max = loader->getParameter("PID_PARAM_H_RPM_MAX");
            pid_param_h_angle_rate_max = loader->getParameter("PID_PARAM_H_ANGLE_RATE_MAX");
            pid_param_yaw_rpm_max = loader->getParameter("PID_PARAM_YAW_RPM_MAX");
            pid_param_yaw_angle_rate_max = loader->getParameter("PID_PARAM_YAW_ANGLE_RATE_MAX");
            max_spd = loader->getParameter("PID_PARAM_MAX_SPD");
            head_control_cycle = loader->getParameter("HEAD_CONTROL_CYCLE");
            yaw_delta_value_deg = loader->getParameter("YAW_DELTA_VALUE_DEG");
            alt_delta_value_m = loader->getParameter("ALT_DELTA_VALUE_M");
            double v = loader->getParameter("RADIO_CONTROL_USE_SPD_CTRL");
            if (v == 0) {
                use_spd_ctrl = false;
            }
            else {
                use_spd_ctrl = true;
            }

        } else {
            delta_time = SIMULATION_DELTA_TIME;
            throttle_gain = THROTTLE_GAIN;
            mass = RADIO_CONTROL_MASS;
            gravity = RADIO_CONTROL_GRAVITY;
            angular_cycle = ANGULAR_CYCLE;
            angular_rate_cycle = ANGULAR_RATE_CYCLE;
            alt_control_cycle = ALT_CONTROL_CYCLE;

            pid_param_max_roll = PID_PARAM_MAX_ROLL;
            pid_param_max_pitch = PID_PARAM_MAX_PITCH;

            pid_param_h_rpm_max = PID_PARAM_H_RPM_MAX;
            pid_param_h_angle_rate_max = PID_PARAM_H_ANGLE_RATE_MAX;
            pid_param_yaw_rpm_max = PID_PARAM_YAW_RPM_MAX;
            pid_param_yaw_angle_rate_max = PID_PARAM_YAW_ANGLE_RATE_MAX;
            max_spd = PID_PARAM_MAX_SPD;
            head_control_cycle = HEAD_CONTROL_CYCLE;
            yaw_delta_value_deg = YAW_DELTA_VALUE_DEG;
#ifdef RADIO_CONTROL_USE_SPD_CTRL
            use_spd_ctrl = false;
#else
            use_spd_ctrl = true;
#endif
        }
    }

    double get_target_rate_value(PidControl *ctrl, double rpm_max, double max_angle_rate, double target, double current) {
        double target_rate_max = RPM2EULER_RATE(rpm_max);
        double target_rate = flight_controller_get_limit_value(target, 0, -target_rate_max, target_rate_max);
        double out_value = ctrl->calculate(target, NORMALIZE_RADIAN(current));
        return flight_controller_get_limit_value(out_value, 0, -max_angle_rate, max_angle_rate);
    }

    double get_target_value(PidControl *ctrl, double max_angle, double target, double current) {
        double out_value = ctrl->calculate(DEGREE2RADIAN(target), NORMALIZE_RADIAN(current));
        return out_value;
    }

    PidRateControlOutputType run_angular_rate_control(PidControlInputType roll, PidControlInputType pitch, PidControlInputType yaw) {
        PidRateControlOutputType out = prev_rate_out;
        if (angular_rate_simulation_time >= angular_rate_cycle) {
            out.p = get_target_rate_value(this->angular_rate_roll, pid_param_h_rpm_max, pid_param_h_angle_rate_max, roll.target, roll.current);
            out.q = get_target_rate_value(this->angular_rate_pitch, pid_param_h_rpm_max, pid_param_h_angle_rate_max, pitch.target, pitch.current);
            out.r = get_target_rate_value(this->angular_rate_yaw, pid_param_yaw_rpm_max, pid_param_yaw_angle_rate_max, yaw.target, yaw.current);
            angular_rate_simulation_time = 0;
            prev_rate_out = out;
        }
        this->angular_rate_simulation_time += this->delta_time;
        return out;
    }

    RadioControlPidControlOutputType run_angular_control(PidControlInputType roll, PidControlInputType pitch) {
        RadioControlPidControlOutputType out = prev_angle_out;
        if (angular_simulation_time >= angular_cycle) {
            out.roll_rate = get_target_value(this->angular_roll, pid_param_max_roll, roll.target, roll.current);
            out.pitch_rate = get_target_value(this->angular_pitch, pid_param_max_pitch, pitch.target, pitch.current);
            angular_simulation_time = 0;
        }
        this->angular_simulation_time += this->delta_time;
        return out;
    }

    double run_thrust_control(double power) {
        return (this->mass * this->gravity) + (this->throttle_gain * power);
    }

public:
    double pid_param_max_roll;
    double pid_param_max_pitch;
    bool use_spd_ctrl = false;
    double max_spd = PID_PARAM_MAX_SPD;
    double delta_time;
    AltitudeController alt;
    HeadingController heading;
    RadioController(PidControl *roll, PidControl *pitch, PidControl *rate_roll, PidControl *rate_pitch, PidControl *rate_yaw, double dt, double throttle_gain, double m, double g)
        : angular_roll(roll), angular_pitch(pitch), angular_rate_roll(rate_roll), angular_rate_pitch(rate_pitch), angular_rate_yaw(rate_yaw), delta_time(dt), throttle_gain(throttle_gain), mass(m), gravity(g) {
        if (HakoControllerParamLoader::is_exist_envpath()) {
            HakoControllerParamLoader loader;
            loader.loadParameters();
            initializeFromLoader(&loader);
        } else {
            initializeFromLoader(nullptr);
        }
    }

    RadioController(const std::string& filename)
        : delta_time(0), throttle_gain(0), mass(0), gravity(0), angular_cycle(0), angular_rate_cycle(0), alt_control_cycle(0),
          angular_roll(nullptr), angular_pitch(nullptr), angular_rate_roll(nullptr), angular_rate_pitch(nullptr), angular_rate_yaw(nullptr) {
        HakoControllerParamLoader loader(filename);
        loader.loadParameters();
        initializeFromLoader(&loader);
    }

    RadioController()
        : delta_time(0), throttle_gain(0), mass(0), gravity(0), angular_cycle(0), angular_rate_cycle(0), alt_control_cycle(0),
          angular_roll(nullptr), angular_pitch(nullptr), angular_rate_roll(nullptr), angular_rate_pitch(nullptr), angular_rate_yaw(nullptr) {
        if (HakoControllerParamLoader::is_exist_envpath()) {
            HakoControllerParamLoader loader;
            loader.loadParameters();
            initializeFromLoader(&loader);
        } else {
            initializeFromLoader(nullptr);
        }
    }

    virtual ~RadioController() {}


    // altitude control
    bool r_altitude_initialized = false;
    double r_altitude = 0;
    double alt_time = 0;

    void update_target_altitude(double v) {
        if (alt_time >= alt_control_cycle) {
            alt_time = 0;
            r_altitude += v * alt_delta_value_m;
            //std::cout << "v: " << v << "r_altitude: " << r_altitude << "alt_delta_value_m: " << alt_delta_value_m << std::endl;
            if (r_altitude >= ALT_VALUE_MAX) {
                r_altitude = ALT_VALUE_MAX;
            } else if (r_altitude < 0) {
                r_altitude = 0;
            }
        }
        alt_time += this->delta_time;
    }
    
    // Define constants
    double yaw_delta_value_deg = YAW_DELTA_VALUE_DEG;
    double head_control_cycle = HEAD_CONTROL_CYCLE;
    double yaw_time = 0;
    double r_yaw = 0;
    // Function to update target yaw
    void update_target_yaw(double v) {
        if (yaw_time >= head_control_cycle) {
            yaw_time = 0;
            r_yaw += v * yaw_delta_value_deg;
        }
        //std::cout << "r_yaw: " << r_yaw << std::endl;
        yaw_time += this->delta_time;
    }
    // speed control
    SpeedController spd;

    FlightControllerOutputType run(RadioControlInputType& in) {
        FlightControllerOutputType out;
        out.thrust = this->run_thrust_control(in.target_thrust);
        RadioControlPidControlOutputType ret_angle = this->run_angular_control({in.euler.x, in.target_roll}, {in.euler.y, in.target_pitch});
        PidRateControlOutputType ret = this->run_angular_rate_control({in.angular_rate.p, ret_angle.roll_rate}, {in.angular_rate.q, ret_angle.pitch_rate}, {in.angular_rate.r, in.target_angular_rate_r});
        out.torque_x = ret.p;
        out.torque_y = ret.q;
        out.torque_z = ret.r;
        return out;
    }
};


static inline RadioControllerParamType get_radio_control_default_parameters() {
    RadioControllerParamType param;

    if (HakoControllerParamLoader::is_exist_envpath()) {
        HakoControllerParamLoader loader;
        loader.loadParameters();
        param.mass = loader.getParameter("RADIO_CONTROL_MASS");
        param.gravity = loader.getParameter("RADIO_CONTROL_GRAVITY");
        param.delta_time = loader.getParameter("SIMULATION_DELTA_TIME");
        param.throttle_gain = loader.getParameter("THROTTLE_GAIN");
        param.angular_cycle = loader.getParameter("ANGULAR_CYCLE");
        param.angular_rate_cycle = loader.getParameter("ANGULAR_RATE_CYCLE");
        param.alt_control_cycle = loader.getParameter("ALT_CONTROL_CYCLE");
        param.roll.Kp = loader.getParameter("PID_PARM_ROLL_Kp");
        param.roll.Ki = loader.getParameter("PID_PARM_ROLL_Ki");
        param.roll.Kd = loader.getParameter("PID_PARM_ROLL_Kd");
        param.pitch.Kp = loader.getParameter("PID_PARM_PITCH_Kp");
        param.pitch.Ki = loader.getParameter("PID_PARM_PITCH_Ki");
        param.pitch.Kd = loader.getParameter("PID_PARM_PITCH_Kd");
        param.roll_rate.Kp = loader.getParameter("PID_PARM_ROLL_RATE_Kp");
        param.roll_rate.Ki = loader.getParameter("PID_PARM_ROLL_RATE_Ki");
        param.roll_rate.Kd = loader.getParameter("PID_PARM_ROLL_RATE_Kd");
        param.pitch_rate.Kp = loader.getParameter("PID_PARM_PITCH_RATE_Kp");
        param.pitch_rate.Ki = loader.getParameter("PID_PARM_PITCH_RATE_Ki");
        param.pitch_rate.Kd = loader.getParameter("PID_PARM_PITCH_RATE_Kd");
        param.yaw_rate.Kp = loader.getParameter("PID_PARM_YAW_RATE_Kp");
        param.yaw_rate.Ki = loader.getParameter("PID_PARM_YAW_RATE_Ki");
        param.yaw_rate.Kd = loader.getParameter("PID_PARM_YAW_RATE_Kd");

        param.pid_param_max_roll = loader.getParameter("PID_PARAM_MAX_ROLL");
        param.pid_param_max_pitch = loader.getParameter("PID_PARAM_MAX_PITCH");

        param.pid_param_h_rpm_max = loader.getParameter("PID_PARAM_H_RPM_MAX");
        param.pid_param_h_angle_rate_max = loader.getParameter("PID_PARAM_H_ANGLE_RATE_MAX");
        param.pid_param_yaw_rpm_max = loader.getParameter("PID_PARAM_YAW_RPM_MAX");
        param.pid_param_yaw_angle_rate_max = loader.getParameter("PID_PARAM_YAW_ANGLE_RATE_MAX");
        double v = loader.getParameter("RADIO_CONTROL_USE_SPD_CTRL");
        if (v == 0) {
            param.use_spd_ctrl = false;
        }
        else {
            param.use_spd_ctrl = true;
        }

    } else {
        param.mass = RADIO_CONTROL_MASS;
        param.gravity = RADIO_CONTROL_GRAVITY;
        param.delta_time = SIMULATION_DELTA_TIME;
        param.throttle_gain = THROTTLE_GAIN;
        param.angular_cycle = ANGULAR_CYCLE;
        param.angular_rate_cycle = ANGULAR_RATE_CYCLE;
        param.alt_control_cycle = ALT_CONTROL_CYCLE;
        param.roll.Kp = PID_PARM_ROLL_Kp;
        param.roll.Ki = PID_PARM_ROLL_Ki;
        param.roll.Kd = PID_PARM_ROLL_Kd;
        param.pitch.Kp = PID_PARM_PITCH_Kp;
        param.pitch.Ki = PID_PARM_PITCH_Ki;
        param.pitch.Kd = PID_PARM_PITCH_Kd;
        param.roll_rate.Kp = PID_PARM_ROLL_RATE_Kp;
        param.roll_rate.Ki = PID_PARM_ROLL_RATE_Ki;
        param.roll_rate.Kd = PID_PARM_ROLL_RATE_Kd;
        param.pitch_rate.Kp = PID_PARM_PITCH_RATE_Kp;
        param.pitch_rate.Ki = PID_PARM_PITCH_RATE_Ki;
        param.pitch_rate.Kd = PID_PARM_PITCH_RATE_Kd;
        param.yaw_rate.Kp = PID_PARM_YAW_RATE_Kp;
        param.yaw_rate.Ki = PID_PARM_YAW_RATE_Ki;
        param.yaw_rate.Kd = PID_PARM_YAW_RATE_Kd;

        param.pid_param_max_roll = PID_PARAM_MAX_ROLL;
        param.pid_param_max_pitch = PID_PARAM_MAX_PITCH;

        param.pid_param_h_rpm_max = PID_PARAM_H_RPM_MAX;
        param.pid_param_h_angle_rate_max = PID_PARAM_H_ANGLE_RATE_MAX;
        param.pid_param_yaw_rpm_max = PID_PARAM_YAW_RPM_MAX;
        param.pid_param_yaw_angle_rate_max = PID_PARAM_YAW_ANGLE_RATE_MAX;
#ifdef RADIO_CONTROL_USE_SPD_CTRL
        param.use_spd_ctrl = false;
#else
        param.use_spd_ctrl = true;
#endif

    }

    return param;
}

static inline RadioController* create_radio_controller(const RadioControllerParamType& param) {
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
    RadioController *rc = new RadioController(roll, pitch, roll_rate, pitch_rate, yaw_rate, param.delta_time, param.throttle_gain, param.mass, param.gravity);
    if (rc == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        return nullptr;
    }
    return rc;
}

#endif /* _RADIO_CONTROLLER_HPP_ */
