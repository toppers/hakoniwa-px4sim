# Architecture

This architectural diagram represents the simulation configuration of a drone, including the PX4 platform and related sensors and actuators. Each component processes a specific type of data and passes this data to other parts within the system. This interaction simulates the real-world operation of the drone.

![スクリーンショット 2023-12-05 9 45 49](https://github.com/toppers/hakoniwa-px4sim/assets/164193/dff76773-3f51-4fbd-a4a1-948ec4621754)


Here is a table summarizing the input/output data and overview of each component in the aforementioned architecture.

|component|type|overview|input|output|
|---|---|---|---|---|
|PX4|Platform|Core flight control platform for the drone| [HIL_SENSOR](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md) <br> [HIL_GPS](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_GPS/README.md) | [HIL_ACTUATOR_CONTROLS](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_ACTUATOR_CONTROLS/README.md)|
|[DroneDynamics](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/component/physics/README.md)|Physics|Simulates physical dynamics of the drone|[Thrust/Torque](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#thrust)| [Position](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#position)<br>[Velocity](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#velocity)<br>[Angle](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angle)<br>[AngularVelocity(Body)](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angular-velocity-in-body-coordinate-system)<br>[Velocity(Body)](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#velocity-in-body-coordinate-system)|
|[ThrustDynamics](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/component/actuator/README.md#thrust-and-torque-dynamics)|Physics|Simulates physical dynamics of multiple rotors|[RotorSpeed](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#rotor-speed)|[Thrust/Torque](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#thrust)|
|[RotorDynamics](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/component/actuator/README.md)|Actuator|Controls the speed of rotors based on input| [HIL_ACTUATOR_CONTROLS](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_ACTUATOR_CONTROLS/README.md) | [RotorSpeed](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#rotor-speed) |
|[Acceleration](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/component/sensor/acceleration/README.md)|Sensor|Measures acceleration in 3D space| [Velocity(Body)](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#velocity-in-body-coordinate-system)| [HIL_SENSOR](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md)/acc |
|[Gyro](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/component/sensor/gyro/README.md)|Sensor|Measures rotational motion| [AngularVelocity(Body)](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angular-velocity-in-body-coordinate-system)| [HIL_SENSOR](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md)/gyro |
|[Geomagnetic](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/component/sensor/geomagnet/README.md)|Sensor|Measures Earth's magnetic field for orientation| [Angle](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angle) | [HIL_SENSOR](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md)/mag |
|Barometric|Sensor|Measures atmospheric pressure for altitude| [Position](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#position) | [HIL_SENSOR](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md)/pressure |
|[GPS](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/component/sensor/gps/README.md)|Sensor|Provides location coordinates and velocity| [Position](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#position)<br>[Velocity](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#velocity) | [HIL_GPS](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_GPS/README.md) |

Here is a table of the sending intervals for the simulator's MAVLink messages, as observed in AirSim:

| Sensor Data  | Sending Interval (ms) |
|--------------|-----------------------|
| SYSTEM_TIME  | 3                     |
| HIL_SENSOR   | 3                     |
| HIL_GPS      | 21                    |

