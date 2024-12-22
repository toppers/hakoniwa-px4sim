#include "aircraft/impl/aircraft.hpp"
#include "aircraft/impl/aircraft_factory.hpp"
#include "aircraft/impl/body/drone_dynamics_body_frame.hpp"
#include "aircraft/impl/thruster/thrust_dynamics_nonlinear.hpp"
#include "aircraft/impl/rotor/rotor_dynamics.hpp"
#include "aircraft/impl/sensors/sensor_acceleration.hpp"
#include "aircraft/impl/sensors/sensor_baro.hpp"
#include "aircraft/impl/sensors/sensor_gps.hpp"
#include "aircraft/impl/sensors/sensor_gyro.hpp"
#include "aircraft/impl/sensors/sensor_mag.hpp"
#include "aircraft/impl/noise/sensor_data_assembler.hpp"
#include "aircraft/impl/noise/sensor_noise.hpp"
#include "config/drone_config.hpp"
#include <math.h>
#include "logger/impl/csv_log_file.hpp"

using namespace hako::aircraft;

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

#define THRUST_PARAM_Ct              drone_config.getCompThrusterParameter("Ct")

#define LOGPATH(index, name)        drone_config.getSimLogFullPathFromIndex(index, name)

#define HAKO_ASSERT(cond)           if (!(cond)) { throw std::runtime_error("assertion failed: " #cond); }

static inline std::unique_ptr<ILogFile> create_logfile(const std::string& path, ILog& entry) {
    return std::make_unique<CsvLogFile>(path, entry.log_head());
}


IAirCraft* hako::aircraft::create_aircraft(int index, const DroneConfig& drone_config)
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
    else {
        throw std::runtime_error("unsupported drone dynamics type");
    }
    //auto drone_dynamics = new DroneDynamicsGroundFrame(DELTA_TIME_SEC);
    HAKO_ASSERT(drone_dynamics != nullptr);
    drone_dynamics->set_use_quaternion(drone_config.getCompDroneDynamicsUseQuaternion());
    std::cout << "INFO: use_quaternion: " << drone_config.getCompDroneDynamicsUseQuaternion() << std::endl;
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
    drone->get_logger().add_entry(*drone_dynamics, 
        create_logfile(LOGPATH(drone->get_index(), "drone_dynamics.csv"), *drone_dynamics));

    //battery dynamics
    BatteryModelParameters battery_config = drone_config.getComDroneDynamicsBattery();
    if (battery_config.vendor == "None") {
        //not supported
    }
    else {
        std::cout << "INFO: battery is not enabled." << std::endl;
    }
    // calculate hovering rpm and maxrpm
    double HoveringRadPerSec = sqrt(drone_dynamics->get_mass() * GRAVITY / (drone_config.getCompThrusterParameter("Ct") * ROTOR_NUM));
    double RadPerSecMax = HoveringRadPerSec * 2.0;
    double HoveringRpm = HoveringRadPerSec * 60.0 / (2 * M_PI);
    HAKO_ASSERT(HoveringRpm != 0.0);
    std::cout << "HoveringRadPerSec: " << HoveringRadPerSec << std::endl;
    std::cout << "HoveringRpm: " << HoveringRpm << std::endl;
    //Tr=J*Rm/(Dm*Rm+K*K+2*Rm*Cq*ω_0)
    auto rotor_constants = drone_config.getCompDroneDynamicsRotorDynamicsConstants();
    double RotorTau = (
                        (rotor_constants.J * rotor_constants.R) / 
                        ( 
                            (rotor_constants.D * rotor_constants.R) 
                          + (rotor_constants.K * rotor_constants.K)
                          + (2 * rotor_constants.R * rotor_constants.Cq * HoveringRadPerSec)
                        )
                    );
    std::cout << "RotorTau: " << RotorTau << std::endl;
    //rotor dynamics
    IRotorDynamics* rotors[hako::aircraft::ROTOR_NUM];
    auto rotor_vendor = drone_config.getCompRotorVendor();
    std::cout<< "Rotor vendor: " << rotor_vendor << std::endl;
    for (int i = 0; i < hako::aircraft::ROTOR_NUM; i++) {
        IRotorDynamics *rotor = nullptr;
        std::string logfilename= "log_rotor_" + std::to_string(i) + ".csv";
        {
            rotor = new RotorDynamics(DELTA_TIME_SEC);
            HAKO_ASSERT(rotor != nullptr);
            static_cast<RotorDynamics*>(rotor)->set_params(RadPerSecMax, RotorTau, RadPerSecMax);
            drone->get_logger().add_entry(*static_cast<RotorDynamics*>(rotor), 
                create_logfile(LOGPATH(drone->get_index(), logfilename), *static_cast<RotorDynamics*>(rotor)));
        }
        rotors[i] = rotor;
    }
    drone->set_rotor_dynamics(rotors);


    //thrust dynamics
    IThrustDynamics *thrust = nullptr;
    auto thrust_vendor = drone_config.getCompThrusterVendor();
    std::cout<< "Thruster vendor: " << thrust_vendor << std::endl;
    double param_Ct = THRUST_PARAM_Ct;
    double param_Cq = rotor_constants.Cq;
    std::cout << "param_Ct: " << param_Ct << std::endl;
    std::cout << "param_Cq: " << param_Cq << std::endl;
    {
        thrust = new ThrustDynamicsNonLinear(DELTA_TIME_SEC);
        HAKO_ASSERT(thrust != nullptr);
        std::cout << "param_J: " << rotor_constants.J << std::endl;
        static_cast<ThrustDynamicsNonLinear*>(thrust)->set_params(param_Ct, param_Cq, rotor_constants.J);
        drone->get_logger().add_entry(*static_cast<ThrustDynamicsNonLinear*>(thrust), 
            create_logfile(LOGPATH(drone->get_index(), "log_thrust.csv"), *static_cast<ThrustDynamicsNonLinear*>(thrust)));
    }
    drone->set_thrus_dynamics(thrust);

    RotorConfigType rotor_config[ROTOR_NUM];
    std::vector<RotorPosition> pos = drone_config.getCompThrusterRotorPositions();
    HAKO_ASSERT(static_cast<size_t>(ROTOR_NUM) == pos.size());
    for (size_t i = 0; i < pos.size(); ++i) {
        rotor_config[i].ccw = pos[i].rotationDirection;
        rotor_config[i].data.x = pos[i].position[0];
        rotor_config[i].data.y = pos[i].position[1];
        rotor_config[i].data.z = pos[i].position[2];
    }    

    thrust->set_rotor_config(rotor_config);

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
    drone->get_logger().add_entry(*acc, 
        create_logfile(LOGPATH(drone->get_index(), "log_acc.csv"), *acc));

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
    drone->get_logger().add_entry(*gyro, 
        create_logfile(LOGPATH(drone->get_index(), "log_gyro.csv"), *gyro));

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
    drone->get_logger().add_entry(*mag, 
        create_logfile(LOGPATH(drone->get_index(), "log_mag.csv"), *mag));

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
    drone->get_logger().add_entry(*baro, 
        create_logfile(LOGPATH(drone->get_index(), "log_baro.csv"), *baro));

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
    drone->get_logger().add_entry(*gps, 
        create_logfile(LOGPATH(drone->get_index(), "log_gps.csv"), *gps));

    return drone;
}