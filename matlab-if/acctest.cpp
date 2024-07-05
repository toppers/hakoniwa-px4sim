/* unit test for acceleration in matlab */
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "drone_physics_osdep.h"

#include "drone_physics_matlab.h"
#include "drone_physics_c.h"

#define assert_almost_equal(a, b) \
    assert(fabs((a) - (b)) < 0.0001 || (fprintf(stderr, "%s=%g <-?-> %s=%g\n", #a, (a), #b, (b)), 0))
#define T(f) do {fprintf(stderr, #f " "); f(); fprintf(stderr, "... PASS\n");} while(0)


/*
 * A function of similar specification by drone_physics
 * First, cross-check with this.
 */
static mi_drone_acceleration_out_t drone_acceleration_by_physics(const mi_drone_acceleration_in_t* in) {
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
static void test_first_case() {
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

    /* This function becomes the current specification */
    mi_drone_acceleration_out_t out_p = drone_acceleration_by_physics(&in);

    // calls the one from matlab
    mi_drone_acceleration_out_t out_m = mi_drone_acceleration(&in);
    

    assert_almost_equal(out_p.du, 0.0);
    assert_almost_equal(out_m.du, 0.0);

    in.u = 1.0; in.v = 2.0; in.w = 3.0;
    in.Ixx = 2.0; in.Iyy = 5.0; in.Izz = 8.0;
    in.torque_x = 1.0; in.torque_y = 2.0; in.torque_z = 3.0;

    out_p = drone_acceleration_by_physics(&in);

    // calls the one from matlab
    out_m = mi_drone_acceleration(&in);
    

    mi_drone_acceleration_out_t expected = {
        0, 0, 0,
        (in.torque_x - 2*3*(in.Izz - in.Iyy))/in.Ixx,
        (in.torque_y - 1*3*(in.Ixx - in.Izz))/in.Iyy,
        (in.torque_z - 1*2*(in.Iyy - in.Ixx))/in.Izz
    };
    /* remove +1 from the line below to pass the test */
    assert_almost_equal(out_p.du, expected.du);
    assert_almost_equal(out_m.dv, expected.dv);
}

void round_test()
{
    mi_drone_acceleration_in_t in;
    in.phi = 0.0;    in.theta = 0.0;    in.psi = 0.0;
    in.u = 0.0;    in.v = 0.0;    in.w = 0.0;
    in.p = 0.0;    in.q = 0.0;    in.r = 0.0;
    in.thrust = 0.0;
    in.torque_x = 0.0;    in.torque_y = 0.0;    in.torque_z = 0.0;
    in.mass = 1.0;
    in.Ixx = 1.0;    in.Iyy = 2.0;    in.Izz = 3.0;
    in.gravity = 9.81;
    in.drag = 0.0;

    for (double i = 0; i < M_PI; i += (M_PI/180)*30) {
        for (double j = 0; j < M_PI/2; j+= (M_PI/180)*30) {
            for (double k = 0; k < M_PI; k+= (M_PI/180)*60) {
                in.phi = i; in.theta = j; in.psi = k;
                in.u = 1.0; in.v = 2.0; in.w = 3.0;
                in.p = 1.0; in.q = 2.0; in.r = 3.0;
                in.thrust = 1.0;
                in.torque_x = 10.0; in.torque_y = 20.0; in.torque_z = 30.0;
                in.mass = 10.0;
                in.Ixx = 2.0; in.Iyy = 5.0; in.Izz = 8.0;
                in.gravity = 9.81;
                in.drag = 2.0;
                mi_drone_acceleration_out_t out_m = mi_drone_acceleration(&in);
                mi_drone_acceleration_out_t out_p = drone_acceleration_by_physics(&in);
                assert_almost_equal(out_m.du, out_p.du);
                assert_almost_equal(out_m.dv, out_p.dv);
                assert_almost_equal(out_m.dw, out_p.dw);
                assert_almost_equal(out_m.dp, out_p.dp);
                assert_almost_equal(out_m.dq, out_p.dq);
                assert_almost_equal(out_m.dr, out_p.dr);
                fprintf(stderr, ".");
            }
        }
    }

}

int main() {
    T(test_first_case);
    T(round_test);
    return 0;
}

