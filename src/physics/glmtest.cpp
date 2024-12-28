#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <cmath>

#define assert_almost_equal_dvec4(u, v) assert(std::abs(u[0]-v[0]) < 0.001 && std::abs(u[1]-v[1]) < 0.001 && std::abs(u[2]-v[2]) < 0.001 && std::abs(u[3]-v[3]) < 0.001 || (out("dvec4 different! u=", u, " "), out("v=", v), false))

void out(const char* msg, const glm::dvec4& v, const char* cont = "\n") {
    std::cout << msg << "(" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << ")" << cont;
}
void out(const char* msg, double d, const char* cont = "\n") {
    std::cout << msg << "(" << d << ")" << cont;
}

void simple_test() {
    using namespace glm;

    dvec4 v1(1, 1, 1, 1);
    dvec4 v2 = v1 * 2.0;
    assert_almost_equal_dvec4(v2, dvec4(2, 2, 2, 2));

    dmat4 I(1.0);
    dvec4 v3 = I*v2;
    assert_almost_equal_dvec4(v3, v2);
}

const int ROTOR_NUM = 4;
const double GRAVITY = 9.8;
const double param_A = 2;
const double param_B = 4;
// const double param_Kr = 0.1;

void mixer_test() {
    bool debugEnabled = true;
    double thrust = 108;
    double torque_x = 0;
    double torque_y = 0;
    double torque_z = 0;


    const double m = 10.0;
    const int N = ROTOR_NUM;
    const double g = GRAVITY;
    const double a = param_A;
    const double b = param_B;
    const double T0 = m*g; // equibilium
    const double omega0 = std::sqrt(T0/(N*a)); // equibilium
    const double delta_T = thrust - T0;
//    const double Kr = param_Kr;

    out("omega0=", omega0);

    glm::dmat4 M(1.0f);
    M[0] = glm::dvec4(1, 1, 1, 1);
    M[1] = glm::dvec4(-1, 1, 1, -1);
    M[2] = glm::dvec4(1, -1, 1, -1);
    M[3] = glm::dvec4(1, 1, -1, -1);
    M = glm::transpose(M);
    
    glm::dmat4 A(1.0f);
    A[0] = glm::dvec4( (a ), 0, 0, 0);
    A[1] = glm::dvec4( 0, (a ), 0, 0);
    A[2] = glm::dvec4( 0, 0, (a ), 0);
    A[3] = glm::dvec4( 0, 0, 0, (b ));
    A = glm::transpose(A);
    glm::dmat4 A_inv = glm::inverse(A);

    glm::dmat4 M_inv = glm::inverse(M);
    glm::dmat4 MA_inv= M_inv * A_inv;

    glm::dvec4 delta_U = glm::dvec4(delta_T, torque_x, torque_y, torque_z);
    glm::dvec4 delta_omega_times_omega0 = 0.5 * MA_inv * delta_U;
    glm::dvec4 delta_omega = delta_omega_times_omega0/omega0;

    // AFTER HOVER !
    for (int i = 0; i < ROTOR_NUM; i++) {
        double ratio = delta_omega[i]/omega0; 

        if (std::abs(ratio) >  0.8) {
            if (debugEnabled) {
                    std::cout << "WARNING: thrust:" << thrust << " tx: " << torque_x << " ty: " << torque_y << " tz: " << torque_z << std::endl;
                    std::cout << "WARNING: delta_omega("<< i << ") is too big(" << delta_omega[i]/omega0 << "%)...limited to 80%." << std::endl;
            }
            delta_omega[i] = delta_omega[i]/ratio  * 0.8;  // keep the sign and make omega0 * 0.8
        }
    }

    glm::dvec4 omega = omega0 + delta_omega;

    out("delta_U=", delta_U);
    out("delta_omega_times_omega0=", delta_omega_times_omega0);
    out("delta_omega=", delta_omega);
    out("omega=", omega);
}

int main() {
    simple_test();
    mixer_test();
    return 0;
}
