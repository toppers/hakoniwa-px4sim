# Drone Dynamics

Here, we describe the architecture and components used for simulating drone dynamics.

## Architecture

The overall architecture is as shown in the figure below.

![スクリーンショット 2024-01-16 8 14 06](https://github.com/toppers/hakoniwa-px4sim/assets/164193/3d1b9ade-262f-4366-bf15-04bc56347d24)


### BodyFrameDroneDynamics

This component calculates the acceleration and angular acceleration in the body frame of reference based on the input thrust and torque.

### AccelerationIntegrator

Integrates the angular velocity and angular acceleration calculated by BodyFrameDroneDynamics into velocity in the body frame of reference.

### GroundFrameVelocityConverter

Converts the velocity calculated in the body frame of reference to velocity in the ground frame of reference.

### VelocityIntegrator

Integrates the velocity in the ground frame of reference to update the position and attitude angle of the drone.

## References

For precise equations and specifications of **BodyFrameDroneDynamics** and **GroundFrameVelocityConverter**, please refer to the [detailed documentation](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_physics/README.md).
