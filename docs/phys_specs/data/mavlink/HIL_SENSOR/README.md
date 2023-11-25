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

TODO

## [xyz]mag

TODO

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
