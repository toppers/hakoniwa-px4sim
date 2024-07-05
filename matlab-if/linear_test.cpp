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


static void log(const char* msg,
    const mi_drone_acceleration_out_t& o1,
    const mi_drone_acceleration_out_t& o2)
{
    (msg == nullptr) ? 0 : fprintf(stderr, "%s ", msg);
    fprintf(stderr, "1=(%g, %g, %g, %g, %g, %g) 2=(%g, %g, %g, %g, %g, %g)\n",
        o1.du, o1.dv, o1.dw, o1.dp, o1.dq, o1.dr,
        o2.du, o2.dv, o2.dw, o2.dp, o2.dq, o2.dr);
//    fprintf(stderr, "(du,dv,dw,dp,dq,dr) 1=(%g, %g, %g, %g, %g, %g)\n 2=(%g, %g, %g, %g, %g, %g)\n",
}

static void log(const char* pre,
    const mi_drone_acceleration_in_t& i1,
    const mi_drone_acceleration_in_t& i2, const char* post = nullptr)
{
    (pre == nullptr) ? 0 : fprintf(stderr, "%s", pre);
    fprintf(stderr, "1=(%g, %g, %g, %g, %g, %g) 2=(%g, %g, %g, %g, %g, %g)",
        i1.u, i1.v, i1.w, i1.p, i1.q, i1.r,
        i2.u, i2.v, i2.w, i2.p, i2.q, i2.r);
    (post == nullptr) ? 0 : fprintf(stderr, "%s", post);
}

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

extern "C" mi_drone_acceleration_out_t drone_acceleration_by_linear_at_hover(const mi_drone_acceleration_in_t* in);

static void increment(mi_drone_acceleration_in_t* in, const mi_drone_acceleration_out_t& out) {
    in->u += out.du;
    in->v += out.dv;
    in->w += out.dw;
    in->p += out.dp;
    in->q += out.dq;
    in->r += out.dr;
}

static void loop(int times, const mi_drone_acceleration_in_t& in) {
    mi_drone_acceleration_in_t in_p = in;
    mi_drone_acceleration_in_t in_l = in;

    for (int i = 0; i < times; i++) {
        mi_drone_acceleration_out_t out_p = drone_acceleration_by_physics(&in_p);
        mi_drone_acceleration_out_t out_l = drone_acceleration_by_linear_at_hover(&in_l);
        log(" diff", out_p, out_l);
        increment(&in_p, out_p);
        increment(&in_l, out_l);
        log("value", in_p, in_l);
    }
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
    mi_drone_acceleration_out_t out_m = drone_acceleration_by_linear_at_hover(&in);
    

    assert_almost_equal(out_p.du, 0.0);
    assert_almost_equal(out_m.du, 0.0);

    log("first try", out_p, out_m);
}

void test_up() {
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
    in.thrust = 10.0;
    in.torque_x = 0.0;
    in.torque_y = 0.0;
    in.torque_z = 0.0;

    in.mass = 1.0;
    in.Ixx = 1.0;
    in.Iyy = 1.0;
    in.Izz = 1.0;
    in.gravity = 9.81;
    in.drag = 0.0;

    loop(10, in);
}

void test_rotate() {
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
    in.thrust = 9.81;
    in.torque_x = 0.0;
    in.torque_y = 0.0;
    in.torque_z = 0.001;

    in.mass = 1.0;
    in.Ixx = 1.0;
    in.Iyy = 1.0;
    in.Izz = 1.0;
    in.gravity = 9.81;
    in.drag = 0.0;

    loop(10, in);

    in.torque_z = 0.0;
    in.torque_x = 0.001;

    loop(10, in);

    in.torque_x = 0.0;
    in.torque_y = 0.001;

    loop(10, in);
}


int main() {
    T(test_first_case);
    T(test_up);
    T(test_rotate);
    return 0;
}