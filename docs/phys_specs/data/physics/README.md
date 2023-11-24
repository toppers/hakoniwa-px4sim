# Position

This section describes the coordinate system and units used to determine the position of the drone, including the definition of the origin point.

| Attribute          | Description                                      |
|--------------------|--------------------------------------------------|
| Coordinate System  | Ground Coordinate System (NED)                   |
| Unit               | Meter                                            |
| X                  | North (Distance towards the North)               |
| Y                  | East (Distance towards the East)                 |
| Z                  | Down (Distance downwards, height from ground)    |

## About the Coordinate System
- **Ground Coordinate System (NED)**: The NED coordinate system uses a ground-fixed reference frame, defining positions in terms of North (X), East (Y), and Down (Z) axes. This system is commonly used in aviation and helps in intuitively understanding the position of drones.

## About the Origin
- The origin point is defined by the initial values of latitude, longitude, and altitude (ALT) provided. This origin serves as the reference point for all positional measurements of the drone in the NED coordinate system.

## About the Unit
- **Meter**: The position of the drone along each axis is measured in meters. This ensures precision in positioning, conforming to international standards.

# Velocity

This section outlines the coordinate system and units used for measuring the drone's velocity, along with relevant details.

| Attribute          | Description                                      |
|--------------------|--------------------------------------------------|
| Coordinate System  | Ground Coordinate System (NED)                   |
| Unit               | Meters per Second (m/s)                          |
| X                  | Velocity towards the North                       |
| Y                  | Velocity towards the East                        |
| Z                  | Velocity downwards (towards the ground)          |

## About the Coordinate System
- **Ground Coordinate System (NED)**: For velocity measurements, the NED coordinate system is used. In this system, the axes are oriented North (X), East (Y), and Down (Z), effectively representing the drone's movement in three-dimensional space.

## About the Unit
- **Meters per Second (m/s)**: The velocity of the drone is measured in meters per second. This unit is the standard for speed measurement, offering a clear and consistent way to gauge how fast the drone is moving in each direction.

## Interpreting Velocity Data
- The velocity data represents the rate of change of position along each axis of the NED coordinate system. Positive values in the X and Y axes indicate movement towards North and East, respectively. For the Z axis, a positive value indicates movement downwards (towards the ground), in line with the orientation of the NED coordinate system.

# Angle

# AngularVelocity

# RotorSpeedVariation
