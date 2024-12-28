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

The `cog`, or Course over Ground, in our drone simulation, signifies the drone's actual trajectory over the Earth's surface. It is distinct from the drone's heading and becomes particularly relevant when the drone is in motion.

- **Course over Ground Definition**: `cog` represents the actual path the drone follows over the ground, differing from its heading due to environmental factors like wind. It's calculated from the drone's movement vector, derived from its northward (vn) and eastward (ve) velocities, and is expressed in degrees from 0 to 359.99, with 0 degrees indicating true north.

- **COG During Stationary Conditions**: When the drone is not moving, `cog` is undefined since there is no ground movement to determine a travel direction. In such scenarios, `cog` is set to `UINT16_MAX` to indicate that the value is unknown or invalid. This helps in data interpretation, signaling to systems that the drone is stationary or the `cog` data is unavailable.

- **Realistic Noise Integration**: To mimic real-world conditions, noise is introduced in the `cog` calculations, simulating natural variations due to sensor inaccuracies and environmental factors.

Accurate simulation of `cog` is crucial for realistic drone navigation. It ensures that developed navigation systems can effectively differentiate between the drone's intended heading and its actual movement path, even when faced with external variabilities.


## others

As of the current state, the following data has been simplified and fixed values have been set. The set values have been chosen based on observations of communication data between PX4 and AirSim to select appropriate representative values.

- fix_type: 3
- eph: 10
- epv: 10
- satellites_visible: 10
- id: 0
- yaw: 0
