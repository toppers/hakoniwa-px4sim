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

int main() {
    std::cout << "-------start unit test-------\n";
    test_all_unit_vectors_with_angle0();
    test_all_unit_vectors_with_some_angles();
    test_matrix_is_unitary();
    test_roundtrip();
    std::cout << "-------all test PASSSED!!----\n";
    return 0;
}