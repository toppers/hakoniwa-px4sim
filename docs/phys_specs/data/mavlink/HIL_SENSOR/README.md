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

This section covers the simulation of magnetometer data within the Hakoniwa environment, reflecting the Earth's magnetic field variations and the drone's orientation.

- **Magnetic North Calculation**: Based on the initial geographic coordinates of the drone, Magnetic North is determined using data from the [Geospatial Information Authority of Japan's geomagnetic calculation service](https://vldb.gsi.go.jp/sokuchi/geomag/menu_04/index.html), ensuring an accurate representation of the Earth's magnetic conditions.

- **Drone's Attitude Impact**: The magnetometer readings, denoted by `xmag`, `ymag`, and potentially `zmag`, are calculated by considering the drone's roll, pitch, and yaw angles relative to Magnetic North. These readings simulate the interaction between the onboard magnetometer and the Earth's magnetic field, factoring in the drone's orientation.

![Magnetometer Readings Simulation Diagram](https://github.com/toppers/hakoniwa-px4sim/assets/164193/40d7d3db-e930-49c4-8c9d-e3920378cd84)

- **Realism through Noise**: Realistic noise is introduced to the magnetometer data to mirror the natural inaccuracies and environmental variations experienced by real-world sensors.

- **Navigation and Orientation**: The magnetometer data, inclusive of realistic noise, are critical for determining the drone's orientation, crucial for navigation systems and compass functionality in simulation.

The method assumes a constant vector for Magnetic North, against which the drone's orientation-induced variations are measured. This ensures that `xmag` and `ymag` reflect the relative intensity and direction of the Earth's magnetic field as it would be perceived by the drone in real flight conditions.

By integrating both the geographic dependence of Magnetic North and the drone's relative orientation, our simulation provides a comprehensive platform for developing and testing navigation algorithms that rely on magnetometer data.

## others

As of the current state, the following data has been simplified and fixed values have been set. The set values have been chosen based on observations of communication data between PX4 and AirSim to select appropriate representative values.

- abs_pressure: 997.667
- diff_pressure: 0
- pressure_alt: 130.476
- temperature: 0
- fields_updated: 0x1FFF
- id: 0


