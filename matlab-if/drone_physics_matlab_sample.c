#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "drone_physics_matlab.h"
#include "../drone_physics/drone_physics_c.h"

mi_drone_acceleration_out_t mi_drone_acceleration(const mi_drone_acceleration_in_t* in) {
    dp_velocity_t vel = {in->u, in->v, in->w};
    dp_euler_t euler = {in->phi, in->theta, in->psi};
    dp_angular_velocity_t ang_vel = {in->p, in->q, in->r};
    dp_torque_t torque = {in->torque_x, in->torque_y, in->torque_z};

    dp_acceleration_t acc = dp_acceleration_in_body_frame(
        &vel, &euler, &ang_vel,
        in->thrust, in->mass, in->gravity, in->drag);

    dp_angular_acceleration_t ang_acc = dp_angular_acceleration_in_body_frame(
        &ang_vel,
        &torque,
        in->Ixx, in->Iyy, in->Izz);

    mi_drone_acceleration_out_t out = {
        acc.x, acc.y, acc.z,
        ang_acc.x, ang_acc.y, ang_acc.z
    };
    return out;
}

void mi_drone_acceleration_initialize(void)
{
    return;
}