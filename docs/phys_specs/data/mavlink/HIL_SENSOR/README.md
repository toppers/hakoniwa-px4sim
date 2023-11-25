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

This section delves into the simulation of magnetometer data, considering the Earth's magnetic field variations and the introduction of realistic noise.

- **Geographic Variability in Magnetic North**: The Earth's magnetic field varies with latitude and longitude, influencing the magnetic north based on the drone's position. In our Hakoniwa simulation, magnetic north is defined as a parameter, determined by the initial geographic coordinates of the drone.

- **Source for Magnetic North Calculation**: The magnetic north for our simulation is calculated using data from the [Geospatial Information Authority of Japan's geomagnetic calculation service](https://vldb.gsi.go.jp/sokuchi/geomag/menu_04/index.html). This ensures the simulation reflects accurate magnetic conditions for the provided coordinates.

- **Calculating [xyz]mag with Drone's Attitude**: The [xyz]mag values are calculated based on the drone's attitude angles (φ, θ, ψ) in relation to the vector of magnetic north. This mimics how the drone's onboard magnetometer would interact with the Earth's magnetic field.

- **Incorporating Noise**: To enhance realism, a level of noise is introduced into the magnetometer data. This noise simulates the inaccuracies and variations that would be present in real-world sensor data, making the simulation more authentic and robust for testing purposes.

- **Role in Orientation and Navigation**: Accurate magnetometer data, complete with realistic noise, is essential for determining the drone's orientation in relation to the Earth's magnetic field. This is particularly important for navigation systems and compass functionality.

By simulating realistic magnetic conditions, including variations based on location and noise, we ensure a more comprehensive and true-to-life experience in the simulation. This detail is critical for the development and testing of navigation algorithms that utilize magnetometer data in real-world applications.

## abs_pressure

TODO

## diff_pressure

TODO

## pressure_alt

TODO

## temperature

TODO

## fields_updated

TODO

## id

TODO
