#include "aircraft_factory.hpp"
#include "utils/hako_utils.hpp"
#include "assets/drone/physics/body_frame/drone_dynamics_body_frame.hpp"
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

#define DELTA_TIME_SEC              0.001
#define HAKO_PHYS_DRAG              0.05
#define REFERENCE_LATITUDE      47.641468
#define REFERENCE_LONGTITUDE    -122.140165
#define REFERENCE_ALTITUDE      121.321
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define PARAMS_MAG_F   53045.1
#define PARAMS_MAG_H   19023.3
#define PARAMS_MAG_Z   49516.6
#define PARAMS_MAG_D    DEGREE2RADIAN(15.306)
#define PARAMS_MAG_I    DEGREE2RADIAN(68.984)

#define ACC_SAMPLE_NUM              2
#define GYRO_SAMPLE_NUM             2
#define BARO_SAMPLE_NUM             2
#define GPS_SAMPLE_NUM              2
#define MAG_SAMPLE_NUM              2

#define RPM_MAX                     4000

IAirCraft* hako::assets::drone::create_aircraft(const char* drone_type)
{
    (void)drone_type;

    auto drone = new AirCraft();
    HAKO_ASSERT(drone != nullptr);

    //drone dynamics
    auto drone_dynamics = new DroneDynamicsBodyFrame(DELTA_TIME_SEC);
    HAKO_ASSERT(drone_dynamics != nullptr);
    drone_dynamics->set_drag(HAKO_PHYS_DRAG);
    drone->set_drone_dynamics(drone_dynamics);

    //rotor dynamics
    IRotorDynamics* rotors[hako::assets::drone::ROTOR_NUM];
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        auto rotor = new RotorDynamics(DELTA_TIME_SEC);
        HAKO_ASSERT(rotor != nullptr);
        rotors[i] = rotor;
        rotor->set_params(RPM_MAX, 1.0, 1.0);
        std::string logfilename= "./log_rotar_" + std::to_string(i) + ".csv";
        drone->get_logger().add_entry(*rotor, logfilename);
    }
    drone->set_rotor_dynamics(rotors);

    //thrust dynamics
    auto thrust = new ThrustDynamics(DELTA_TIME_SEC);
    HAKO_ASSERT(thrust != nullptr);
    drone->set_thrus_dynamics(thrust);
    double param_A =  32 * GRAVITY / (ROTOR_NUM * HOVERING_ROTOR_RPM * HOVERING_ROTOR_RPM);
    double param_B =  0.000001 / (ROTOR_NUM * HOVERING_ROTOR_RPM * HOVERING_ROTOR_RPM);
    double param_Jr = 0.000001;
    thrust->set_params(param_A, param_B, param_Jr);

    RotorConfigType rotor_config[ROTOR_NUM];
    rotor_config[0].ccw = 0.05;
    rotor_config[0].data = { 0.1515, 0.245, 0 };
    rotor_config[1].ccw = 0.05;
    rotor_config[1].data = { -0.1515, -0.1875, 0 };
    rotor_config[2].ccw = -0.05;
    rotor_config[2].data = { 0.1515, -0.245, 0 };
    rotor_config[3].ccw = -0.05;
    rotor_config[3].data = { -0.1515, 0.1875, 0 };

    thrust->set_rotor_config(rotor_config);
    drone->get_logger().add_entry(*thrust, "./log_thrust.csv");

    //sensor acc
    auto acc = new SensorAcceleration(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(acc != nullptr);
    drone->set_acc(acc);
    drone->get_logger().add_entry(*acc, "./log_acc.csv");

    //sensor gyro
    auto gyro = new SensorGyro(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(gyro != nullptr);
    drone->set_gyro(gyro);
    drone->get_logger().add_entry(*gyro, "./log_gyro.csv");

    //sensor mag
    auto mag = new SensorMag(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(mag != nullptr);
    auto mag_noise = new SensorNoise(0.0001);
    HAKO_ASSERT(mag_noise != nullptr);
    mag->set_noise(mag_noise);
    mag->set_params(PARAMS_MAG_F, PARAMS_MAG_I, PARAMS_MAG_D);
    drone->set_mag(mag);
    drone->get_logger().add_entry(*mag, "./log_mag.csv");

    //sensor baro
    auto baro = new SensorBaro(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    baro->init_pos(REFERENCE_LATITUDE, REFERENCE_LONGTITUDE, REFERENCE_ALTITUDE);
    HAKO_ASSERT(baro != nullptr);
    drone->set_baro(baro);
    drone->get_logger().add_entry(*baro, "./log_baro.csv");

    //sensor gps
    auto gps = new SensorGps(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(gps != nullptr);
    gps->init_pos(REFERENCE_LATITUDE, REFERENCE_LONGTITUDE, REFERENCE_ALTITUDE);
    drone->set_gps(gps);
    drone->get_logger().add_entry(*gps, "./log_gps.csv");

    return drone;
}