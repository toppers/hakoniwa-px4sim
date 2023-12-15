#include <cassert>
#include <iostream>
#include <numbers>
#include "math_and_physics.hpp"

#define assert_almost_equal(a, b) \
    assert(std::fabs(std::get<0>(a) - std::get<0>(b)) < 0.0001); \
    assert(std::fabs(std::get<1>(a) - std::get<1>(b)) < 0.0001); \
    assert(std::fabs(std::get<2>(a) - std::get<2>(b)) < 0.0001)

#define print_vec(v) \
    std::cout << #v "=" << std::get<0>(v) << ", " << std::get<1>(v) << ", " << std::get<2>(v) << std::endl

#define T(f) do {std::cout<< #f ; f(); std::cout << "... PASS" << std::endl;} while(false)

const double PI = 3.14159265358979323846;

void test_all_unit_vectors_with_angle0() {
    VelocityType v1(1, 0, 0);
    VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, 0, 0));
    assert_almost_equal(v1, v2);

    v1 = {0, 1, 0};
    v2 = velocity_body_to_ground(v1, AngleType(0, 0, 0));
    assert_almost_equal(v1, v2);

    v1 = {0, 0, 1};
    v2 = velocity_body_to_ground(v1, AngleType(0, 0, 0));
    assert_almost_equal(v1, v2);
}

void test_all_unit_vectors_with_some_angles() {
    const VelocityType v1(1, 0, 0);
    VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, 0, 0));
    assert_almost_equal(v1, v2);

    for (int i = 0; i < 360; i++) {  // 0 to 360 degree x-axis
        v2 = velocity_body_to_ground(v1, AngleType(i * PI / 180, 0, 0));
        assert_almost_equal(v1, v2);
    }

    v2 = velocity_body_to_ground(v1, AngleType(0, PI/6, 0)); // 30 degree head up
    assert_almost_equal(VelocityType(sqrt(3)/2, 0, -0.5), v2);

    v2 = velocity_body_to_ground(v1, AngleType(0, PI/3, 0)); // 60 degree
    assert_almost_equal(VelocityType(0.5, 0, -sqrt(3)/2), v2);

    v2 = velocity_body_to_ground(v1, AngleType(0, 0, PI/6)); // yaw 30 degree
    assert_almost_equal(VelocityType(sqrt(3)/2, 0.5, 0), v2);
}

double length(VelocityType v) {
    double x = std::get<0>(v);
    double y = std::get<1>(v);
    double z = std::get<2>(v);
    return sqrt(x*x + y*y + z*z);
}

void test_matrix_is_unitary() {
    VelocityType v1(1, 0, 0);
    for (int i = 0; i < 360; i+=30) {
        VelocityType v2 = velocity_body_to_ground(v1, AngleType(i * (PI/180), 0, 0));
        double len = length(v2);
        assert(fabs(len - 1.0) < 0.0001);
    }
    for (int i = 0; i < 360; i+=30) {
        VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, i * (PI/180), 0));
        double len = length(v2);
        assert(fabs(len - 1.0) < 0.0001);
    }
  
    for (int i = 0; i < 360; i+=30) {
        VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, 0, i * (PI/180)));
        double len = length(v2);
        assert(fabs(len - 1.0) < 0.0001);
    }

    // conbinations
    for (int i = 0; i < 360; i+=30) {
        for (int j = 0; j < 360; j+=30) {
            for (int k = 0; k < 360; k+=30) {
                VelocityType v2 = velocity_body_to_ground(v1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                double len = length(v2);
                assert(fabs(len - 1.0) < 0.0001);
            }
        }
    }
}

void test_roundtrip() {
    const VelocityType v1(1, 0, 0);
    VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, 0, 0));
    assert_almost_equal(v1, v2);

    for (int i = 0; i < 360; i+=30) {  // 0 to 360 degree x-axis
        v2 = velocity_body_to_ground(v1, AngleType(i * PI / 180, 0, 0));
        VelocityType v3 = velocity_ground_to_body(v2, AngleType(i * PI / 180, 0, 0));
        assert_almost_equal(v1, v3);
    }
    for (int i = 0; i < 360; i+=30) {  // 0 to 360 degree x-axis
        v2 = velocity_body_to_ground(v1, AngleType(0, i * PI / 180, 0));
        VelocityType v3 = velocity_ground_to_body(v2, AngleType(0, i * PI / 180, 0));
        assert_almost_equal(v1, v3);
    }
    // conbinations
    for (int i = 0; i < 360; i+=30) {
        for (int j = 0; j < 360; j+=30) {
            for (int k = 0; k < 360; k+=30) {
                v2 = velocity_body_to_ground(v1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                VelocityType v3 = velocity_ground_to_body(v2, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                assert_almost_equal(v1, v3);
            }
        }
    }


}

void test_body_acceleration() {
    const VelocityType v(1, 2, 3);

    double trust = 1, mass = 1, gravity = 1, drag = 0;
    AccelerationType a = acceleration_in_body_frame(v, AngleType(0, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(0, 0, 0));

    trust = 10, mass = 2, gravity = 1, drag = 0;
    a = acceleration_in_body_frame(v, AngleType(0, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(0, 0, -trust/mass+gravity));

    /* change psi angle (doesn't matter) */
    a = acceleration_in_body_frame(v, AngleType(0, 0, PI/6),
        trust, mass, gravity, drag);
        assert_almost_equal(a, AccelerationType(0, 0, -trust/mass+gravity));

    /* change phi */
    a = acceleration_in_body_frame(v, AngleType(PI/6, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(0, gravity*sin(PI/6), -trust/mass+gravity*cos(PI/6)));

    /* change theta */
    a = acceleration_in_body_frame(v, AngleType(0, PI/6, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(-gravity*sin(PI/6), 0, -trust/mass+gravity*cos(PI/6)));

    /* add drag */
    trust = 10, mass = 2, gravity = 1, drag = 0.1;
    a = acceleration_in_body_frame(v, AngleType(0, PI/6, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(-gravity*sin(PI/6)-0.1, -0.2, -trust/mass+gravity*cos(PI/6)-0.3));

}

void test_body_angular_acceleration() {
    const AngularVelocityType v(1, 2, 3);
    double I_xx = 1, I_yy = 1, I_zz = 1, torque_x = 0, torque_y = 0, torque_z = 0;
    AngularAccelerationType a = angular_acceleration_in_body_frame(v, AngleType(0, 0, 0),
        torque_x, torque_y, torque_z, I_xx, I_yy, I_zz);
    assert_almost_equal(a, AngularAccelerationType(0, 0, 0));

    I_xx = 1, I_yy = 1, I_zz = 1, torque_x = 1, torque_y = 2, torque_z = 3;
    a = angular_acceleration_in_body_frame(v, AngleType(0, 0, 0),
        torque_x, torque_y, torque_z, I_xx, I_yy, I_zz);
    assert_almost_equal(a, AngularAccelerationType(1, 2, 3));

    I_xx = 2, I_yy = 5, I_zz = 8, torque_x = 1, torque_y = 2, torque_z = 3;
    // remember v = (p, q, r) = {1, 2, 3}
    a = angular_acceleration_in_body_frame(v, AngleType(0, 0, 0),
        torque_x, torque_y, torque_z, I_xx, I_yy, I_zz);
    assert_almost_equal(a, AngularAccelerationType(
        (torque_x - 2*3*(I_zz - I_yy))/I_xx,
        (torque_y - 1*3*(I_xx - I_zz))/I_yy,
        (torque_z - 1*2*(I_yy - I_xx))/I_zz));
}
    

int main() {
    std::cout << "-------start unit test-------\n";
    T(test_all_unit_vectors_with_angle0);
    T(test_all_unit_vectors_with_some_angles);
    T(test_matrix_is_unitary);
    T(test_roundtrip);
    T(test_body_acceleration);
    T(test_body_angular_acceleration);
    std::cout << "-------all test PASSSED!!----\n";
    return 0;
}