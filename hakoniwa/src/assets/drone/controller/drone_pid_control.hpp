#ifndef _DRONE_PID_CONTROL_HPP_
#define _DRONE_PID_CONTROL_HPP_

#define DRONE_PID_CONTROL_CPP

#include <iostream>
#include <string>
#include "../../../utils/simple_pid.hpp"
#include "../../../utils/csv_data.hpp"

class DronePidControl {
private:
    PID pid_control;

    CsvData csv_data;

public:
    DronePidControl(double Kp, double Ki, double Kd, double setpoint, 
                    const std::string& csv_file_name, const std::vector<std::string>& csv_header)
        : pid_control(Kp, Ki, Kd, setpoint),
          csv_data(csv_file_name, csv_header) {}
    double calculate(double input) {
        return pid_control.calculate(input);
    }

    void write_to_csv(const std::vector<std::string>& data) {
        csv_data.write(data);
    }

    void flush_csv() {
        csv_data.flush();
    }

    void set_setpoint(double setpoint) {
        pid_control.set_setpoint(setpoint);
    }
};

extern void drone_pid_control_init();
extern void drone_pid_control_run();

#endif /* _DRONE_PID_CONTROL_HPP_ */