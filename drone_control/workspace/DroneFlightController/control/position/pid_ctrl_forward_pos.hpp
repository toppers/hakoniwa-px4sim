#ifndef _PID_CTRL_FORWARD_POS_HPP_
#define _PID_CTRL_FORWARD_POS_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "utils/frame_convertor.hpp"
#include "primitive_types.h"
#include <cmath> // for std::sqrt, std::pow, std::sin, std::cos
#include <iostream>

class PidCtrlForwardPos {
private:
    PID pidF;
    PID pidH;
    double length(const PositionType& target_pos, const PositionType& current_pos)
    {
        return std::sqrt(std::pow(target_pos.x - current_pos.x, 2) + std::pow(target_pos.y - current_pos.y, 2));
    }
    double length(const PositionType& pos)
    {
        return std::sqrt(std::pow(pos.x, 2) + std::pow(pos.y, 2));
    }
public:
    PidCtrlForwardPos() : pidF(FORWARD_POS_Kp, FORWARD_POS_Ki, FORWARD_POS_Kd, 0),
                          pidH(FORWARD_POS_Kp, FORWARD_POS_Ki, FORWARD_POS_Kd, 0)
    {
    }
    std::pair<double, double> run(const PositionType& target_pos, const PositionType& current_pos, const EulerType& euler)
    {
        PositionType relative_vector = { target_pos.x - current_pos.x, target_pos.y - current_pos.y, 0};
        double relative_len = length(relative_vector);
        double current_psi = euler.psi;
        double relative_psi = atan2(relative_vector.y, relative_vector.x) - current_psi;

        double diffF_local = -relative_len * cos(relative_psi);
        double diffH_local = relative_len * sin(relative_psi);
        //std::cout << "diffF: " << diffF_local << std::endl;
        //std::cout << "diffH: " << diffH_local << std::endl;
        pidF.set_target(0);
        pidH.set_target(0);

        double controlOutputF = pidF.calculate(diffF_local);
        double controlOutputH = pidH.calculate(diffH_local);

        return {controlOutputF, controlOutputH};
    }
};
#endif /* _PID_CTRL_FORWARD_POS_HPP_ */
