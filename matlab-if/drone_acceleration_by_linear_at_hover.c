#include "drone_physics_matlab.h"
#include <assert.h>
#include <math.h>

mi_drone_acceleration_out_t drone_acceleration_by_linear_at_hover(const mi_drone_acceleration_in_t* in) {
    double phi = in->phi;
    double theta = in->theta;
    double psi = in->psi;
    double u = in->u;
    double v = in->v;
    double w = in->w;
    double p = in->p;
    double q = in->q;
    double r = in->r;
    double T = in->thrust;
    double tx = in->torque_x;
    double ty = in->torque_y;
    double tz = in->torque_z;

    /* other constants */
    double m = in->mass;
    double Ixx = in->Ixx;
    double Iyy = in->Iyy;
    double Izz = in->Izz;
    double g = in->gravity;
    double d = in->drag;

    double T0 = m*g; // equilibrium
    T = T - T0;   // delta from equilibrium
    /* others are 0 equilibrium */

    return (mi_drone_acceleration_out_t) {
        .du = -(d/m)*u - g*theta,
        .dv = -(d/m)*v + g*phi,
        .dw = -(d/m)*w - T/m,
        .dp = tx/Ixx,
        .dq = ty/Iyy,
        .dr = tz/Izz
    };
}

