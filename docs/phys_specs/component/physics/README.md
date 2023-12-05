# Drone Dynamics

Drone Dynamics encompasses the mathematical and computational models that simulate the physical behavior of a drone during flight. These models incorporate the forces, moments, and kinematic equations that govern the movement of the drone through three-dimensional space. Dynamics are typically analyzed in two reference frames: the Body Frame, which moves with the drone, and the Ground Frame, which is stationary relative to the Earth.

![スクリーンショット 2023-12-05 9 45 06](https://github.com/toppers/hakoniwa-px4sim/assets/164193/bfef00a5-8c05-46da-a7a5-98efa777d90e)


## BodyFrameDroneDynamics

`BodyFrameDroneDynamics` component models the drone's behavior and motion from the perspective of its own body frame. It accounts for the linear and angular velocities, the impact of the propellers' thrust and torque, and the drone's attitude, which is its orientation in space. The body frame dynamics are essential for direct drone control and for understanding the immediate effects of control inputs on the drone's movement.

![スクリーンショット 2023-12-05 9 53 33](https://github.com/toppers/hakoniwa-px4sim/assets/164193/5c59bd35-99e3-45cd-b508-85b49e9c0049)

* u,v,w are the drone's velocities along its body-frame axes.
* g is the acceleration due to gravity.
* θ and φ are the pitch and roll angles, respectively.
* c is the air friction coefficient, affecting the velocity terms.
* m is the mass of the drone.
* p,q,r represent the angular velocities around the body-frame axes.
* τφ,τθ,τψ are the torques around the body-frame axes.
* cx​, cy, cz are coefficients related to the torques.

## GroundFrameVelocityConverter

`GroundFrameVelocityConverter` is tasked with translating the drone's velocity vectors and angular velocities from the body frame to the ground frame. This transformation is essential for navigation and control, allowing for the drone's movement to be interpreted and managed in relation to the Earth. This facilitates essential functions such as waypoint navigation, collision avoidance, and maintaining a stable hover.

Velocity: 

![スクリーンショット 2023-12-05 10 04 44](https://github.com/toppers/hakoniwa-px4sim/assets/164193/992bb7fe-0d50-47a5-aab5-e17aba4f716d)


AngularVelocity:

![スクリーンショット 2023-12-05 10 05 51](https://github.com/toppers/hakoniwa-px4sim/assets/164193/9b036e35-6ed5-4fd0-8ceb-05364e5cccdb)



## Integrator

The `Integrator` function numerically integrates time-dependent variables, such as velocity to determine position, or angular velocity to ascertain orientation angles. Within drone dynamics, it is utilized to compile incremental changes over time, thereby updating the drone's state, including its position and orientation in the ground coordinate frame. This continuous integration is crucial for plotting the drone's trajectory and for the feedback control systems that depend on the knowledge of the drone's historical path.
