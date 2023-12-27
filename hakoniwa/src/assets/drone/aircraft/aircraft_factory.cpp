#include "aircraft_factory.hpp"
#include "utils/hako_utils.hpp"
#include "assets/drone/physics/body_frame/drone_dynamics_body_frame.hpp"
#include "assets/drone/physics/body_frame_rk4/drone_dynamics_body_frame_rk4.hpp"
#include "assets/drone/physics/ground_frame/drone_dynamics_ground_frame.hpp"
#include "assets/drone/physics/rotor_dynamics.hpp"
#include "assets/drone/physics/thrust_dynamics.hpp"
#include "assets/drone/sensors/acc/sensor_acceleration.hpp"
#include "assets/drone/sensors/baro/sensor_baro.hpp"
#include "assets/drone/sensors/gps/sensor_gps.hpp"
#include "assets/drone/sensors/gyro/sensor_gyro.hpp"
#include "assets/drone/sensors/mag/sensor_mag.hpp"
#include "assets/drone/aircraft/aricraft.hpp"
#include "assets/drone/utils/sensor_noise.hpp"
#include "config/drone_config.hpp"
#include <math.h>

using hako::assets::drone::AirCraft;
using hako::assets::drone::DroneDynamicsBodyFrame;
using hako::assets::drone::DroneDynamicsGroundFrame;
using hako::assets::drone::SensorAcceleration;
using hako::assets::drone::SensorBaro;
using hako::assets::drone::SensorGps;
using hako::assets::drone::SensorMag;
using hako::assets::drone::SensorGyro;
using hako::assets::drone::RotorDynamics;
using hako::assets::drone::ThrustDynamics;
using hako::assets::drone::SensorNoise;

#define DELTA_TIME_SEC              drone_config.getSimTimeStep()
#define HAKO_PHYS_DRAG              drone_config.getCompDroneDynamicsAirFrictionCoefficient()
#define REFERENCE_LATITUDE          drone_config.getSimLatitude()
#define REFERENCE_LONGTITUDE        drone_config.getSimLongitude()
#define REFERENCE_ALTITUDE          drone_config.getSimAltitude()
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define PARAMS_MAG_F   drone_config.getSimMagneticField().intensity_nT
#define PARAMS_MAG_D    DEGREE2RADIAN(drone_config.getSimMagneticField().declination_deg)
#define PARAMS_MAG_I    DEGREE2RADIAN(drone_config.getSimMagneticField().inclination_deg)

#define ACC_SAMPLE_NUM              drone_config.getCompSensorSampleCount("acc")
#define GYRO_SAMPLE_NUM             drone_config.getCompSensorSampleCount("gyro")
#define BARO_SAMPLE_NUM             drone_config.getCompSensorSampleCount("baro")
#define GPS_SAMPLE_NUM              drone_config.getCompSensorSampleCount("gps")
#define MAG_SAMPLE_NUM              drone_config.getCompSensorSampleCount("mag")

#define RPM_MAX                     drone_config.getCompRotorRpmMax()
#define ROTOR_TAU                   drone_config.getCompRotorTr()
#define ROTOR_K                     drone_config.getCompRotorKr()

#define THRUST_PARAM_B              drone_config.getCompThrusterParameter("parameterB")
#define THRUST_PARAM_JR             drone_config.getCompThrusterParameter("parameterJr")

#define LOGPATH(name)               drone_config.getSimLogFullPath(name)

IAirCraft* hako::assets::drone::create_aircraft(const char* drone_type)
{
    (void)drone_type;

    auto drone = new AirCraft();
    HAKO_ASSERT(drone != nullptr);

    //drone dynamics
    IDroneDynamics *drone_dynamics = nullptr;
    if (drone_config.getCompDroneDynamicsPhysicsEquation() == "BodyFrame") {
        drone_dynamics = new DroneDynamicsBodyFrame(DELTA_TIME_SEC);
    }
    else if (drone_config.getCompDroneDynamicsPhysicsEquation() == "BodyFrameRK4") {
        drone_dynamics = new DroneDynamicsBodyFrameRK4(DELTA_TIME_SEC);
    }
    else {
        drone_dynamics = new DroneDynamicsGroundFrame(DELTA_TIME_SEC);
    }
    //auto drone_dynamics = new DroneDynamicsGroundFrame(DELTA_TIME_SEC);
    HAKO_ASSERT(drone_dynamics != nullptr);
    drone_dynamics->set_drag(HAKO_PHYS_DRAG);
    drone_dynamics->set_mass(drone_config.getCompDroneDynamicsMass());
    auto inertia = drone_config.getCompDroneDynamicsInertia();
    drone_dynamics->set_torque_constants(inertia[0], inertia[1], inertia[2]);
    auto position = drone_config.getCompDroneDynamicsPosition();
    DronePositionType drone_pos;
    drone_pos.data = { position[0], position[1], position[2] }; 
    drone_dynamics->set_pos(drone_pos);
    auto angle = drone_config.getCompDroneDynamicsAngle();
    DroneAngleType rot;
    rot.data = { DEGREE2RADIAN(angle[0]), DEGREE2RADIAN(angle[1]), DEGREE2RADIAN(angle[2]) };
    drone_dynamics->set_angle(rot);
    drone->set_drone_dynamics(drone_dynamics);
    drone->get_logger().add_entry(*drone_dynamics, LOGPATH("drone_dynamics.csv"));

    //rotor dynamics
    IRotorDynamics* rotors[hako::assets::drone::ROTOR_NUM];
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        auto rotor = new RotorDynamics(DELTA_TIME_SEC);
        HAKO_ASSERT(rotor != nullptr);
        rotors[i] = rotor;
        rotor->set_params(RPM_MAX, ROTOR_TAU, ROTOR_K);
        std::string logfilename= "log_rotor_" + std::to_string(i) + ".csv";
        drone->get_logger().add_entry(*rotor, LOGPATH(logfilename));
    }
    drone->set_rotor_dynamics(rotors);

    //thrust dynamics
    auto thrust = new ThrustDynamics(DELTA_TIME_SEC);
    HAKO_ASSERT(thrust != nullptr);
    drone->set_thrus_dynamics(thrust);
    double mass = drone_dynamics->get_mass();
    double param_A = ( 
                        mass * GRAVITY / 
                        (
                            pow(drone_config.getCompThrusterParameter("HoveringRpm"), 2) * ROTOR_NUM
                        )
                    );
    std::cout << "param_A: " << param_A << std::endl;
    std::cout << "param_B: " << THRUST_PARAM_B << std::endl;
    std::cout << "param_Jr: " << THRUST_PARAM_JR << std::endl;
    thrust->set_params(param_A, THRUST_PARAM_B, THRUST_PARAM_JR);

    RotorConfigType rotor_config[ROTOR_NUM];
    std::vector<RotorPosition> pos = drone_config.getCompThrusterRotorPositions();
    HAKO_ASSERT(pos.size() == ROTOR_NUM);
    for (size_t i = 0; i < pos.size(); ++i) {
        rotor_config[i].ccw = pos[i].rotationDirection;
        rotor_config[i].data.x = pos[i].position[0];
        rotor_config[i].data.y = pos[i].position[1];
        rotor_config[i].data.z = pos[i].position[2];
    }    

    thrust->set_rotor_config(rotor_config);
    drone->get_logger().add_entry(*thrust, LOGPATH("log_thrust.csv"));

    //sensor acc
    auto acc = new SensorAcceleration(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(acc != nullptr);
    double variance = drone_config.getCompSensorNoise("acc");
    if (variance > 0) {
        auto noise = new SensorNoise(variance);
        HAKO_ASSERT(noise != nullptr);
        acc->set_noise(noise);
    }
    drone->set_acc(acc);
    drone->get_logger().add_entry(*acc, LOGPATH("log_acc.csv"));

    //sensor gyro
    auto gyro = new SensorGyro(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(gyro != nullptr);
    variance = drone_config.getCompSensorNoise("gyro");
    if (variance > 0) {
        auto noise = new SensorNoise(variance);
        HAKO_ASSERT(noise != nullptr);
        gyro->set_noise(noise);
    }
    drone->set_gyro(gyro);
    drone->get_logger().add_entry(*gyro, LOGPATH("log_gyro.csv"));

    //sensor mag
    auto mag = new SensorMag(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(mag != nullptr);
    variance = drone_config.getCompSensorNoise("mag");
    if (variance > 0) {
        auto noise = new SensorNoise(variance);
        HAKO_ASSERT(noise != nullptr);
        mag->set_noise(noise);
    }
    mag->set_params(PARAMS_MAG_F, PARAMS_MAG_I, PARAMS_MAG_D);
    drone->set_mag(mag);
    drone->get_logger().add_entry(*mag, LOGPATH("log_mag.csv"));

    //sensor baro
    auto baro = new SensorBaro(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(baro != nullptr);
    baro->init_pos(REFERENCE_LATITUDE, REFERENCE_LONGTITUDE, REFERENCE_ALTITUDE);
    variance = drone_config.getCompSensorNoise("baro");
    if (variance > 0) {
        auto noise = new SensorNoise(variance);
        HAKO_ASSERT(noise != nullptr);
        baro->set_noise(noise);
    }
    drone->set_baro(baro);
    drone->get_logger().add_entry(*baro, LOGPATH("log_baro.csv"));

    //sensor gps
    auto gps = new SensorGps(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(gps != nullptr);
    variance = drone_config.getCompSensorNoise("gps");
    if (variance > 0) {
        auto noise = new SensorNoise(variance);
        HAKO_ASSERT(noise != nullptr);
        gps->set_noise(noise);
    }
    gps->init_pos(REFERENCE_LATITUDE, REFERENCE_LONGTITUDE, REFERENCE_ALTITUDE);
    drone->set_gps(gps);
    drone->get_logger().add_entry(*gps, LOGPATH("log_gps.csv"));

    return drone;
}