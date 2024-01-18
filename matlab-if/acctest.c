/* unit test for acceleration in matlab */
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "drone_physics_matlab.h"

#define assert_almost_equal(a, b) \
    assert(fabs((a) - (b)) < 0.0001 || (fprintf(stderr, "%s=%g,%s=%g\n", #a, (a), #b, (b)), 0))

int main() {
    mi_drone_acceleration_in_t in;
    in.phi = 0.0;
    in.theta = 0.0;
    in.psi = 0.0;
    in.u = 0.0;
    in.v = 0.0;
    in.w = 0.0;
    in.p = 0.0;
    in.q = 0.0;
    in.r = 0.0;
    in.thrust = 0.0;
    in.torque_x = 0.0;
    in.torque_y = 0.0;
    in.torque_z = 0.0;

    in.mass = 1.0;
    in.Ixx = 1.0;
    in.Iyy = 1.0;
    in.Izz = 1.0;
    in.gravity = 9.81;
    in.drag = 0.0;

    mi_drone_acceleration_out_t out = mi_drone_acceleration(&in);
    assert_almost_equal(out.du, 0.0);

    in.u = 1.0; in.v = 2.0; in.w = 3.0;
    in.Ixx = 2.0; in.Iyy = 5.0; in.Izz = 8.0;
    in.torque_x = 1.0; in.torque_y = 2.0; in.torque_z = 3.0;

    out = mi_drone_acceleration(&in);
    mi_drone_acceleration_out_t expected = {
        0, 0, 0,
        (in.torque_x - 2*3*(in.Izz - in.Iyy))/in.Ixx,
        (in.torque_y - 1*3*(in.Ixx - in.Izz))/in.Iyy,
        (in.torque_z - 1*2*(in.Iyy - in.Ixx))/in.Izz
    };
    assert_almost_equal(out.du, expected.du);

    
    return 0;
}