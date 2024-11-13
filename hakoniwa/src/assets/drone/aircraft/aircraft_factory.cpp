#include "aircraft_factory.hpp"
#include "utils/hako_utils.hpp"
#include "assets/drone/physics/body_frame_matlab/drone_dynamics_body_frame_matlab.hpp"
#include "assets/drone/physics/body_frame/drone_dynamics_body_frame.hpp"
#include "assets/drone/physics/body_frame_rk4/drone_dynamics_body_frame_rk4.hpp"
#include "assets/drone/physics/ground_frame/drone_dynamics_ground_frame.hpp"
#include "assets/drone/physics/rotor/rotor_dynamics.hpp"
#include "assets/drone/physics/battery/battery_dynamics.hpp"
#include "assets/drone/physics/rotor/rotor_dynamics_jmavsim.hpp"
#include "assets/drone/physics/thruster/thrust_dynamics_linear.hpp"
#include "assets/drone/physics/thruster/thrust_dynamics_nonlinear.hpp"
#include "assets/drone/sensors/acc/sensor_acceleration.hpp"
#include "assets/drone/sensors/baro/sensor_baro.hpp"
#include "assets/drone/sensors/gps/sensor_gps.hpp"
#include "assets/drone/sensors/gyro/sensor_gyro.hpp"
#include "assets/drone/sensors/mag/sensor_mag.hpp"
#include "assets/drone/aircraft/aricraft.hpp"
#include "assets/drone/utils/sensor_noise.hpp"
#include "config/drone_config.hpp"
#include <math.h>
#include "utils/hako_module_loader.hpp"
#include "hako_module_drone_sensor_gyro.h"

using hako::assets::drone::AirCraft;
using hako::assets::drone::DroneDynamicsBodyFrameMatlab;
using hako::assets::drone::DroneDynamicsBodyFrame;
using hako::assets::drone::DroneDynamicsGroundFrame;
using hako::assets::drone::SensorAcceleration;
using hako::assets::drone::SensorBaro;
using hako::assets::drone::SensorGps;
using hako::assets::drone::SensorMag;
using hako::assets::drone::SensorGyro;
using hako::assets::drone::RotorDynamics;
using hako::assets::drone::ThrustDynamicsLinear;
using hako::assets::drone::ThrustDynamicsNonLinear;
using hako::assets::drone::SensorNoise;

static inline double rpmToRadPerSec(double rpm) {
    return rpm * (2 * M_PI / 60.0);
}

#define DELTA_TIME_SEC              drone_config.getSimTimeStep()
#define REFERENCE_LATITUDE          drone_config.getSimLatitude()
#define REFERENCE_LONGTITUDE        drone_config.getSimLongitude()
#define REFERENCE_ALTITUDE          drone_config.getSimAltitude()
#define PARAMS_MAG_F   drone_config.getSimMagneticField().intensity_nT
#define PARAMS_MAG_D    DEGREE2RADIAN(drone_config.getSimMagneticField().declination_deg)
#define PARAMS_MAG_I    DEGREE2RADIAN(drone_config.getSimMagneticField().inclination_deg)

#define ACC_SAMPLE_NUM              drone_config.getCompSensorSampleCount("acc")
#define GYRO_SAMPLE_NUM             drone_config.getCompSensorSampleCount("gyro")
#define BARO_SAMPLE_NUM             drone_config.getCompSensorSampleCount("baro")
#define GPS_SAMPLE_NUM              drone_config.getCompSensorSampleCount("gps")
#define MAG_SAMPLE_NUM              drone_config.getCompSensorSampleCount("mag")

#define RAD_PER_SEC_MAX             rpmToRadPerSec(drone_config.getCompRotorRpmMax())
#define ROTOR_TAU                   drone_config.getCompRotorTr()
#define ROTOR_K                     rpmToRadPerSec(drone_config.getCompRotorKr())

#define THRUST_PARAM_B              drone_config.getCompThrusterParameter("parameterB")
#define THRUST_PARAM_JR             drone_config.getCompThrusterParameter("parameterJr")

#define LOGPATH(index, name)        drone_config.getSimLogFullPathFromIndex(index, name)

IAirCraft* hako::assets::drone::create_aircraft(int index, const DroneConfig& drone_config)
{

    auto drone = new AirCraft();
    HAKO_ASSERT(drone != nullptr);
    drone->set_name(drone_config.getRoboName());
    drone->set_index(index);
    
    //drone dynamics
    IDroneDynamics *drone_dynamics = nullptr;
    if (drone_config.getCompDroneDynamicsPhysicsEquation() == "BodyFrame") {
        std::cout << "DroneDynamicType: BodyFrame" << std::endl;
        drone_dynamics = new DroneDynamicsBodyFrame(DELTA_TIME_SEC);
    }
    else if (drone_config.getCompDroneDynamicsPhysicsEquation() == "BodyFrameMatlab") {
        std::cout << "DroneDynamicType: BodyFrameMatlab" << std::endl;
        drone_dynamics = new DroneDynamicsBodyFrameMatlab(DELTA_TIME_SEC);
    }
    else if (drone_config.getCompDroneDynamicsPhysicsEquation() == "BodyFrameRK4") {
        std::cout << "DroneDynamicType: BodyFrameRK4" << std::endl;
        drone_dynamics = new DroneDynamicsBodyFrameRK4(DELTA_TIME_SEC);
    }
    else {
        std::cout << "DroneDynamicType: GroundFrame" << std::endl;
        drone_dynamics = new DroneDynamicsGroundFrame(DELTA_TIME_SEC);
    }
    //auto drone_dynamics = new DroneDynamicsGroundFrame(DELTA_TIME_SEC);
    HAKO_ASSERT(drone_dynamics != nullptr);
    auto drags = drone_config.getCompDroneDynamicsAirFrictionCoefficient();
    drone_dynamics->set_drag(drags[0], drags[1]);
    drone_dynamics->set_mass(drone_config.getCompDroneDynamicsMass());
    drone_dynamics->set_collision_detection(drone_config.getCompDroneDynamicsCollisionDetection());
    if (drone_config.getCompDroneDynamicsEnableDisturbance()) {
        drone->enable_disturb();
    }
    drone_dynamics->set_manual_control(drone_config.getCompDroneDynamicsManualControl());
    auto body_size = drone_config.getCompDroneDynamicsBodySize();
    drone_dynamics->set_body_size(body_size[0], body_size[1], body_size[2]);
    auto inertia = drone_config.getCompDroneDynamicsInertia();
    drone_dynamics->set_torque_constants(inertia[0], inertia[1], inertia[2]);
    auto position = drone_config.getCompDroneDynamicsPosition();
    DronePositionType drone_pos;
    drone_pos.data = { position[0], position[1], position[2] }; 
    drone_dynamics->set_pos(drone_pos);
    auto angle = drone_config.getCompDroneDynamicsAngle();
    DroneEulerType rot;
    rot.data = { DEGREE2RADIAN(angle[0]), DEGREE2RADIAN(angle[1]), DEGREE2RADIAN(angle[2]) };
    drone_dynamics->set_angle(rot);
    //out of bounds reset option
    auto out_of_bounds_reset = drone_config.getCompDroneDynamicsOutOfBoundsReset();
    drone_dynamics->set_out_of_bounds_reset(out_of_bounds_reset);
    drone->set_drone_dynamics(drone_dynamics);
    std::cout << "INFO: logpath: " << LOGPATH(drone->get_index(), "drone_dynamics.csv") << std::endl;
    drone->get_logger().add_entry(*drone_dynamics, LOGPATH(drone->get_index(), "drone_dynamics.csv"));


    //battery dynamics
    BatteryModelParameters battery_config = drone_config.getComDroneDynamicsBattery();
    //TODO vendor support
    IBatteryDynamics *battery = nullptr;
    if (battery_config.vendor == "None") {
        battery = new BatteryDynamics(DELTA_TIME_SEC);
        HAKO_ASSERT(battery != nullptr);
        battery->set_params(battery_config);
        drone->set_battery_dynamics(battery);
        drone->get_logger().add_entry(*static_cast<BatteryDynamics*>(battery), LOGPATH(drone->get_index(), "log_battery.csv"));
    }
    else {
        std::cout << "INFO: battery is not enabled." << std::endl;
    }
    //rotor dynamics
    IRotorDynamics* rotors[hako::assets::drone::ROTOR_NUM];
    auto rotor_vendor = drone_config.getCompRotorVendor();
    std::cout<< "Rotor vendor: " << rotor_vendor << std::endl;
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        IRotorDynamics *rotor = nullptr;
        std::string logfilename= "log_rotor_" + std::to_string(i) + ".csv";
        if (rotor_vendor == "jmavsim") {
            rotor = new RotorDynamicsJmavsim(DELTA_TIME_SEC);
            HAKO_ASSERT(rotor != nullptr);
            static_cast<RotorDynamicsJmavsim*>(rotor)->set_params(RAD_PER_SEC_MAX, ROTOR_TAU, ROTOR_K);
            drone->get_logger().add_entry(*static_cast<RotorDynamicsJmavsim*>(rotor), LOGPATH(drone->get_index(), logfilename));
        }
        else if (rotor_vendor == "BatteryModel") {
            rotor = new RotorDynamics(DELTA_TIME_SEC);
            HAKO_ASSERT(rotor != nullptr);
            auto constants = drone_config.getCompDroneDynamicsRotorDynamicsConstants();
            rotor->set_battery_dynamics_constants(constants);
            static_cast<RotorDynamics*>(rotor)->set_params(RAD_PER_SEC_MAX, 0, ROTOR_K);
            drone->get_logger().add_entry(*static_cast<RotorDynamics*>(rotor), LOGPATH(drone->get_index(), logfilename));
            HAKO_ASSERT(battery != nullptr);
            battery->add_device(*static_cast<RotorDynamics*>(rotor));
        }
        else {
            rotor = new RotorDynamics(DELTA_TIME_SEC);
            HAKO_ASSERT(rotor != nullptr);
            static_cast<RotorDynamics*>(rotor)->set_params(RAD_PER_SEC_MAX, ROTOR_TAU, ROTOR_K);
            drone->get_logger().add_entry(*static_cast<RotorDynamics*>(rotor), LOGPATH(drone->get_index(), logfilename));
        }
        rotors[i] = rotor;
    }
    drone->set_rotor_dynamics(rotors);


    //thrust dynamics
    IThrustDynamics *thrust = nullptr;
    auto thrust_vendor = drone_config.getCompThrusterVendor();
    std::cout<< "Thruster vendor: " << thrust_vendor << std::endl;
    double param_A = 1.0;
    double param_B = THRUST_PARAM_B;
    if (thrust_vendor == "linear") {
        thrust = new ThrustDynamicsLinear(DELTA_TIME_SEC);
        HAKO_ASSERT(thrust != nullptr);
        double HoveringRpm = drone_config.getCompThrusterParameter("HoveringRpm");
        HAKO_ASSERT(HoveringRpm != 0);
        double mass = drone_dynamics->get_mass();
        param_A = (mass * GRAVITY / (rpmToRadPerSec(HoveringRpm) * ROTOR_NUM));
        param_B = drone_config.getCompThrusterParameter("parameterB_linear");
        static_cast<ThrustDynamicsLinear*>(thrust)->set_params(
            param_A,
            param_B
        );
        std::cout << "param_A_linear: " << param_A << std::endl;
        std::cout << "param_B_linear: " << param_B << std::endl;
        drone->get_logger().add_entry(*static_cast<ThrustDynamicsLinear*>(thrust), LOGPATH(drone->get_index(), "log_thrust.csv"));
    }
    else {
        thrust = new ThrustDynamicsNonLinear(DELTA_TIME_SEC);
        HAKO_ASSERT(thrust != nullptr);
        double HoveringRpm = drone_config.getCompThrusterParameter("HoveringRpm");
        HAKO_ASSERT(HoveringRpm != 0);
        double mass = drone_dynamics->get_mass();
        param_A = ( 
                            mass * GRAVITY / 
                            (
                                pow(rpmToRadPerSec(HoveringRpm), 2) * ROTOR_NUM
                            )
                        );
        std::cout << "param_A: " << param_A << std::endl;
        std::cout << "param_B: " << THRUST_PARAM_B << std::endl;
        std::cout << "param_Jr: " << THRUST_PARAM_JR << std::endl;
        static_cast<ThrustDynamicsNonLinear*>(thrust)->set_params(param_A, THRUST_PARAM_B, THRUST_PARAM_JR);
        drone->get_logger().add_entry(*static_cast<ThrustDynamicsNonLinear*>(thrust), LOGPATH(drone->get_index(), "log_thrust.csv"));
    }
    drone->set_thrus_dynamics(thrust);

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

    // mixer
    DroneConfig::MixerInfo mixer_info;
    if (drone_config.getControllerMixerInfo(mixer_info)) {
        std::cout << "INFO: mixer is enabled" << std::endl;
        DroneMixer *mixer = new DroneMixer(ROTOR_K, param_A, param_B, rotor_config);
        HAKO_ASSERT(mixer != nullptr);
        bool inv_m = mixer->calculate_M_inv();
        HAKO_ASSERT(inv_m == true);
        mixer->setMixerInfo(mixer_info);
        drone->set_mixer(mixer);
    }
    else {
        std::cout << "INFO: mixer is not enabled" << std::endl;
    }
    // rotor control
    if (drone_config.getControllerDirectRotorControl()) {
        drone->set_rotor_control_enabled();
    }

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
    drone->get_logger().add_entry(*acc, LOGPATH(drone->get_index(), "log_acc.csv"));

    //sensor gyro
    auto gyro = new SensorGyro(DELTA_TIME_SEC, ACC_SAMPLE_NUM);
    HAKO_ASSERT(gyro != nullptr);
    variance = drone_config.getCompSensorNoise("gyro");
    if (variance > 0) {
        auto noise = new SensorNoise(variance);
        HAKO_ASSERT(noise != nullptr);
        gyro->set_noise(noise);
    }
    auto module_path = drone_config.getCompSensorVendor("gyro");
    if (!module_path.empty()) {
        std::cout << "INFO: now loading gyro vendor model " << module_path << std::endl;
        void *handle;
        HakoModuleHeaderType *header = nullptr;
        HakoModuleDroneSensorGyroType *gyro_model = nullptr;
        handle = hako_module_handle(module_path.c_str(), &header);
        HAKO_ASSERT(handle != nullptr);
        gyro_model = (HakoModuleDroneSensorGyroType*)hako_module_load_symbol(handle, HAKO_MODULE_DRONE_SENSOR_GYRO_SYMBOLE_NAME);
        HAKO_ASSERT(gyro_model != nullptr);
        //std::cout << "gyro_model addr: " << gyro_model << std::endl;
        std::cout << "SUCCESS: Loaded module name: " << header->get_name() << std::endl;
        std::string filepath = drone_config.getCompSensorContext("gyro", "file");
        std::cout << "filepath:" << filepath <<std::endl;
        mi_drone_sensor_gyro_context_t* context = new mi_drone_sensor_gyro_context_t();
        HAKO_ASSERT(context != nullptr);
        if (filepath.empty()) {
            context->filepath = nullptr;
        }
        else {
            context->filepath = new char[filepath.size() + 1];
            std::strcpy(context->filepath, filepath.c_str());
        }
        std::cout << "gyro_model: index = " << index << std::endl;
        context->index = index;
        auto v = gyro_model->init(context);
        HAKO_ASSERT(v == 0);
        gyro->set_vendor(gyro_model, (void*)context);
    }
    drone->set_gyro(gyro);
    drone->get_logger().add_entry(*gyro, LOGPATH(drone->get_index(), "log_gyro.csv"));

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
    drone->get_logger().add_entry(*mag, LOGPATH(drone->get_index(), "log_mag.csv"));

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
    drone->get_logger().add_entry(*baro, LOGPATH(drone->get_index(), "log_baro.csv"));

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
    drone->get_logger().add_entry(*gps, LOGPATH(drone->get_index(), "log_gps.csv"));

    return drone;
}