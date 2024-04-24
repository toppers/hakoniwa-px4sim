#ifndef _DRONE_MIXER_HPP_
#define _DRONE_MIXER_HPP_
#include "drone_primitive_types.hpp"
#include "ithrust_dynamics.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <cmath>

namespace hako::assets::drone {
    struct PwmDuty {
        double d[ROTOR_NUM];
    };
    class DroneMixer {
    private:
        double param_Kr;
        double param_A;
        double param_B;
        glm::mat4 M;
        glm::mat4 M_inv;
        glm::mat4 A;
        glm::mat4 A_inv;
        glm::mat4 MA_inv;
        bool debugEnabled = false;

    public:
        virtual ~DroneMixer() {}
        DroneMixer(double Kr, double a, double b, RotorConfigType rotor[ROTOR_NUM])
        {
            this->param_Kr = Kr;
            this->param_A = a;
            this->param_B = b;
            this->M = glm::mat4(1.0f);
            this->M[0] = glm::vec4(1, 1, 1, 1);
            //this->M[1] = glm::vec4(rotor[0].data.y, rotor[1].data.y, rotor[2].data.y, rotor[3].data.y);
            //this->M[2] = glm::vec4(-rotor[0].data.x, -rotor[1].data.x, -rotor[2].data.x, -rotor[3].data.x);
            this->M[1] = glm::vec4(-rotor[0].data.y, -rotor[1].data.y, -rotor[2].data.y, -rotor[3].data.y);
            this->M[2] = glm::vec4(rotor[0].data.x, rotor[1].data.x, rotor[2].data.x, rotor[3].data.x);
            this->M[3] = glm::vec4(rotor[0].ccw, rotor[1].ccw, rotor[2].ccw, rotor[3].ccw);
            this->M = glm::transpose(this->M);
            
            this->A = glm::mat4(1.0f);
            this->A[0] = glm::vec4( (a ), 0, 0, 0);
            this->A[1] = glm::vec4( 0, (a ), 0, 0);
            this->A[2] = glm::vec4( 0, 0, (a ), 0);
            this->A[3] = glm::vec4( 0, 0, 0, (b ));
            this->A = glm::transpose(this->A);
            this->A_inv = glm::inverse(A);
        }
        bool calculate_M_inv()
        {
            float det = glm::determinant(this->M);
            if (std::abs(det) < 0.0001) {
                std::cout << "The matrix is singular and does not have an inverse." << std::endl;
                return false;
            }
            this->M_inv = glm::inverse(M);
            this->MA_inv= M_inv * A_inv;
            return true;
        }
        PwmDuty run(double thrust, double torque_x, double torque_y, double torque_z)
        {
            glm::vec4 U = glm::vec4(thrust, torque_x, torque_y, torque_z);
            
            glm::vec4 Omega2 = M_inv * A_inv * U;
            PwmDuty duty = {};
            for (int i = 0; i < ROTOR_NUM; i++) {
                if (Omega2[i] < 0) {
                    std::cout << "ERROR: thrust:" << thrust << " tx: " << torque_x << " ty: " << torque_y << " tz: " << torque_z << std::endl;
                    std::cout << "ERROR: Invalid caluculation of Omega to duty because of omega^2("<< i << ") is minus value(" << Omega2[i] << ")..." << std::endl;
                    Omega2[i] = 0;
                }
                else {
                    duty.d[i] = std::sqrt(Omega2[i]) / this->param_Kr;
                }
                if (debugEnabled) {
                    std::cout << "Motor " << i << ": Omega^2 = " << Omega2[i] << ", PWM Duty = " << duty.d[i] << std::endl;
                }                
            }
            return duty;
        }
        glm::vec4 reconstructControlInput(const PwmDuty& duty) {
            glm::vec4 Omega2;
            for (int i = 0; i < ROTOR_NUM; i++) {
                Omega2[i] = std::pow(duty.d[i] * param_Kr, 2);
            }
            return A * M * Omega2;
        }
        bool testReconstruction(double thrust, double torque_x, double torque_y, double torque_z) {
            glm::vec4 U_original = glm::vec4(thrust, torque_x, torque_y, torque_z);
            PwmDuty duty = run(thrust, torque_x, torque_y, torque_z);
            glm::vec4 U_reconstructed = reconstructControlInput(duty);

            const double epsilon = 1e-5;
            for (int i = 0; i < 4; i++) {
                if (std::abs(U_original[i] - U_reconstructed[i]) > epsilon) {
                    return false;
                }
            }
            return true;
        }
    };
}

#endif /* _DRONE_MIXER_HPP_ */
