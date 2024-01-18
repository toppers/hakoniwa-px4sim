#include "drone_physics_debug.h"

const double PI = M_PI;

void static test_frame_all_unit_vectors_with_some_angles() {
    const dp_velocity_t v1 = {1, 0, 0};
    dp_velocity_t v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){0, 0, 0});
    assert_almost_equal(v1, v2);

    for (int i = 0; i < 360; i++) {  // 0 to 360 degree x-axis
        v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){i * PI / 180, 0, 0});
        assert_almost_equal(v1, v2);
    }

    v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){0, PI/6, 0}); // 30 degree head up
    assert_almost_equal(((dp_velocity_t){sqrt(3)/2, 0, -0.5}), v2);

    v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){0, PI/3, 0}); // 60 degree
    assert_almost_equal(((dp_velocity_t){0.5, 0, -sqrt(3)/2}), v2);

    v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){0, 0, PI/6}); // yaw 30 degree
    assert_almost_equal(((dp_velocity_t){sqrt(3)/2, 0.5, 0}), v2);
}

void static test_frame_roundtrip() {
    dp_velocity_t v1 = {1, 0, 0};
    dp_velocity_t v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){PI/6, PI/4, 0});
    dp_velocity_t v3 = dp_body_vector_from_ground(&v2, &(dp_euler_t){PI/6, PI/4, 0});
    assert_almost_equal(v1, v3);

    v1 = (dp_velocity_t){0, 1, 0};
    v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){PI/6, PI/3, PI/2});
    v3 = dp_body_vector_from_ground(&v2, &(dp_euler_t){PI/6, PI/3, PI/2});
    assert_almost_equal(v1, v3);

    v1 = (dp_velocity_t){0, 0, 1};
    v2 = dp_ground_vector_from_body(&v1, &(dp_euler_t){PI/8, -PI/3, PI/6});
    v3 = dp_body_vector_from_ground(&v2, &(dp_euler_t){PI/8, -PI/3, PI/6});
    assert_almost_equal(v1, v3);
}

void static test_body_acceleration()
{
    // some of the test cases from utest.cpp, skip others.
    dp_velocity_t v1 = {1, 2, 3};
    dp_euler_t e1 = {0, 0, 0};
    dp_angular_velocity_t a1 = {0, 0, 0};
    double trust = 1, mass = 1, gravity = 1, drag = 0;
    dp_acceleration_t acc =  dp_acceleration_in_body_frame(&v1, &e1, &a1, trust, mass, gravity, drag);
    assert_almost_equal(acc, ((dp_velocity_t){0, 0, 0}));

    /* add drag */
    dp_euler_t e2 = {0, PI/6, 0}; dp_angular_velocity_t a2 = {0, 0, 0};
    trust = 10, mass = 2, gravity = 1, drag = 0.1;
    acc = dp_acceleration_in_body_frame(&v1, &e2, &a2, trust, mass, gravity, drag);
    assert_almost_equal((acc), ((dp_acceleration_t){
        -gravity*sin(PI/6)-drag/mass*1,
        -drag/mass*2,
        -trust/mass+gravity*cos(PI/6)- drag/mass*3}
        ));
}

static void test_body_angular_acceleration()
{
    const dp_angular_velocity_t  v = {1, 2, 3};
    double I_xx = 1, I_yy = 1, I_zz = 1, torque_x = 0, torque_y = 0, torque_z = 0;
    I_xx = 2, I_yy = 5, I_zz = 8, torque_x = 1, torque_y = 2, torque_z = 3;

    dp_angular_acceleration_t a = dp_angular_acceleration_in_body_frame(&v, torque_x, torque_y, torque_z, I_xx, I_yy, I_zz);
    dp_angular_acceleration_t expected = {
        (torque_x - 2*3*(I_zz - I_yy))/I_xx,
        (torque_y - 1*3*(I_xx - I_zz))/I_yy,
        (torque_z - 1*2*(I_yy - I_xx))/I_zz
    };
    assert_almost_equal(a, expected);
}


int main() {
    T(test_frame_all_unit_vectors_with_some_angles);
    T(test_frame_roundtrip);
    T(test_body_acceleration);
    T(test_body_angular_acceleration);
    return 0;
}