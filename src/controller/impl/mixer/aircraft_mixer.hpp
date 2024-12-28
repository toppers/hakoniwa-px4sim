#ifndef _DRONE_MIXER_HPP_
#define _DRONE_MIXER_HPP_

#include "config/drone_config.hpp"
#include "controller/iaircraft_mixer.hpp"
#include "aircraft/interfaces/primitive_types.hpp"
#include "aircraft/interfaces/ithrust_dynamics.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <cmath>

using namespace hako::config;
using namespace hako::aircraft;

namespace hako::controller {
    class AircraftMixer : public IAircraftMixer{
    private:
        double param_Kr;
        double param_A;
        double param_B;
        glm::dmat4 M;
        glm::dmat4 M_inv;
        glm::dmat4 A;
        glm::dmat4 A_inv;
        glm::dmat4 MA_inv;
        DroneConfig::MixerInfo mixer_info {};

        double get_duty(double omega) {
            //e = K * w + (Cq * R / K ) w ^2
            double e = mixer_info.K * omega + (mixer_info.Cq * mixer_info.R / mixer_info.K) * omega * omega;
            //d = e / V_bat
            double d = e / mixer_info.V_bat;
            return d;
        }

        PwmDuty run_default(double mass, double thrust, double torque_x, double torque_y, double torque_z)
        {
            (void)mass;
            glm::dvec4 U = glm::dvec4(thrust, torque_x, torque_y, torque_z);
            
            glm::dvec4 Omega2 = MA_inv * U;
            PwmDuty duty = {};
            for (int i = 0; i < ROTOR_NUM; i++) {
                if (Omega2[i] < 0) {
                    if (mixer_info.enableErrorLog) {
                        std::cout << "ERROR: thrust:" << thrust << " tx: " << torque_x << " ty: " << torque_y << " tz: " << torque_z << std::endl;
                        std::cout << "ERROR: Invalid caluculation of Omega to duty because of omega^2("<< i << ") is minus value(" << Omega2[i] << ")..." << std::endl;
                    }
                    Omega2[i] = 0;
                }
                else {
                    //duty.d[i] = std::sqrt(Omega2[i]) / this->param_Kr;
                    duty.d[i] = get_duty(std::sqrt(Omega2[i]));
                }
                if (mixer_info.enableDebugLog) {
                    std::cout << "Motor " << i << ": Omega^2 = " << Omega2[i] << ", PWM Duty = " << duty.d[i] << std::endl;
                }                
            }
            return duty;
        }
        // linearized version
        PwmDuty run_linear(double mass, double thrust, double torque_x, double torque_y, double torque_z)
        {
            const double m = mass; // pass through mass from caller
            const int N = ROTOR_NUM;
            const double g = GRAVITY;
            const double A = param_A;
            const double T0 = m*g; // equibilium
            const double omega0 = std::sqrt(T0/(N*A)); // equibilium
            const double delta_T = thrust - T0;
            const double Kr = param_Kr;
            
            glm::dvec4 delta_U = glm::dvec4(delta_T, torque_x, torque_y, torque_z);
            glm::dvec4 delta_omega_times_omega0 = 0.5 * MA_inv * delta_U;
            glm::dvec4 delta_omega = delta_omega_times_omega0/omega0;
            glm::dvec4 omega = omega0 + delta_omega;

            PwmDuty duty = {};
            for (int i = 0; i < ROTOR_NUM; i++) {
                if (omega[i] < 0) {
                    if (mixer_info.enableErrorLog) {
                        std::cout << "ERROR: thrust:" << thrust << " tx: " << torque_x << " ty: " << torque_y << " tz: " << torque_z << std::endl;
                        std::cout << "ERROR: Invalid caluculation of Omega to duty because of omega("<< i << ") is minus value(" << omega[i] << ")..." << std::endl;
                    }
                    omega[i] = 0.0;
                }
                else {
                    duty.d[i] = omega[i] / Kr;
                }
                if (mixer_info.enableDebugLog) {
                    std::cout << "Motor " << i << ": Omega = " << omega[i] << ", PWM Duty = " << duty.d[i] << std::endl;
                }                
            }
            return duty;
        }
        PwmDuty run(double mass, double thrust, double torque_x, double torque_y, double torque_z)
        {
            PwmDuty duty = {};
            if (mixer_info.enable) {
                if (mixer_info.vendor == "linear") {
                    return run_linear(mass, thrust, torque_x, torque_y, torque_z);
                }
                else {
                    return run_default(mass, thrust, torque_x, torque_y, torque_z);
                }
            }
            return duty;
        }

    public: 
        virtual ~AircraftMixer() {}
        AircraftMixer(double Kr, double a, double b, RotorConfigType rotor[ROTOR_NUM])
        {
            this->param_Kr = Kr;
            this->param_A = a;
            this->param_B = b;
            this->M = glm::dmat4(1.0f);
            this->M[0] = glm::dvec4(1, 1, 1, 1);
            this->M[1] = glm::dvec4(-rotor[0].data.y, -rotor[1].data.y, -rotor[2].data.y, -rotor[3].data.y);
            this->M[2] = glm::dvec4(rotor[0].data.x, rotor[1].data.x, rotor[2].data.x, rotor[3].data.x);
            this->M[3] = glm::dvec4(rotor[0].ccw, rotor[1].ccw, rotor[2].ccw, rotor[3].ccw);
            this->M = glm::transpose(this->M);
            
            this->A = glm::dmat4(1.0f);
            this->A[0] = glm::dvec4( (a ), 0, 0, 0);
            this->A[1] = glm::dvec4( 0, (a ), 0, 0);
            this->A[2] = glm::dvec4( 0, 0, (a ), 0);
            this->A[3] = glm::dvec4( 0, 0, 0, (b ));
            this->A = glm::transpose(this->A);
            this->A_inv = glm::inverse(A);
        }
        bool calculate_M_inv()
        {
            auto det = glm::determinant(this->M);
            if (std::abs(det) < 0.0001) {
                std::cout << "The matrix is singular and does not have an inverse." << std::endl;
                return false;
            }
            this->M_inv = glm::inverse(M);
            this->MA_inv= M_inv * A_inv;
            return true;
        }        
        void setMixerInfo(DroneConfig::MixerInfo& info)
        {
            mixer_info = info;
        }
        static void printMatrix(const glm::dmat4& mat) {
            for (int i = 0; i < 4; i++) { 
                for (int j = 0; j < 4; j++) {
                    std::cout << mat[j][i] << " ";
                }
                std::cout << std::endl;
            }
        }
        PwmDuty run(mi_aircraft_control_out_t& in) override
        {
            return run(in.mass, in.thrust, in.torque_x, in.torque_y, in.torque_z);
        }
        glm::vec4 reconstructControlInput(const PwmDuty& duty) {
            glm::dvec4 Omega2;
            for (int i = 0; i < ROTOR_NUM; i++) {
                Omega2[i] = std::pow(duty.d[i] * param_Kr, 2);
            }
            return A * M * Omega2;
        }
        bool testReconstruction(double thrust, double torque_x, double torque_y, double torque_z) {
            glm::dvec4 U_original = glm::dvec4(thrust, torque_x, torque_y, torque_z);
            PwmDuty duty = run_default(0, thrust, torque_x, torque_y, torque_z);
            glm::dvec4 U_reconstructed = reconstructControlInput(duty);

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
