# HIL_ACTUATOR_CONTROLS

The [HIL_ACTUATOR_CONTROLS](https://mavlink.io/en/messages/common.html#HIL_ACTUATOR_CONTROLS) specification is defined on the MAVLink site. While the MAVLink specification provides a general framework, in our PX4 implementation, we define the following specific values and parameters as our assumed standards.

* controls

The table below is a replica of the MAVLink specification:

| Field Name   | Type        | Units | Values | Description |
|--------------|-------------|-------|--------|-------------|
| time_usec    | uint64_t    | us    | -      | Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. |
| controls     | float[16]   | -     | -      | Control outputs -1 .. 1. Channel assignment depends on the simulated hardware. |
| mode         | uint8_t     | -|MAV_MODE_FLAG | System mode. Includes arming state. |
| flags        | uint64_t    | -     | -     | Flags as bitfield, 1: indicate simulation using lockstep. |

## controls

While PX4 allows for customization in the number of rotors, our current Hakoniwa version is configured with four rotors. Consequently, we've designed the controls array to accommodate four corresponding values.

According to the MAVLink specifications, values are typically ranged from -1 to 1. However, our observations from the AirSim and PX4 communication data indicate that the value range actually spans from 0 to 1.

Based on this finding, we've structured the control values to align with a motor Duty ratio ranging from 0 to 1. In this scheme, a value of 0 corresponds to no output, while a value of 1 represents the maximum output.
