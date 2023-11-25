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

TODO

## vel, vn, ve, vd

TODO

## cog

TODO

## others

TODO
