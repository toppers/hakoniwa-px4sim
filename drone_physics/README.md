English ｜ [日本語](README-ja.md)

# Hakoniwa Drone Physics Library(math, physics and dynamics)

## What is this ?

This is a math, physics, and dynamics library for the drone plant model in the Hakoniwa project(Open Source Runtime Environment for Simulating Cyber-Physical Systems).

You can transform vectors between the ground and the body frame coordinate system, and also calculate the drone's speed, acceleration from the rotors' thrust and gravity.

It is first designed for Hakoniwa px4sim project, but I found it was more general, so
made it a separate library with more user-friendly interface, and also added the reference to the equations in the book below.

Most of the functions are implemented based on the equations in the following book:

- ["Introduction to Drone Engineering" by Dr. Kenzo Nonami(Japanese)](https://www.coronasha.co.jp/np/isbn/9784339032307/)

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/c92d3d96-25f9-4b6a-ae4e-25d898b75a28)

All the functions are implemented in C++, with the equation numbers in the book as comments in the sourde code. The coverage is not enough but the drone in the project started flying with this library, 
although not all the equations in the book are not implemented yet.

I hope this can be a reference implmentation for the basic drone dynamics.

## Hello World

### In C++

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

### In C

```C
#include <stdio.h>

// include the library header
#include "drone_physics_c.h"

int main() {
    // create a body frame from Euler angles. dp_ is the prefix for this lib.
    dp_angle_t frame = {0, 0, M_PI/2};
    dp_velocity_t body_velocity = {100, 200, 300};
    
    // Convert the body velocity to the ground frame.
    dp_velocity_t g = dp_velocity_body_to_ground(&body_velocity, &frame);

    // get the x,y,z components of the velocity.
    printf("x=%g, y=%g, z=%g\n", g.x, g.y, g.z);
    // output: x = 200, y = -100, z = 300

    // you can also use explicit initialization.
    // reverse the conversion to the body frame.
    dp_velocity_t b = dp_velocity_ground_to_body(
        &g, &(dp_angle_t){0, 0, M_PI/2}
    );

    // get the x,y,z components of the velocity.
    printf("x=%g, y=%g, z=%g\n", b.x, b.y, b.z);
    // output: x = 100, y = 200, z = 300, back again.

    return 0;
}
```

## Installation

Copy this whole directory to your project.
There is `CMkakeLists.txt`, use CMake to build.

```bash
$ cmake .
$ make
```

- The C++ library is built as `libdrone_physics.a`.
- The C library is built as `libdrone_physics_c.a`.
- Test programs `utest` `ctest` `examples` `cexamples` are also built as unit tests and examples.

I your programs,

- In C++, include `drone_physics.hpp` into your C++ code and link with libdrone_physics.a.
- In C, include `drone_physics_c.h` into your C code and link with libdrone_physics_c.a.

See `examples.cpp`, `utest.cpp` for more examples in C++,
and `cexamples.c`, `ctest.c` for more examples in C.

## List of functions

Functions are implemented in the following categories, with the referece to the book.

### Frame conversion:
| Function | equation | note |
|----------|-----------|------|
|`velocity_body_to_ground`  | (1.71), (1.124) | Body velocity to ground velocity |
|`velocity_ground_to_body`  | (1.69), inverse of (1.124) | Ground velocity to body velocity |
|`angular_velocity_body_to_ground` | (1.109) | Body angular velocity to ground angular velocity |
|`angular_velocity_ground_to_body` | (1.106) | Ground angular velocity to body angular velocity |

### Body dynamics(Acceleration):
| Function | equations in the book | note |
|----------|-----------|------|
|`acceleration_in_body_frame` | (1.136),(2.31) | Acceleration in body frame |
|`angular_acceleration_in_body_frame` | (1.37),(2.31) | Angular acceleration in body frame |

### Rotor dynamics(1 rotor, rotation speed and thrust):
| Function | equations in the book | note |
|----------|-----------|------|
|`rotor_omega_acceleration` | (2.48) | Rotor angular velocity acceleration from dury rate |
|`rotor_thrust` | (2.50) | Rotor thrust from rotor angular velocity |
|`rotor_anti_torque` | (2.56) | Rotor anti-torque from rotor thrust. this makes z-axis rotation. |

### Body dynamics(n rotors, thrust and torque to the body):
| Function | equations in the book | note |
|----------|-----------|------|
|`body_thrust` | (2.61) | Sum of the $n$ trust from the rotors |
|`body_torque` | (2.60)-(2.62) | Sum of the torques from the $n$ rotors based on the positionings of them |

## Equations

The ground frame coordinate system fixed to the ground is defined by right hand rule,
in which $z$-axis is downward.

The body frame coordinate system is defined by right hand rule, in which $x$-axis is the front of the drone, $y$-axis is the right side of the drone, and $z$-axis is the bottom of the drone.

The origin of the body frame is the center of gravity of the drone. The body frame is attached to the drone, and the body frame moves with the drone.

The rotation order from ground to body is $z$-axis($\psi$), $y$-axis($\theta$) and $x$-axis($\phi$). 

### Body dynamics

The basic dynamics equations in the body frame are as follows eq.(2.31).

$$
\begin{array}{l}
m \dot{v} + \omega \times m v = F \\
I \dot{\omega} + \omega \times I \omega = \tau
\end{array}
$$

where;

- $m$ - mass of the drone
- $I$ - inertia matrix of the drone 
- $v$ - linear velocity of the drone $v=(u, v, w)$
- $\omega$ - angular velocity of the drone $\omega = (p, q, r)$
- $F$ - force vector including gravity($mg$), drag($-dv)$, and thrust $(T)$
- $\tau$ - torque vector from the rotors

The body frame dynamics above are expanded based on the body angles $\phi, \theta, \psi$ as follows.

$$
\begin{array}{l}
\dot{u} = -g \sin{\theta} -(qw -rv) -\frac{d}{m}u \\
\dot{v} = g \cos{\theta}\sin{\phi} -(ru -pw) -\frac{d}{m}v \\
\dot{w} = -\frac{T}{m} + g \cos{\theta}cos{\phi} -(pv-qu)-\frac{d}{m}w \\
\dot{p} = (\tau_{\phi} -qr(I_{zz}-I_{yy}))/I_{xx} \\
\dot{q} = (\tau_{\theta}-rp(I_{xx}-I_{zz}))/I_{yy} \\
\dot{r} = (\tau_{\psi}-pq(I_{yy}-I_{xx}))/I_{zz} \\
\end{array}
$$

where;

- $g$ is the acceleration due to gravity.
- $\phi, \theta, \psi$ are the roll($x$-axis), pitch($y$-axis) and yaw($z$-axis) angles, respectively.
- $d$ is the air friction coefficient called "drag", affecting the velocity terms.
- $I_{xx}​,I_{yy}, I_{zz}$ are inertia moments around the body-frame axes $x, y, z$ respectively($x, y, z$ should be aligned with the drone's principal axes, from the gravity-center(other slant inertia $I_{xy}, I_{yz}, I_{zx}$ are assumed to be zero).

The transformation from/to the body frame and the ground frame is as follows.

### Frame transformation

#### Velocity, Acceleration

The body velocity $v = (u, v, w)$ is transformed to ground $v_e = (u_e, v_e, w_e)$ by the following matrix.

$$
\left[
  \begin{array}{c}
   u_e \\
  v_e \\
  w_e \end{array}
\right] =
  \begin{bmatrix}
    \cos\theta\cos\psi & \sin\phi\sin\theta\cos\psi - \cos\phi\sin\psi & \cos\phi\sin\theta\cos\psi + \sin\phi\sin\psi \\
    \cos\theta\sin\psi & \sin\phi\sin\theta\sin\psi + \cos\phi\cos\psi & \cos\phi\sin\theta\sin\psi -\sin\phi\cos\psi \\
    -\sin\theta & \sin\phi\cos\theta & \cos\phi\cos\theta
  \end{bmatrix}
\left[
  \begin{array}{c} u \\
  v \\
  w \end{array}
\right]
$$


#### Angular Velocity, Angular Acceleration

The body angular velocity $\omega = (p, q, r)$ 
is transformed to ground($\omega_e = (p_e, q_e, r_e$).
From the body to the ground, the transformation matrix is;

$$
\begin{bmatrix}
   p_e \\ 
   q_e \\ 
   r_e
\end{bmatrix} =
  \begin{bmatrix}
1 & \sin \phi \tan \theta & \cos \phi \tan \theta \\ 
0 & \cos \phi & -\sin \phi \\
0 & \sin \phi \sec \theta & \cos \phi \sec \theta
\end{bmatrix}
\begin{bmatrix}
    p \\ 
    q \\ 
    r
\end{bmatrix}
$$

### Rotor dynamics

Each rotor can be modeled as a first-order lag system, in which the rotor angular velocity
$\Omega(t)$ is controlled by the duty rate $d(t)$, described as transfer function G(s)
eq.(2.48) in the book,

$G(s)/D(s) = K_r/(T_r s + 1)$

and the time domain differential equation is as follows.

$\dot{\Omega}(t) = K_r ( d(t) - \frac{\Omega(t)}{ T_r})$

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

`utest.cpp` has unit tests for all the functions. It is not easy to read, but you can use it as a reference.
`ctest.c` has C interface tests.

## Implementation Policy
- All the functions are implemented in standard C++17.
- No external libraries are used other than ones in the std:: namespace.
- Implemented as functions, not classes. Meaning stateless.

## Acknowledgement

I thank Dr. Nonami for writing the detailed description of the math around the drone development.
And also I thank （[@tmori](https://github.com/tmori)）for connecting Hakoniwa to
PX4, QGroundControl, and Unity, and spending a long time testing the drone flight virtually, and also 
for leading this whole Hakoniwa project.
