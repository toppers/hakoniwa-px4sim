English ｜ [日本語](README-ja.md)

# Hakoniwa Drone Physics Library(math, physics and dynamics)

## What is this ?

This is a math, physics, and dynamics library for the drone plant model in the Hakoniwa project(Open Source Runtime Environment for Simulating Cyber-Physical Systems).

It is designed for Hakoniwa px4sim project, but this library is more general, and can be used for any drone simulation projects
to calculate the drone's speed, acceleration and etc. in the ground and the body frame coordinate system.

Most of the functions are implemented based on the equations in the following book:

- ["Introduction to Drone Engineering" by Dr. Kenzo Nonami(Japanese)](https://www.coronasha.co.jp/np/isbn/9784339032307/)

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/c92d3d96-25f9-4b6a-ae4e-25d898b75a28)

All the functions are implemented in C++, with the equation numbers in the book as comments in the sourde code.

I hope this can be a reference implmentation for the basic drone dynamics.

## Hello World

```cpp
#include <iostream>

// include the library header
#include "drone_physics.hpp"

int main() {
    // the namespace of the library
    using namespace hako::drone_physics;

    // create a body frame from Euler angles.
    VectorType frame = {0, 0, M_PI/2};
    VelocityType body_velocity = {100, 200, 300};
    
    // Convert the body velocity to the ground frame.
    VelocityType ground_velocity = velocity_body_to_ground(body_velocity, frame);

    // get the x,y,z components of the velocity.
    auto [u, v, w] = ground_velocity;

    std::cout << "u = " << u << ", v = " << v << ", w = " << w << std::endl;
    // output: u = 200, v = -100, w = 300

    // you can also use explicit constructors.
    // reverse the conversion to the body frame.
    VelocityType body_velocity2 = velocity_ground_to_body(
        VelocityType(u, v, w),
        AngleType(0, 0, M_PI/2)
    );

    auto [u2, v2, w2] = body_velocity2;
    std::cout << "u2 = " << u2 << ", v2 = " << v2 << ", w2 = " << w2 << std::endl;
    // output: u2 = 100, v2 = 200, w2 = 300, back again.
}
```
## List of functions

Functions are implemented in the following categories, with the referece to the book.

### Frame conversion functions:
| Function | equation | note |
|----------|-----------|------|
|`velocity_body_to_ground`  | (1.71), (1.124) | Body velocity to ground velocity |
|`velocity_ground_to_body`  | (1.69), inverse of (1.124) | Ground velocity to body velocity |
|`angular_velocity_body_to_ground` | (1.109) | Body angular velocity to ground angular velocity |
|`angular_velocity_ground_to_body` | (1.106) | Ground angular velocity to body angular velocity |

### Body dynamics(Acceleration) functions:
| Function | equations in the book | note |
|----------|-----------|------|
|`acceleration_in_body_frame` | (1.136),(2.31) | Acceleration in body frame |
|`angular_acceleration_in_body_frame` | (1.37),(2.31) | Angular acceleration in body frame |

### Rotor dynamics functions:
| Function | equations in the book | note |
|----------|-----------|------|
|`rotor_omega_acceleration` | (2.48) | Rotor angular velocity acceleration from dury rate |
|`rotor_thrust` | (2.50) | Rotor thrust from rotor angular velocity |

### Body dynamics(n rotors) functions:
| Function | equations in the book | note |
|----------|-----------|------|
|`body_thrust` | (2.61) | Sum of the $n$ trust from the rotors |
|`body_torque` | (2.60)-(2.62) | Sum of the torques from the $n$ rotors based on the positionings of them |
|`rotor_anti_torque` | (2.56) | Rotor anti-torque from rotor thrust. this makes z-axis rotation. |

## Usage

See examples.cpp and utest.cpp for more examples.

## Equations

The ground frame coordinate system fixed to the ground is defined by right hand rule,
in which $z$-axis is downward.

The body frame coordinate system is defined by right hand rule, in which $x$-axis is the front of the drone, $y$-axis is the right side of the drone, and $z$-axis is the bottom of the drone. The origin of the body frame is the center of gravity of the drone. The body frame is attached to the drone, and the body frame moves with the drone.

The basic dynamics equations in the body frame are as follows eq.(2.31).

$m \dot{v} + \omega \times m v = F$

$I \dot{\omega} + \omega \times I \omega = \tau$

where;

- $m$ - mass of the drone
- $I$ - inertia matrix of the drone 
- $v$ - linear velocity of the drone $v=(u, v, w)$
- $\omega$ - angular velocity of the drone $\omega = (p, q, r)$
- $F$ - force vector including gravity($mg$), drag($-dv)$, and thrust $(T)$
- $\tau$ - torque vector from the rotors


The body frame dynamics above are expanded based on the body angles $\phi, \theta, \psi$ as follows.


$\dot{u} = -g \sin{\theta} -(qw -rv) -\frac{d}{m}u$

$\dot{v} = g \cos{\theta}\sin{\phi} -(ru -pw) -\frac{d}{m}v$

$\dot{w} = -\frac{T}{m} + g \cos{\theta}cos{\phi} -(pv-qu)-\frac{d}{m}w$

$\dot{p} = (\tau_{\phi} -qr(I_{zz}-I_{yy}))/I_{xx} $

$\dot{q} = (\tau_{\theta}-rp(I_{xx}-I_{zz}))/I_{yy}$

$\dot{r} = (\tau_{\psi}-pq(I_{yy}-I_{xx}))/I_{zz}$

where;

- $g$ is the acceleration due to gravity.
- $\phi, \theta, \psi$ are the roll($x$-axis), pitch($y$-axis) and yaw($z$-axis) angles, respectively.
- $d$ is the air friction coefficient called "drag", affecting the velocity terms.
- $I_{xx}​,I_{yy}, I_{zz}$ are inertia moments around the body-frame axes $x, y, z$ respectively($x, y, z$ should be aligned with the drone's principal axes, from the gravity-center(other slant inertia $I_{xy}, I_{yz}, I_{zx}$ are assumed to be zero).

The transformation from/to the body frame and the ground frame is as follows.

Velocity: 

![スクリーンショット 2023-12-05 10 04 44](https://github.com/toppers/hakoniwa-px4sim/assets/164193/992bb7fe-0d50-47a5-aab5-e17aba4f716d)


AngularVelocity:

![スクリーンショット 2023-12-05 10 05 51](https://github.com/toppers/hakoniwa-px4sim/assets/164193/9b036e35-6ed5-4fd0-8ceb-05364e5cccdb)

Each rotor can be modeled as a first-order lag system, in which the rotor angular velocity
$\Omega(t)$ is controlled by the duty rate $d(t)$, described as transfer function G(s)
eq.(2.48) in the book,

$G(s)/D(s) = K_r/(T_r s + 1)$

and the time domain differential equation is as follows.

$\dot{\Omega}(t) = K_r (d(t) - \Omega / T_r)$

where;

- $K_r$ is the rotor gain constant.
- $T_r$ is the rotor time constant.
- $d(t)$ is the duty rate of the rotor. ($0.0 - 1.0$)

The thrust $T$ of the rotor is proportional to the square of the rotor angular velocity $\Omega$ eq.(2.50). $A$ is a parameter related to the rotor size and the air density.

$T = A \Omega^2 $

The anti-torque $\tau_i$ of the rotor (2.56).
 
$\tau_i = B \Omega^2 + Jr \dot{\Omega}$

where $B$, $Jr$ is parameters related to the rotor properties. This makes the drone rotate around the $z$-axis.

## Experiments

We connected Hakoniwa to PX4 SITL simulator and tested the library with the following experiments.
The architecture of the simulation is described here.

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/2ffd9cd6-e8b4-4c75-9328-69c6a8aebba4)


Mission:
- Lift off and hover at height 10m.
- Move to the right 10m.

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/afa89bfd-e873-4cee-b4f1-606c6fed409e)

<img width="1072" alt="image" src="https://github.com/toppers/hakoniwa-px4sim/assets/1093925/ef02a826-4ba8-4dbb-86d2-090b9de1919e">




## Tests

utest.cpp has unit tests for the functions.

## Implementation Policy
- All the functions are implemented in standard C++17.
- No external libraries are used other than ones in the std:: namespace.
- Implemented as functions, not classes. Meaning stateless.

## Acknowledgement

I thank Dr. Nonami for writing the detailed description of the math around the drone development.
And also I thank @tmori for leading this Hakoniwa project.
