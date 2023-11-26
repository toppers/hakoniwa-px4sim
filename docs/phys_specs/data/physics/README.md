# Position

This section describes the coordinate system and units used to determine the position of the drone, including the definition of the origin point.

| Attribute          | Description                                      |
|--------------------|--------------------------------------------------|
| Coordinate System  | Ground Coordinate System (NED)                   |
| Unit               | Meter                                            |
| X                  | North (Distance towards the North)               |
| Y                  | East (Distance towards the East)                 |
| Z                  | Down (Distance downwards, height from ground)    |

![スクリーンショット 2023-11-24 16 27 52](https://github.com/toppers/hakoniwa-px4sim/assets/164193/4f1a23af-7eb1-41a6-9608-56794593293f)


## About the Coordinate System
- **Ground Coordinate System (NED)**: The NED coordinate system uses a ground-fixed reference frame, defining positions in terms of North (X), East (Y), and Down (Z) axes. This system is commonly used in aviation and helps in intuitively understanding the position of drones.

## About the Origin
- The origin point is defined by the initial values of latitude, longitude, and altitude (ALT) provided. This origin serves as the reference point for all positional measurements of the drone in the NED coordinate system.

## Interpreting Position Data
- In the NED coordinate system, positive values in the X and Y axes indicate a position to the North and East of the origin, respectively. For the Z axis, a positive value indicates a position below the origin point, aligning with the downward orientation of the Z axis in the NED system.

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

This section details the orientation of the drone, described in terms of angles along the NED coordinate system axes, and measured in radians.

| Attribute          | Description                                      |
|--------------------|--------------------------------------------------|
| Coordinate System  | Ground Coordinate System (NED)                   |
| Unit               | Radian                                           |
| Roll               | Rotation around the X-axis (North)               |
| Pitch              | Rotation around the Y-axis (East)                |
| Yaw                | Rotation around the Z-axis (Down)                |

![スクリーンショット 2023-11-24 16 33 07](https://github.com/toppers/hakoniwa-px4sim/assets/164193/fcd1339a-c0b0-448d-9e9d-14fecc58f6df)


## About the Coordinate System
- **Ground Coordinate System (NED)**: The NED system is used to describe the orientation of the drone. In this system, roll, pitch, and yaw angles are defined relative to the North (X), East (Y), and Down (Z) axes respectively.

## About the Unit
- **Radian**: Angles are measured in radians, providing a standard and precise way to quantify rotation. One full rotation (360 degrees) equals 2π radians.

## Interpreting Angle Data
- **Roll**: A positive roll angle indicates a clockwise rotation when viewed from the rear, tilting the right wing down and the left wing up.
- **Pitch**: A positive pitch angle denotes the nose of the drone tilting up.
- **Yaw**: A positive yaw angle signifies a clockwise rotation when viewed from above, turning the drone to the right.

These angular orientations help in understanding the drone's attitude in 3D space, crucial for navigation and control.



# AngularVelocity

This section explains the measurement of the drone's angular velocity, which is the rate of change of its angular position over time.

| Attribute          | Description                                           |
|--------------------|-------------------------------------------------------|
| Coordinate System  | Ground Coordinate System (NED)                        |
| Unit               | Radian per Second (rad/s)                             |
| Roll Rate          | Angular velocity around the X-axis (North)            |
| Pitch Rate         | Angular velocity around the Y-axis (East)             |
| Yaw Rate           | Angular velocity around the Z-axis (Down)             |

## About the Coordinate System
- **Ground Coordinate System (NED)**: In this system, the angular velocity is measured relative to the North (X), East (Y), and Down (Z) axes, corresponding to roll, pitch, and yaw movements.

## About the Unit
- **Radian per Second (rad/s)**: Angular velocity is measured in radians per second. This unit quantifies how quickly the drone is rotating about each of the three axes.

## Interpreting Angular Velocity Data
- **Roll Rate**: A positive value indicates a clockwise rotation around the X-axis when viewed from the rear.
- **Pitch Rate**: A positive value denotes a nose-up rotation around the Y-axis.
- **Yaw Rate**: A positive value signifies a clockwise rotation around the Z-axis when viewed from above.

Understanding these rates of angular velocity is crucial for controlling the drone's orientation and for stabilizing its flight.

# Rotor Speed

This section describes the variation in rotor speed, a critical parameter for understanding and managing the drone's flight dynamics.

| Attribute   | Description |
|-------------|-------------|
| **Unit**    | Revolutions per Second (RPS) |
| **Description** | The rate of change in the drone's rotor speed, directly influencing lift and flight stability |

## About the Measurement
- **Rotor Speed**: Indicates the rate of change in the rotational speed of the drone's rotors. It is a key determinant of the drone's lifting capability, stability, and agility.

## About the Unit
- **Revolutions per Second (RPS)**: Measures the frequency of the rotor's complete revolutions in one second. This unit provides a direct indication of how quickly the rotor speed is increasing or decreasing.

## Interpreting Rotor Speed Variation
- A positive **RotorSpeed** value signifies an increase in rotor speed, typically associated with an increase in lift and the drone's potential to ascend.
- A negative **RotorSpeed** value indicates a decrease in rotor speed, corresponding to a reduction in lift and the drone's potential to descend.

Comprehending these changes in rotor speed is crucial for the effective control of the drone's altitude and stability during various flight maneuvers.


# Thrust

This section explains how the rotors' speed translates into thrust, which is essential for the drone's vertical movement.

| Attribute   | Description |
|-------------|-------------|
| **Unit**    | Newton (N) |
| **Description** | The force exerted by the rotors to lift the drone against gravity |

## About the Measurement

- **Thrust**: The upward force generated by the rotors, crucial for lift-off, hovering, and ascending motions.

## About the Unit
- **Newton (N)**: The SI unit of force, measuring the amount of force exerted. For drones, it equates to the force required to accelerate the drone's mass.

## Interpreting Rotor Speed Variation
- Greater thrust than the drone's weight results in ascending motion.
- Less thrust than the drone's weight causes descending motion.
- Equal thrust and weight allow for hovering at a constant altitude.

Understanding thrust dynamics is essential for managing the drone's lift and ensuring stable flight.

# Torque

This section delves into the torques generated by rotor motion that affect the drone's orientation and stability.

| Attribute   | Description |
|-------------|-------------|
| **Unit**    | Newton-meter (Nm) |
| **Description** | The rotational force that influences the drone's yaw, pitch, and roll |

## About the Measurement

- **Torque**: The measure of rotational force produced by the rotors that contributes to the drone's turning and angular orientation.


## About the Unit
- **Newton-meter (Nm)**: The SI unit of torque, representing the force applied at a distance from the rotation axis.


## Interpreting Rotor Speed Variation
- Aerodynamic torque is generated by rotor blades moving through the air and is proportional to the rotor speed squared.
- Reactive torque, or counter-torque, is the drone's body response to the rotors' rotation, typically counteracted by opposite rotor spins.

Understanding torque dynamics is vital for precision in controlling the drone's heading and attitude during flight.
