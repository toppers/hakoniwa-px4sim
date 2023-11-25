# HIL_GPS


The [HIL_GPS](https://mavlink.io/en/messages/common.html#HIL_GPS) specification is defined on the MAVLink site. While the MAVLink specification provides a general framework, in our PX4 implementation, we define the following specific values and parameters as our assumed standards.

* lat, lon, alt
* eph, epv
* vel, vn, ve, vd
* cog
* satelites_visible, id, yaw


The table below is a replica of the MAVLink specification:

| Field Name   | Type        | Units | Values | Description |
|--------------|-------------|-------|--------|-------------|
| time_usec    | uint64_t    | us    | -      |Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number.|
|fix_type|uint8_t|||0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.|
|lat|int32_t|degE7||Latitude (WGS84)|
|lon|int32_t|degE7||Longitude (WGS84)|
|alt|int32_t|mm||Altitude (MSL). Positive for up.|
|eph|uint16_t|||GPS HDOP horizontal dilution of position (unitless * 100). If unknown, set to: UINT16_MAX|
|epv|uint16_t|||GPS VDOP vertical dilution of position (unitless * 100). If unknown, set to: UINT16_MAX|
|vel|uint16_t|cm/s||GPS ground speed. If unknown, set to: UINT16_MAX|
|vn|int16_t|cm/s||GPS velocity in north direction in earth-fixed NED frame|
|ve|int16_t|cm/s||GPS velocity in east direction in earth-fixed NED frame|
|vd|int16_t|cm/s||GPS velocity in down direction in earth-fixed NED frame|
|cog|uint16_t|cdeg||Course over ground (NOT heading, but direction of movement), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX|
|satelites_visible|uint8_t|||Number of satellites visible. If unknown, set to UINT8_MAX|
|id|uint8_t|||GPS ID (zero indexed). Used for multiple GPS inputs|
|yaw|uint16_t|cdeg||Yaw of vehicle relative to Earth's North, zero means not available, use 36000 for north|

## lat, lon, alt

For the initial positioning of the drone in our simulation, `lat` (latitude), `lon` (longitude), and `alt` (altitude) are provided by the simulator. The simulator manages relative positioning, with the starting coordinates set as a base from which all movement is calculated.

- **Initial Coordinates**: The simulator assigns initial `lat`, `lon`, and `alt` values that represent the drone's starting point in the virtual world. These coordinates correspond to a real-world GPS location.

- **Relative Positioning**: As the drone moves, the simulator updates its position by adding the change in location to the initial values. This method allows the simulation to track the drone's journey from its origin point.

- **Noise Integration**: To enhance realism, a degree of GPS noise is integrated into the latitude, longitude, and altitude data. This noise simulates the small variations and inaccuracies that a real drone would experience due to GPS signal quality, atmospheric conditions, and other environmental factors.

This simulation setup ensures that the drone's virtual position remains consistent with realistic GPS data, providing an authentic navigational experience. The inclusion of noise is critical for developing robust navigation algorithms that can handle real-world GPS inaccuracies.



## vel, vn, ve, vd

In the context of our simulator, the velocity parameters `vel` (overall ground speed), `vn` (velocity northward), `ve` (velocity eastward), and `vd` (velocity downward) are essential for defining the drone's movement dynamics.

- **Overall Ground Speed (vel)**: This value represents the drone's speed over the ground, combining its northward and eastward components. It's crucial for understanding the drone's travel speed in relation to the surface.

- **Northward Velocity (vn)**: Reflects the component of the drone's velocity in the northern direction within the NED coordinate system. It's a key factor in navigation and is used to calculate the drone's displacement over time.

- **Eastward Velocity (ve)**: Represents the component of the drone's velocity towards the east. This, along with `vn`, helps in plotting the drone's trajectory in the horizontal plane.

- **Downward Velocity (vd)**: Indicates the rate at which the drone is moving downwards. In the NED coordinate system, 'down' corresponds to the negative Z-axis and is important for altitude management and vertical profiling.

Additionally, realistic noise is applied to these velocity readings to simulate the variances and uncertainties inherent in actual drone movement. This noise can result from various factors, including atmospheric conditions, terrain, and drone's response to control inputs.

By accurately simulating these velocities and incorporating noise, the simulator can effectively replicate the conditions a drone would encounter in real flight, aiding in the development of more sophisticated and resilient flight control algorithms.


## cog

The `cog` parameter, which stands for Course over Ground, is a crucial navigational data point in our simulator. It represents the drone's current travel direction relative to true north and is measured in degrees.

- **Course over Ground (cog)**: This value indicates the drone's directional heading over the earth's surface. It's expressed in degrees, with a range from 0 to 359.99, where 0 degrees corresponds to a northward trajectory.

- **Directional Accuracy**: The `cog` is calculated using the drone's horizontal velocity vectors (`vn` and `ve`). It provides an essential bearing for navigation, especially when planning waypoints or following a predetermined path.

- **Integration of Noise**: To simulate the real-world operational conditions, a degree of variance or 'noise' is added to the `cog` value. This reflects the minor fluctuations that can be expected due to factors like wind, air pressure changes, or compass inaccuracies.

The `cog` value, enhanced with noise, helps develop navigation systems that are capable of accurate heading determination even when faced with environmental uncertainties. This makes the simulated flight experience as close to reality as possible, which is invaluable for testing and refining drone navigation algorithms.


## others

TODO
