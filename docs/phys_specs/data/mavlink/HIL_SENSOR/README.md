# HIL_SENSOR


The [HIL_SENSOR](https://mavlink.io/en/messages/common.html#HIL_SENSOR) specification is defined on the MAVLink site. While the MAVLink specification provides a general framework, in our PX4 implementation, we define the following specific values and parameters as our assumed standards.

* [xyz]acc
* [xyz]gyro
* [xyz]mag
* abs_pressure
* diff_pressure
* pressure_alt
* temperature
* fields_updated
* id

The table below is a replica of the MAVLink specification:

| Field Name   | Type        | Units | Values | Description |
|--------------|-------------|-------|--------|-------------|
| time_usec    | uint64_t    | us    | -      |Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number.|
|xacc|float|m/s/s||X acceleration|
|yacc|float|m/s/s||Y acceleration|
|zacc|float|m/s/s||Z acceleration|
|xgyro|float|rad/s||Angular speed around X axis in body frame|
|ygyro|float|rad/s||Angular speed around Y axis in body frame|
|zgyro|float|rad/s||Angular speed around Z axis in body frame|
|xmag|float|gauss||X Magnetic field|
|ymag|float|gauss||Y Magnetic field|
|zmag|float|gauss||Z Magnetic field|
|abs_pressure|float|hPa||Absolute pressure|
|diff_pressure|float|hPa||Differential pressure (airspeed)|
|pressure_alt|float|||Altitude calculated from pressure|
|temperature|float|degC||Temperature|
|fields_updated	|uint32_t||HIL_SENSOR_UPDATED_FLAGS|Bitmap for fields that have updated since last message|
|id **|uint8_t|||Sensor ID (zero indexed). Used for multiple sensor inputs|


## [xyz]acc


In this section, we discuss the handling of accelerometer data in our simulation environment:

- **Accurate Position and Speed Understanding**: In the simulation, we have precise knowledge of the physical model's position and speed, allowing for ideal accelerometer data generation.

- **Considering Noise**: Similar to real-world sensors, we assume a certain level of noise in the simulated accelerometer data. This assumption helps in mimicking real flight conditions more accurately.

- **Coordinate System and Body Frame**: The accelerometer data in the simulation is represented in the NED (North, East, Down) coordinate system, relative to the body frame of the drone. This representation aligns the simulated data with real-world drone movements.

These factors play a crucial role in the operation and data interpretation of the accelerometer in the simulation environment. Details on data processing methods will be discussed in a separate section.

## [xyz]gyro

This section focuses on the simulation handling of the gyroscope data, which is crucial for understanding the rotational movement of the drone.

- **Accurate Angular Velocity Measurement**: In the simulation environment, the gyroscope data accurately captures the drone's angular velocity, providing precise information about its rotational movements around the X, Y, and Z axes.

- **Noise Consideration in Simulation**: Just like in real-world scenarios, the simulated gyroscope data includes a realistic level of noise. This enhances the authenticity of the simulation, replicating real flight dynamics more closely.

- **NED Coordinate System and Body Frame Orientation**: The gyroscope data is represented in the NED (North, East, Down) coordinate system within the drone's body frame. This orientation ensures that the data corresponds closely with the actual rotational behavior of a drone in flight.

- **Application in Flight Dynamics**: Gyroscope data is essential for various flight control algorithms, particularly in stabilization and navigation tasks. In the simulation, this data helps in accurately mimicking the drone's response to control inputs and environmental factors.

The handling of gyroscope data in the simulation is a key component in creating a realistic and responsive flight experience. This data is vital for testing and refining drone control systems in a simulated environment before real-world implementation.

## [xyz]mag

This section details the process of simulating magnetometer data within the Hakoniwa environment, reflecting the complexities of the Earth's magnetic field and its interaction with the drone's orientation.

![スクリーンショット 2023-12-09 10 58 16](https://github.com/toppers/hakoniwa-px4sim/assets/164193/31bce2f9-cfc6-4aff-a2d6-06075344c2e7)


### Magnetic North Calculation
Magnetic North is determined based on the drone's initial geographic coordinates, utilizing data from the [IGRF-13](https://wdc.kugi.kyoto-u.ac.jp/igrf/point/index-j.html). This ensures an accurate portrayal of Earth's magnetic conditions within the simulation.

#### Example

Location:

* Lat: 47.641468
* Lon: -122.140165
* Alt: 121.321


Data:
* Total Magnetic Field (F): 53045.1 nT
* Northward (X): 18348.5 nT
* Eastward (Y): 5021.6 nT
* Downward (Z): 49516.6 nT
* Horizontal (H): 19023.3 nT
* Declination (D): 15.306 degrees
* Inclination (I): 68.984 degrees


### Drone's Attitude Impact
The onboard magnetometer readings, labeled as `xmag`, `ymag`, and `zmag`, are influenced by the drone's roll, pitch, and yaw. These values are crucial for simulating the magnetometer's interaction with the Earth's magnetic field, considering the drone's changing orientation.

### Realism through Noise
To enhance the realism of the simulation, noise is introduced to the magnetometer data. This mimics the natural inaccuracies and the environmental variations that affect real-world sensors.

### Navigation and Orientation
The magnetometer data, with the added layer of realistic noise, are indispensable in determining the drone's heading. This data is crucial for the operation of navigation systems and compass functionalities in the simulation.

The simulation assumes a fixed vector for Magnetic North, which serves as a reference for measuring the drone's orientation-induced variations. This ensures that `xmag` and `ymag` accurately reflect the relative intensity and direction of the Earth's magnetic field, as it would be perceived by a real drone in flight.

By incorporating both the geographic influence on Magnetic North and the drone's relative orientation, the Hakoniwa simulation provides an extensive platform for the development and testing of navigation algorithms that utilize magnetometer data.

## others

As of the current state, the following data has been simplified and fixed values have been set. The set values have been chosen based on observations of communication data between PX4 and AirSim to select appropriate representative values.

- abs_pressure: 997.667
- diff_pressure: 0
- pressure_alt: 130.476
- temperature: 0
- fields_updated: 0x1FFF
- id: 0


