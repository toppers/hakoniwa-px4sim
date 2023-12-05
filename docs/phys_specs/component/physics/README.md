# Drone Dynamics

Drone Dynamics encompasses the mathematical and computational models that simulate the physical behavior of a drone during flight. These models incorporate the forces, moments, and kinematic equations that govern the movement of the drone through three-dimensional space. Dynamics are typically analyzed in two reference frames: the Body Frame, which moves with the drone, and the Ground Frame, which is stationary relative to the Earth.

## BodyFrameDroneDynamics

`BodyFrameDroneDynamics` component models the drone's behavior and motion from the perspective of its own body frame. It accounts for the linear and angular velocities, the impact of the propellers' thrust and torque, and the drone's attitude, which is its orientation in space. The body frame dynamics are essential for direct drone control and for understanding the immediate effects of control inputs on the drone's movement.

## GroundFrameVelocityConverter

`GroundFrameVelocityConverter` is tasked with translating the drone's velocity vectors and angular velocities from the body frame to the ground frame. This transformation is essential for navigation and control, allowing for the drone's movement to be interpreted and managed in relation to the Earth. This facilitates essential functions such as waypoint navigation, collision avoidance, and maintaining a stable hover.

## Integrator

The `Integrator` function numerically integrates time-dependent variables, such as velocity to determine position, or angular velocity to ascertain orientation angles. Within drone dynamics, it is utilized to compile incremental changes over time, thereby updating the drone's state, including its position and orientation in the ground coordinate frame. This continuous integration is crucial for plotting the drone's trajectory and for the feedback control systems that depend on the knowledge of the drone's historical path.
