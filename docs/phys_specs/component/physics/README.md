# Drone Dynamics

Drone Dynamics encompasses the mathematical and computational models that simulate the physical behavior of a drone during flight. These models incorporate the forces, moments, and kinematic equations that govern the movement of the drone through three-dimensional space. Dynamics are typically analyzed in two reference frames: the Body Frame, which moves with the drone, and the Ground Frame, which is stationary relative to the Earth.

![スクリーンショット 2023-12-05 9 45 06](https://github.com/toppers/hakoniwa-px4sim/assets/164193/bfef00a5-8c05-46da-a7a5-98efa777d90e)


## BodyFrameDroneDynamics

`BodyFrameDroneDynamics` component models the drone's behavior and motion from the perspective of its own body frame. It accounts for the linear and angular velocities, the impact of the propellers' thrust and torque, and the drone's attitude, which is its orientation in space. The body frame dynamics are essential for direct drone control and for understanding the immediate effects of control inputs on the drone's movement.

The basic dynamics equations in the body frame are as follows.

$m \dot{v} + \omega \times m v = F$

$I \dot{\omega} + \omega \times I \omega = \tau$

where;

- $m$ - the mass of the drone
- $I$ - the inertia matrix of the drone 
- $v$ - the linear velocity of the drone $v=(u, v, w)$
- $\omega$ - the angular velocity of the drone $\omega = (p, q, r)$
- $F$ - the force vector including gravity($mg$), drag($-dv)$, and thrust $(T)$
- $\tau$ - the torque vector including the propeller torque


The body frame dynamics based on the body angles $\phi, \theta, \psi$, are described in the following equations.


$\dot{u} = -g \sin{\theta} -(qw -rv) -\frac{d}{m}u$

$\dot{v} = g \cos{\theta}\sin{\phi} -(ru -pw) -\frac{d}{m}v$

$\dot{w} = -\frac{T}{m} + g \cos{\theta}cos{\phi} -(pv-qu)-\frac{d}{m}w$

$\dot{p} = (\tau_{\phi} -qr(I_{zz}-I_{yy}))/I_{xx} $

$\dot{q} = (\tau_{\theta}-rp(I_{xx}-I_{zz}))/I_{yy}$

$\dot{r} = (\tau_{\psi}-pq(I_{yy}-I_{xx}))/I_{zz}$

- $g$ is the acceleration due to gravity.
- $\phi, \theta, \psi$ are the roll(x-axis), pitch(y-axis) and yaw(z-axis) angles, respectively($\psi$
- $d$ is the air friction coefficient called "drag", affecting the velocity terms.
- $I_{xx}​,I_{yy}, I_{zz}$ are inertia moments around the body-frame axes $x, y, z$ respectively($x, y, z$ should be aligned with the drone's principal axes, from the gravity-center(other slant inertia $I_{xy}, I_{yz}, I_{zx}$ are assumed to be zero).


References:
- https://mtkbirdman.com/flight-dynamics-body-axes-system/ 
- https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf
- Nonami's book(details of the above) eq.(1.136).


## GroundFrameVelocityConverter

`GroundFrameVelocityConverter` is tasked with translating the drone's velocity vectors and angular velocities from the body frame to the ground frame. This transformation is essential for navigation and control, allowing for the drone's movement to be interpreted and managed in relation to the Earth. This facilitates essential functions such as waypoint navigation, collision avoidance, and maintaining a stable hover.

Velocity: 

![スクリーンショット 2023-12-05 10 04 44](https://github.com/toppers/hakoniwa-px4sim/assets/164193/992bb7fe-0d50-47a5-aab5-e17aba4f716d)


AngularVelocity:

![スクリーンショット 2023-12-05 10 05 51](https://github.com/toppers/hakoniwa-px4sim/assets/164193/9b036e35-6ed5-4fd0-8ceb-05364e5cccdb)



## Integrator

The `Integrator` function numerically integrates time-dependent variables, such as velocity to determine position, or angular velocity to ascertain orientation angles. Within drone dynamics, it is utilized to compile incremental changes over time, thereby updating the drone's state, including its position and orientation in the ground coordinate frame. This continuous integration is crucial for plotting the drone's trajectory and for the feedback control systems that depend on the knowledge of the drone's historical path.
