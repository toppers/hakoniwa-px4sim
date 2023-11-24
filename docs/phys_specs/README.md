# Architecture

This architectural diagram represents the simulation configuration of a drone, including the PX4 platform and related sensors and actuators. Each component processes a specific type of data and passes this data to other parts within the system. This interaction simulates the real-world operation of the drone.

![image](https://github.com/toppers/hakoniwa-px4sim/assets/164193/706f9782-b52d-43c3-9dd9-d6518ecf1744)

Here is a table summarizing the input/output data and overview of each component in the aforementioned architecture.

|component|type|overview|input|output|
|---|---|---|---|---|
|PX4|Platform|Core flight control platform for the drone| - HIL_SENSOR <br> - HIL_GPS | - HIL_ACTUATOR_CONTROLS|
|DronePhysics|Physics|Simulates physical dynamics of the drone| - RotorSpeedVariation ΔΩ | - Position<br>- Velocity<br>- Angle<br>- AngularVelocity<br>|
|RotorDynamics|Actuator|Controls the speed of rotors based on input| - PWMDutyCycle | - RotorSpeedVariation ΔΩ |
|Acceleration|Sensor|Measures acceleration in 3D space| - Velocity | - HIL_SENSOR/acc |
|Gyro|Sensor|Measures rotational motion| - AngularVelocity | - HIL_SENSOR/gyro |
|Geomagnetic|Sensor|Measures Earth's magnetic field for orientation| - Angle | - HIL_SENSOR/mag |
|Barometric|Sensor|Measures atmospheric pressure for altitude| - Position | - HIL_SENSOR/pressure |
|GPS|Sensor|Provides location coordinates and velocity| - Velocity | - HIL_GPS |
