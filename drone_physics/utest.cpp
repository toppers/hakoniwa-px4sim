#include <cassert>
#ifdef __linux__
#include <cmath>
#endif /* __linux__ */
#include <iostream>
#include "body_physics.hpp"
#include "rotor_physics.hpp"

using namespace hako::drone_physics;

std::ostream& operator << (std::ostream& os, const VectorType& v) {
    os << "(" << std::get<0>(v) << ", " << std::get<1>(v) << ", " << std::get<2>(v) << ")";
    return os;
}

#define assert_almost_equal(a, b) \
    assert(std::fabs(std::get<0>(a) - std::get<0>(b)) < 0.0001 || (std::cerr << std::endl << #a "=" << a << ", " #b "=" << b << std::endl, 0)); \
    assert(std::fabs(std::get<1>(a) - std::get<1>(b)) < 0.0001 || (std::cerr << std::endl << #a "=" << a << ", " #b "=" << b << std::endl, 0)); \
    assert(std::fabs(std::get<2>(a) - std::get<2>(b)) < 0.0001 || (std::cerr << std::endl << #a "=" << a << ", " #b "=" << b << std::endl, 0))

#define print_vec(v) \
    std::cerr << #v "=" << std::get<0>(v) << ", " << std::get<1>(v) << ", " << std::get<2>(v) << std::endl

#define T(f) do {std::cerr<< #f ; f(); std::cerr << "... PASS" << std::endl;} while(false)

const double PI = 3.14159265358979323846;

void test_frame_all_unit_vectors_with_angle0() {
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

void test_frame_all_unit_vectors_with_some_angles() {
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

static double length(VelocityType v) {
    auto [x, y, z] = v; // C++17 structured binding declaration
    return sqrt(x*x + y*y + z*z);
}

void test_frame_matrix_is_unitary() {
    VelocityType v1(1, 0, 0);
    for (int i = -180; i < 180; i+=30) {
        VelocityType v2 = velocity_body_to_ground(v1, AngleType(i * (PI/180), 0, 0));
        double len = length(v2);
        assert(fabs(len - 1.0) < 0.0001);
    }
    for (int i = 0; i < 90; i+=30) {
        VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, i * (PI/180), 0));
        double len = length(v2);
        assert(fabs(len - 1.0) < 0.0001);
    }
  
    for (int i = -180; i < 360; i+=30) {
        VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, 0, i * (PI/180)));
        double len = length(v2);
        assert(fabs(len - 1.0) < 0.0001);
    }

    // conbinations
    VelocityType u1 = {0, 1, 0};
    VelocityType w1 = {0, 0, 1};
    for (int i = -180; i < 180; i+=30) {
        for (int j = -90; j < 90; j+=30) {
            for (int k = -180; k < 180; k+=30) {
                VelocityType V = velocity_body_to_ground(v1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                double len = length(V);
                assert(fabs(len - 1.0) < 0.0001);

                // bug #89 indicated that need testing (0,1,0) and (0,0,1) vectors.
                V = velocity_body_to_ground(u1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                len = length(V);
                assert(fabs(len - 1.0) < 0.0001);

                V = velocity_body_to_ground(w1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                len = length(V);
                assert(fabs(len - 1.0) < 0.0001);

            }
        }
    }
}

void test_issue_89_yaw_angle_bug() {
    // this works ok.
    VelocityType v1(0, 1, 0);
    VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, 0, PI/2));
    assert_almost_equal(v2, VelocityType(-1, 0, 0));
    // v2 = (-1, 0, 1)); bug #89 produced this result. now it is (-1,0,0)
}

void test_frame_roundtrip() {
    VelocityType v1(1, 0, 0);
    VelocityType v2 = velocity_body_to_ground(v1, AngleType(0, 0, 0));
    assert_almost_equal(v1, v2);

    for (int i = -180; i < 180; i+=30) {  // 0 to 360 degree x-axis
        v2 = velocity_body_to_ground(v1, AngleType(i * PI / 180, 0, 0));
        VelocityType v3 = velocity_ground_to_body(v2, AngleType(i * PI / 180, 0, 0));
        assert_almost_equal(v1, v3);
    }
    for (int i = -90; i < 90; i+=30) {  // 0 to 360 degree x-axis
        v2 = velocity_body_to_ground(v1, AngleType(0, i * PI / 180, 0));
        VelocityType v3 = velocity_ground_to_body(v2, AngleType(0, i * PI / 180, 0));
        assert_almost_equal(v1, v3);
    }
    // conbinations
    VelocityType u1 = {1, 0, 0};
    v1 = {0, 1, 0};
    VectorType w1 = {0, 0, 1};
    for (int i = -180; i < 180; i+=30) {
        for (int j = -90; j < 90; j+=30) {
            for (int k = -180; k < 180; k+=30) {
                v2 = velocity_body_to_ground(v1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                VelocityType v3 = velocity_ground_to_body(v2, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                assert_almost_equal(v1, v3);

                // bug #89 indicated that need testing (0,1,0) and (0,0,1) vectors.
                v2 = velocity_body_to_ground(u1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                v3 = velocity_ground_to_body(v2, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                assert_almost_equal(u1, v3);

                v2 = velocity_body_to_ground(w1, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                v3 = velocity_ground_to_body(v2, AngleType(i * (PI/180), j * (PI/180), k * (PI/180)));
                assert_almost_equal(w1, v3);
            }
        }
    }
}

void test_angular_frame_roundtrip() {
    AngularVelocityType v1(1, 0, 0);
    AngularVelocityType v2 = angular_velocity_body_to_ground(v1, AngleType(0, 0, 0));
    assert_almost_equal(v1, v2);

    for (int i = -180; i < 180; i+=30) {  // 0 to 360 degree x-axis
        v2 = angular_velocity_body_to_ground(v1, AngleType(i * PI / 180, 0, 0));
        AngularVelocityType v3 = angular_velocity_ground_to_body(v2, AngleType(i * PI / 180, 0, 0));
        assert_almost_equal(v1, v3);
    }
    for (int i = -90; i < 90; i+=30) {  // y-axis
        v2 = angular_velocity_body_to_ground(v1, AngleType(0, i * PI / 180, 0));
        AngularVelocityType v3 = angular_velocity_ground_to_body(v2, AngleType(0, i * PI / 180, 0));
        assert_almost_equal(v1, v3);
    }
    for (int i = -180; i < 180; i+=30) {  // z-axis
        v2 = angular_velocity_body_to_ground(v1, AngleType(0, 0, i * PI / 180));
        AngularVelocityType v3 = angular_velocity_ground_to_body(v2, AngleType(0, 0, i * PI / 180));
        assert_almost_equal(v1, v3);
    }
    // conbinations
    AngularVelocityType u1 = {1, 0, 0};
    v1 = {0, 1, 0};
    VectorType w1 = {0, 0, 1};
    VectorType ans;
    for (int i = -90; i < 180; i+=30) {
         for (int j = -90; j < 90; j+=30) {
            if (j == -90) continue;  // gimbal lock
            for (int k = -180; k < 180; k+=30) {
                AngleType rot_angle(i * (PI/180), j * (PI/180), k * (PI/180));

                v2 = angular_velocity_body_to_ground(v1, rot_angle);
                ans = angular_velocity_ground_to_body(v2, rot_angle);
                assert_almost_equal(v1, ans);

                v2 = angular_velocity_body_to_ground(u1, rot_angle);
                ans= angular_velocity_ground_to_body(v2, rot_angle);
                assert_almost_equal(u1, ans);

                v2 = angular_velocity_body_to_ground(w1, rot_angle);
                ans = angular_velocity_ground_to_body(v2, rot_angle);
                assert_almost_equal(w1, ans);
            }
         }
    }
}

void test_body_acceleration() {
    const VelocityType v(1, 2, 3);

    double trust = 1, mass = 1, gravity = 1, drag = 0;
    AccelerationType a = acceleration_in_body_frame(v, AngleType(0, 0, 0), AngularVelocityType(0, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(0, 0, 0));

    trust = 10, mass = 2, gravity = 1, drag = 0;
    a = acceleration_in_body_frame(v, AngleType(0, 0, 0), AngularVelocityType(0, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(0, 0, -trust/mass+gravity));

    /* change psi angle (doesn't matter) */
    a = acceleration_in_body_frame(v, AngleType(0, 0, PI/6), AngularVelocityType(0, 0, 0),
        trust, mass, gravity, drag);
        assert_almost_equal(a, AccelerationType(0, 0, -trust/mass+gravity));

    /* change phi */
    a = acceleration_in_body_frame(v, AngleType(PI/6, 0, 0), AngularVelocityType(0, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(0, gravity*sin(PI/6), -trust/mass+gravity*cos(PI/6)));

    /* change theta */
    a = acceleration_in_body_frame(v, AngleType(0, PI/6, 0), AngularVelocityType(0, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(-gravity*sin(PI/6), 0, -trust/mass+gravity*cos(PI/6)));

    /* add drag */
    trust = 10, mass = 2, gravity = 1, drag = 0.1;
    a = acceleration_in_body_frame(v, AngleType(0, PI/6, 0), AngularVelocityType(0, 0, 0),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(
        -gravity*sin(PI/6)-drag/mass*1,
                          -drag/mass*2,
        -trust/mass+gravity*cos(PI/6)- drag/mass*3));

    trust = 10, mass = 2, gravity = 1, drag = 0;
    // setting angle to (0,0,0), drag = 0, same anglular and linear velocity, so Coliori=(0,0,0)
    a = acceleration_in_body_frame(v, AngleType(0, 0, 0), AngularVelocityType(1, 2, 3),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(0, 0, -trust/mass+gravity));
    
    // now Coliori is (1,1,1)x(1,2,3) = (1,-2,1)
    a = acceleration_in_body_frame(v, AngleType(0, 0, 0), AngularVelocityType(1, 1, 1),
        trust, mass, gravity, drag);
    assert_almost_equal(a, AccelerationType(-1, 2, -trust/mass+gravity-1));
}

void test_body_angular_acceleration() {
    const AngularVelocityType v(1, 2, 3);
    double I_xx = 1, I_yy = 1, I_zz = 1, torque_x = 0, torque_y = 0, torque_z = 0;
    AngularAccelerationType a = angular_acceleration_in_body_frame(v, torque_x, torque_y, torque_z, I_xx, I_yy, I_zz);
    assert_almost_equal(a, AngularAccelerationType(0, 0, 0));

    I_xx = 1, I_yy = 1, I_zz = 1, torque_x = 1, torque_y = 2, torque_z = 3;
    a = angular_acceleration_in_body_frame(v, torque_x, torque_y, torque_z, I_xx, I_yy, I_zz);
    assert_almost_equal(a, AngularAccelerationType(1, 2, 3));

    I_xx = 2, I_yy = 5, I_zz = 8, torque_x = 1, torque_y = 2, torque_z = 3;
    // remember v = (p, q, r) = {1, 2, 3}
    a = angular_acceleration_in_body_frame(v, torque_x, torque_y, torque_z, I_xx, I_yy, I_zz);
    assert_almost_equal(a, AngularAccelerationType(
        (torque_x - 2*3*(I_zz - I_yy))/I_xx,
        (torque_y - 1*3*(I_xx - I_zz))/I_yy,
        (torque_z - 1*2*(I_yy - I_xx))/I_zz));
}

#include <fstream>
void test_rotor_omega_accesleration() {
    double Kr = 0.1, Tr = 1, duty_rate = 1, omega = 1;
    double a = rotor_omega_acceleration(Kr, Tr, omega, duty_rate);
    assert(a == (Kr * duty_rate - omega) / Tr);

    std::ofstream ofs;
    ofs.open("omega_times_series.csv", std::ios::out);
    ofs << "TIME,ACC,OMEGA" << std::endl;

    Kr = 1, Tr = 100, duty_rate = 0.5, omega = 0;

    for (double time = 0; time < 3; time += 0.01) {
        double a = rotor_omega_acceleration(Kr, Tr, omega, duty_rate);
        assert(a == (Kr * duty_rate - omega) / Tr);
        omega += a;

        /** differencial equation
         *  d(Omega)/dt = ( Kr * (duty rate) - (Omega) ) / Tr
         *  if duty_rate is a constant, the solution is
         *  Omega = Kr * (1 - exp(-t/Tr) * (duty rate))
         */
        assert(std::fabs(omega -  Kr*(1 - std::exp(-time/Tr)*duty_rate) < 0.0001));
        ofs << time << "," << a << "," << omega << std::endl;
    }
    ofs.close();
}

void test_vector_operators(){
    VectorType v1(1, 2, 3);
    VectorType v2(4, 5, 6);
    assert_almost_equal(v1+v2, VectorType(5, 7, 9));
    assert_almost_equal(cross(v1,v2), VectorType(-3, 6, -3));
}

void test_rotor_thrust() {
    double A = 1, omega = 10;
    double thrust = rotor_thrust(A, omega);
    assert(thrust == A * (omega * omega));
}

void test_rotor_anti_torque() {
    double B = 1, Jr = 1, omega = 10, omega_acceleration = 1;
    /* ccw=1, means the rotor is rotating ccw, so the anti-torque is cw=positive(+z=down) */
    double torque = rotor_anti_torque(B, Jr, omega, omega_acceleration, 1);
    assert(torque == B * omega * omega + Jr * omega_acceleration);

    /* ccw=-1, means the rotor is rotating cw, so the anti-torque is ccw=negative(-z=up) */
    torque = rotor_anti_torque(B, Jr, omega, omega_acceleration, -1);
    assert(torque == -B * omega * omega - Jr * omega_acceleration);
}

void test_body_thrust() {
    double A = 1;
    unsigned n = 4;
    double omega[4] = {1, 2, 3, 4};
    double thrust = body_thrust(A, n, omega);
    assert(thrust == 1*1 + 2*2 + 3*3 + 4*4);
}

void test_body_torque() {
    double A = 1, B = 1, Jr = 1;
    unsigned n = 4;
    PositionType position[4] = { // + shape coordinaites
        {0.3, 0.0, 0},  // +x  (front)
        {0.0, -0.3, 0}, // -y  (left)
        {-0.3, 0.0, 0}, // -x  (back)
        {0.0, 0.3, 0}   // +y  (right)
    };
    double ccw[4] = {-1, 1, -1, 1};
    double omega[4] = {100, 100, 100, 100};
    double omega_acceleration[4] = {0, 0, 0, 0};
    TorqueType torque = body_torque(A, B, Jr, n, position, ccw, omega, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, 0, 0)); // ZERO TORQUE

    double omega2[4] = {100, 200, 100, 200};
    A = 1; B = 0;
    torque = body_torque(A, B, Jr, n, position, ccw, omega2, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, 0, 0));

    /** HERE, four omega's one each */
    double omega3[4] = {100, 0, 0, 0};
    /* only get rotor0 torque */
    torque = body_torque(A, B, Jr, n, position, ccw, omega3, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, (0.3)*A*100*100, 0));

    double omega4[4] = {0, 100, 0, 0};
    torque = body_torque(A, B, Jr, n, position, ccw, omega4, omega_acceleration);
    assert_almost_equal(torque, TorqueType((0.3)*A*100*100, 0, 0));

    double omega5[4] = {0, 0, 100, 0};
    torque = body_torque(A, B, Jr, n, position, ccw, omega5, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, -(0.3)*A*100*100, 0));

    double omega6[4] = {0, 0, 0, 100};
    torque = body_torque(A, B, Jr, n, position, ccw, omega6, omega_acceleration);
    assert_almost_equal(torque, TorqueType(-(0.3)*A*100*100, 0, 0));
}

void test_body_anti_torque() {
    double A = 10, B = 1, Jr = 1;
    unsigned n = 4;
    PositionType position[4] = { // + shape coordinaites
        {0.3, 0.0, 0},  // +x  (front)
        {0.0, -0.3, 0}, // -y  (left)
        {-0.3, 0.0, 0}, // -x  (back)
        {0.0, 0.3, 0}   // +y  (right)
    };
    double ccw[4] = {-1, 1, -1, 1};
    double omega[4] = {100, 100, 100, 100};
    double omega_acceleration[4] = {0, 0, 0, 0};
    TorqueType torque = body_torque(A, B, Jr, n, position, ccw, omega, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, 0, 0)); // ZERO TORQUE

    omega[1] = omega[3] = 200;  // increase CWW rotors
    torque = body_torque(A, B, Jr, n, position, ccw, omega, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, 0, 2*(200*200-100*100)*B));

    omega[0] = omega[2] = 200;  // increase CW rotors
    omega[1] = omega[3] = 100;
    torque = body_torque(A, B, Jr, n, position, ccw, omega, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, 0, -2*(200*200-100*100)*B));
}

void test_body_anti_Jr_torque() {
    unsigned n = 4;
    PositionType position[4] = { // + shape coordinaites
        {0.3, 0.0, 0},  // +x  (front)
        {0.0, -0.3, 0}, // -y  (left)
        {-0.3, 0.0, 0}, // -x  (back)
        {0.0, 0.3, 0}   // +y  (right)
    };
    double ccw[4] = {-1, 1, -1, 1};
    double omega[4] = {100, 100, 100, 100};
    double omega_acceleration[4] = {0, 0, 0, 10};
    double A = 10, B = 0, Jr = 1;
    TorqueType torque = body_torque(A, B, Jr, n, position, ccw, omega, omega_acceleration);
    assert_almost_equal(torque, TorqueType(0, 0, 10*Jr));
}

int main() {
    std::cerr << "-------start unit test-------\n";
    T(test_frame_all_unit_vectors_with_angle0);
    T(test_frame_all_unit_vectors_with_some_angles);
    T(test_frame_matrix_is_unitary);
    T(test_frame_roundtrip);
    T(test_body_acceleration);
    T(test_angular_frame_roundtrip);
    T(test_body_angular_acceleration);
    T(test_rotor_omega_accesleration);
    T(test_vector_operators);
    T(test_rotor_thrust);
    T(test_rotor_anti_torque);
    T(test_body_thrust);
    T(test_body_torque);
    T(test_body_anti_torque);
    T(test_body_anti_Jr_torque);
    std::cerr << "-------all standard test PASSSED!!----\n";
    T(test_issue_89_yaw_angle_bug);
    std::cerr << "-------all bug issue test PASSSED!!----\n";
    return 0;
}
