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
    VelocityType ground_velocity = ground_vector_from_body(body_velocity, frame);

    // get the x,y,z components of the velocity.
    auto [u, v, w] = ground_velocity;

    std::cout << "u = " << u << ", v = " << v << ", w = " << w << std::endl;
    // output: u = 200, v = -100, w = 300

    // reverse the conversion to the body frame.
    VelocityType body_velocity2 = body_vector_from_ground(
        {u, v, w},
        {0, 0, M_PI/2}
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
    dp_euler_t frame = {0, 0, M_PI/2};
    dp_velocity_t body_velocity = {100, 200, 300};
    
    // Convert the body velocity to the ground frame.
    dp_velocity_t g = dp_ground_vector_from_body(&body_velocity, &frame);

    // get the x,y,z components of the velocity.
    printf("x=%g, y=%g, z=%g\n", g.x, g.y, g.z);
    // output: x = 200, y = -100, z = 300

    // you can also use explicit initialization.
    // reverse the conversion to the body frame.
    dp_velocity_t b = dp_body_vector_from_ground(
        &g, &(dp_euler_t){0, 0, M_PI/2}
    );

    // get the x,y,z components of the velocity.
    printf("x=%g, y=%g, z=%g\n", b.x, b.y, b.z);
    // output: x = 100, y = 200, z = 300, back again.
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

- In C++, include `drone_physics.hpp` into your C++ code and link with `libdrone_physics.a`.
- In C, include `drone_physics_c.h` into your C code and link with `libdrone_physics_c.a`.

See `examples.cpp`, `utest.cpp` for more examples in C++,
and `cexamples.c`, `ctest.c` for more examples in C.

## Type list

### Vectors
`VectorType` is a 3-dimensional vector, used in both the ground frame and the body frame. The following subtypes are available.

- `VelocityType` - Velocity
- `AccelerationType` - Acceleration
- `ForceType` - Force
- `TorqueType` - Torque

### Angular velocities
- `AngularVelocityType` - Angular velocity
- `AngularAccelerationType` - Angular acceleration

### Euler angles
`EulerType` is a 3-dimensional vector, used in transformation between the ground frame and the body frame. The following subtypes are available.
Note that the euleer angles are not vectors, and cannot be added, scaled, or multiplied by matrices. The following subtypes are available.

- `EulerType` - Euler angles
- `EulerRateType` - Change rate of the Euler angles
- `EulerAccelerationType` - Acceleration of the Euler angles(2nd order differential)

## List of functions
Functions(C++) are implemented in the following categories, with the referece to the book.
As our naming policy, the function names represent the output of the function(omitting get_ prefix)
and describe the source using _from if needed, and the input as the arguments, (maybe) 
overly using function overloading by argument types.
None of the functions have states(static variable) nor side effects.
All the arguments are passed by value(or const reference) and won't be changed in the function.

### Frame conversion:
| Function | equation | note |
|----------|-----------|------|
|`ground_vector_from_body`  | (1.71), (1.124) | Body velocity to ground velocity |
|`body_vector_from_ground`  | (1.69), inverse of (1.124) | Ground velocity to body velocity |
|`euler_rate_from_body_angular_velocity` | (1.109) | Body angular velocity to euler rate |
|`body_angular_velocity_from_euler_rate` | (1.106) | Euler rate to body angular velocity |
|`euler_from_quaternion` | (1.66) | Quaternion to Euler angles |
|`quaternion_from_euler` | (1.74)-(1.77) | Euler angles to Quaternion |
|`quaternion_velocity_from_angular_velocity` | (1.86)(1.87) | Angular velocity to Quaternion velocity |


### Body dynamics(Acceleration):
| Function | equations in the book | note |
|----------|-----------|------|
|`acceleration_in_body_frame` | (1.136),(2.31) | Acceleration in body frame by force |
|`angular_acceleration_in_body_frame` | (1.137),(2.31) | Angular acceleration in body frame by force |
|`acceleration_in_ground_frame` | (2.46), (2.47) | Acceleration in ground frame by torque |
|`euler_acceleration_in_ground_frame` | differential of (1.109) | Euler acceleration by torque(not used) |



### Rotor dynamics(for one rotor, rotation speed and thrust):
| Function | equations in the book | note |
|----------|-----------|------|
|`rotor_omega_acceleration` | (2.48) | Rotor angular rate acceleration from dury rate (linear and non-linear versions) |
|`rotor_thrust` | (2.50) | Rotor thrust from rotor angular rate($-z$ direction) |
|`rotor_anti_torque` | (2.56) | Rotor anti-torque from rotor thrust($z$ -axis rotation) |
|`rotor_current` |  | Rotor current |

### Body dynamics(n rotors, thrust and torque to the body):
| Function | equations in the book | note |
|----------|-----------|------|
|`body_thrust` | (2.61) | Sum of the $n$ trust from the rotors |
|`body_torque` | (2.60)-(2.62) | Sum of the torques from the $n$ rotors based on the positionings of them |

There are C language interfaces for all the functions above, with the prefix `dp_` for "drone physics".

## Equations
The ground frame coordinate system fixed to the ground is defined by right hand rule,
in which $x$-axis is north, $y$-axis is east, and $z$-axis is down(NED: North-East-Down).
The body frame coordinate system is defined by right hand rule, in which $x$-axis is the front of the drone, $y$-axis is the right side of the drone, and $z$-axis is the bottom of the drone(FRB: Front-Right-Botton).

![body_ground_frame](body_ground_frame.png)

The origin of the body frame is the center of gravity of the drone. The body frame is attached to the drone, and the body frame moves with the drone. And the origin of the ground frame is assumed to be the same as the origin of the body frame.
This ground frame is paticularly called the "Vehicle Carried NED".

In the equations below, we use the ground frame(Vehicle Carried NED) and
the body frame in calculating the drone dynamics.

This is reasonable because the equations do not contain the position variables,
and the position is obtained in the last step by integrating the velocity in the ground frame.

The rotation order from the ground frame to the body frame
is $z$-axis($\psi$), $y$-axis($\theta$) and $x$-axis($\phi$),
so that the ground frame axies are rotated to be aligned with the body frame.

Note $\phi, \theta, \psi$ are the bridge across the two frames and the same values are used in both frames.
In other words, the angles are the same values in the equations of the ground frame and the body frame(not to be converted between one another).

The most basic Newtonian(translation) and Euler(rotational) equations of motion
in the ground frame are as follows,
where the subscript $e$ denotes the ground(earth) frame.

$$
\begin{array}{l}
m \dot{v_e} &= F_e \\
I \dot{\omega_e}  &= \tau_e
\end{array}
$$


The basic dynamics equations in the body frame are as follows eq.(2.31).

$$
\begin{array}{l}
m \dot{v} &+ \omega \times m v &= F \\
I \dot{\omega} &+ \omega \times I \omega &= \tau
\end{array}
$$

where;

- $m$ - mass of the drone
- $I$ - inertia matrix of the drone 
- $v$ - linear velocity of the drone $v=(u, v, w)$
- $\omega$ - angular rate of the drone $\omega = (p, q, r)$
- $F$ - force vector including gravity($mg$), drag($-dv)$, and thrust $(T)$
- $\tau$ - torque vector from the rotors

In the first equation, the second term is the inertial force(Coriolis force), wihch is the force due to the angular velocity of the body frame. The centrifugal force does not occur because the origin of the body frame is at the center of gravity.

In the second equation, the second term is the inertial torque(gyro effect), which is the torque due to the angular velocity of the body frame.

The body frame dynamics above are expanded based on the body angles $\phi, \theta, \psi$ as follows.

### Body frame dynamics
Here is the body frame dynamics equations used in this library.
All the state variables $(u,v,w,p,q,r,\phi,\theta,\psi)^T$ and their time-derivatives
$(\dot{u},\dot{v},\dot{w},\dot{p},\dot{q},\dot{r},\dot{\phi},\dot{\theta},\dot{\psi})^T$
generated by the force and torque are calculated by using these equations and the frame transformations below.

Letting $x$ be the state vector of the drone, the equation as a contrrol system is as follows(unfortunately, this is not a linear system).

$$
x = (u, v, w, p, q, r, \phi, \theta, \psi)^T
$$

The time-derivative of $x$ is described by the function of $x, F, \tau$ as follows.

$$
\dot{x} = f(x, F, \tau)
$$

Unfortunately the function $f$ is not linear, but we can draw the trajectory of 
$x$ using numerical computation.

In the program, finally the body velocity $(u, v, w)^T$ transformed to the ground frame $(u_e, v_e, w_e)^T$
is time-integrated to get the body position $(x, y, z)^T$ which is described in the transformation section.

And the body angular velocity $(p,q,r)^T$ transformed to the euler
rate $(\dot{\phi}, \dot{\theta}, \dot{\psi})^T$ is time-integrated to get
the euler angles $(\phi, \theta, \psi)^T$ which is the body attitude, also
described in the transformation section.

The last part above can alternatively be implemented by quaternion, which is more stable than the euler angles.
We will describe the quaternion-based implementation in the later section.

![plant-model](plant-model.png)

#### Velocity and Acceleration(linear translation)

$$
\begin{array}{llll}
\dot{u} = & &-g \sin{\theta} &-(qw -rv) &-\frac{d}{m}u \\
\dot{v} = & &+g \cos{\theta}\sin{\phi} &-(ru -pw) &-\frac{d}{m}v \\
\dot{w} = &-\frac{T}{m} &+ g \cos{\theta} \cos{\phi} &-(pv-qu) &-\frac{d}{m}w
\end{array}
$$

The function name: `acceleration_in_body_frame`.

#### Angular velocity and Angular Acceleration(rotation)

$$
\begin{array}{l}
\dot{p} = (\tau_{\phi} -qr(I_{zz}-I_{yy}))/I_{xx} \\
\dot{q} = (\tau_{\theta}-rp(I_{xx}-I_{zz}))/I_{yy} \\
\dot{r} = (\tau_{\psi}-pq(I_{yy}-I_{xx}))/I_{zz} 
\end{array}
$$

The function name: `angular_acceleration_in_body_frame`.

where;

- $m$ - mass of the drone($m>0$).
- $g$ - acceleration due to gravity($g>0$).
- $(u, v, w)^T$ - velocity of the drone in the body frame.
- $(p, q, r)^T$ - angular velocity of the drone in the body frame.
- $d$ - air friction coefficient "drag", affecting the velocity terms(d>0).
- $(\phi, \theta, \psi)^T$ - roll($x$-axis), pitch($y$-axis) and yaw($z$-axis) euler angles.
- $I_{xx}​,I_{yy}, I_{zz}$ are inertia moments around the body-frame axes $x, y, z$ respectively. ($x, y, z$) should be aligned with the drone's principal axes, from the gravity-center(other slant inertia $I_{xy}, I_{yz}, I_{zx}$ are assumed to be zero).

## Ground frame dynamics
The ground frame dynamics of the translational motion is as follows.
Note that the fraction of the air is also assumed to be the same in the three direction.
The rotational motion in the ground frame are not calculated in this library
because time-varying inertia is too complex(to me) in the ground frame.

$$
\begin{array}{llll}
\dot{u_e} = &-\frac{T}{m}(\cos{\phi}\sin{\theta}\cos{\psi} + \sin{\psi}\sin{\phi}) & &- \frac{d}{m}u_e \\
\dot{v_e} = &-\frac{T}{m}(\cos{\phi}\sin{\theta}\sin{\phi} - \sin{\phi}\cos{\psi}) & &-\frac{d}{m}v_e \\
\dot{w_e} = &-\frac{T}{m}(\sin{\phi}\cos{\theta})                  &+g              &-\frac{d}{m}w_e 
\end{array}
$$

The function name is `acceleration_in_ground_frame`.

The transformations from/to the body frame and the ground frame are as follows.

### Frame transformation
The dynamics above are calculated using the transformations between the ground frame and the body frame.

#### Velocity, Acceleration
The body velocity $v = (u, v, w)^T$ is transformed to ground $v_e = (u_e, v_e, w_e)^T$ by the following matrix. The acceleration is transformed in the same way. Any vectors in the body frame can be transformed to the ground frame by this matrix, including acceleration, angular velocity,
angular acceleration, force, and torque(not including the euler angle which is NOT a vector).

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

This matrix is called the direction cosine matrix(DCM) and is the product of three rotation matrices in the order $R_z(\psi)R_y(\theta)R_x(\phi)$.

DCM is always orthogonal and has a '1' as its eigenvalue.
The direction of the eigenvector corresponding to it is the direction of the rotation,
which is the quaternion's imaginary part. This makes sense that the DCM is a rotation matrix
around the eigenvector with the eigenvalue 1(length preserving).

$$
R_z(\psi) = \begin{bmatrix}
    \cos\psi & -\sin\psi & 0 \\
    \sin\psi & \cos\psi & 0 \\
    0 & 0 & 1
  \end{bmatrix}, \quad
R_y(\theta) = \begin{bmatrix}
    \cos\theta & 0 & \sin\theta \\
    0 & 1 & 0 \\
    -\sin\theta & 0 & \cos\theta
  \end{bmatrix}, \quad
R_x(\phi) = \begin{bmatrix}
    1 & 0 & 0 \\
    0 & \cos\phi & -\sin\phi \\
    0 & \sin\phi & \cos\phi
  \end{bmatrix}
$$

The function name is `ground_vector_from_body`,
and the inverse transformation is `body_vector_from_ground`.

##### Memo on linear algebra

The rotation matrix is a basis transformation matrix, which transforms the basis vectors of the original space to the basis vectors of the new space. 

$R_z(\psi), R_y(\theta), R_x(\phi)$ are the basis transformation matrices. For example, $R_z(\psi)$, which rotates the ground frame, namely, the basis $e_x, e_y, e_z$ around the z-axis.

$$
\begin{bmatrix} e_x' & e_y' & e_z' \end{bmatrix} = 
  \begin{bmatrix} e_x & e_y & e_z \end{bmatrix}
  \begin{bmatrix}
    \cos\psi & -\sin\psi & 0 \\
    \sin\psi & \cos\psi & 0 \\
    0 & 0 & 1
  \end{bmatrix} = \begin{bmatrix} e_x & e_y & e_z \end{bmatrix}
R_z(\psi)
$$

Focusing on the new $e_x'$, it can be expressed using the old basis $e_x, e_y, e_z$ as follows (looking at the first column of the matrix).

$$
e_x' = (\cos\psi) e_x +(\sin\psi) e_y + (0)e_z
$$

Now, as a general theory of linear algebra, using the basis transformation matrix $R$, the basis transformation is expressed as follows:

$$
\begin{bmatrix} e_x' & e_y' & e_z' \end{bmatrix} =  \begin{bmatrix} e_x & e_y & e_z \end{bmatrix}R
$$

The old coordinates $r=(x, y, z)^T$ and the new coordinates $r'=(x', y', z')^T$ have the following relationship (the both sides of the equation represent the same vector).

$$
\begin{array}{l}
\begin{bmatrix} e_x' & e_y' & e_z' \end{bmatrix} 
\begin{bmatrix} x' \\
y' \\
z' \end{bmatrix} =
\begin{bmatrix} e_x & e_y & e_z \end{bmatrix}
\begin{bmatrix} x \\
y \\
z \end{bmatrix}
\end{array}
$$

From these two equations, the transformation equation for the coordinates is as follows (multiplying both sides of the first equation by $(x',y',z')^T$ from the right and using the second equation).

$$
\begin{array}{l}
\begin{bmatrix} x\\
y\\
z \end{bmatrix} &= R
\begin{bmatrix} x'\\
y'\\
z' \end{bmatrix} \\ \\
r &= R r'
\end{array}
$$

The right side is the product of the transformed "basis" and its "coordinate components", and the product doesn't change before and after the transformation.

Now, apply this to the three rotation matrices in this case, i.e., the transformation from the ground frame coordinate system to the body frame coordinate system, rotated ($\psi, \theta, \phi$) in this order.

$$
\begin{array}{l}
r &= R_z(\psi)r' \\
r' &= R_y(\theta)r'' \\
r'' &= R_x(\phi)r'''
\end{array}
$$

That is,

$$
\begin{array}{l}
r = R_z(\psi) R_y(\theta) R_x(\phi) r'''
\end{array}
$$

This becomes the transformation matrix (DCM) from the body frame coordinate system to the ground frame coordinate system.
(End of linear algebra memo)

- [Euler Angles and the Euler Rotation sequence(Christopher Lum)](https://github.com/clum/YouTube/blob/main/FlightMech07/lecture02c_euler_angles.pdf), [YouTube](https://youtu.be/GJBc6z6p0KQ)
- [Flight Dynamics in Body Axes Coordinate System(Japanese) by @mtk_birdman](https://mtkbirdman.com/flight-dynamics-body-axes-system)
- [Euler's Angles(Japanese) by Sky Engineering Laboratory Inc](https://www.sky-engin.jp/blog/eulerian-angles/)
- [3D rotation in Quaterinon by @kenjihiranabe](https://qiita.com/kenjihiranabe/items/945232fbde58fab45681)


#### Body angular velocity and Euler angles
The body angular rate $\omega = (p, q, r)$ 
is transformed to the euler angle rate $\dot{\omega} = (\dot{\phi}, \dot{\theta}, \dot{\psi})^T$ by the following matrix.

$$
\begin{bmatrix}
   \dot{\phi} \\ 
    \dot{\theta} \\
    \dot{\psi}
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

Note that this matrix is close to $I$(identity) when all the angles are small.

The function name is `euler_rate_from_body_angular_velocity` ,
and the inverse transformation is `body_angular_velocity_from_euler_rate`.

### Euler angles and Quaternion(new release 12/12/2024)

The attitude representation using Euler angles has a problem called "gimbal lock" 
when the pitch angle is 90 degrees (the nose is vertical), and there are two Euler angle representations to represent the same attitude.
For example, $(\phi, \theta, \psi) = (0,90 \degree, 0)$ and $(90 \degree, 90 \degree, 90 \degree)$ are the same attitude.
Both can represent the same attitude, but at the same time, one degree of freedom is lost, and the attitude cannot move continuously around
one axis from this attitude (gimbal lock).
Specifically, in our implementation `euler_rate_from_body_angular_velocity`, a zero division occurs due to $\cos \theta = 0$,
and the rate of change of the Euler angles cannot be obtained.

This problem does not occur in normal flight with a small pitch angle like a passenger plane,
but it becomes unstable when performing extreme movements such as drone racing or "aerobatics" in fighter jets.
To solve this problem, quaternions are used as the attitude representation instead of Euler angles.

In the implementation of the drone, the quaternion representing the attitude should be maintained internally
while always updating it, and converting it to Euler angles as needed.
But even if the attitude quaternion changes continuously, the Euler angles could jump discontinuously(the attitude is still correct in that case).

#### Quaternions and Euler angles conversion

Euler angles $(\phi, \theta, \psi)^T$ are converted to quaternions $(q_0, q_1, q_2, q_3)^T$ as follows(eq. 1.66).

$$
\begin{array}{l}
\begin{bmatrix} q_0\\  q_1\\ q_2\\ q_3 \end{bmatrix} = 
\begin{bmatrix}
\cos \frac{\psi}{2} \cos \frac{\theta}{2} \cos \frac{\phi}{2} + \sin \frac{\psi}{2} \sin \frac{\theta}{2} \sin \frac{\phi}{2}\\
\cos \frac{\psi}{2} \cos \frac{\theta}{2} \sin \frac{\phi}{2} - \sin \frac{\psi}{2} \sin \frac{\theta}{2} \cos \frac{\phi}{2}\\
\cos \frac{\psi}{2} \sin \frac{\theta}{2} \cos \frac{\phi}{2} + \sin \frac{\psi}{2} \cos \frac{\theta}{2} \sin \frac{\phi}{2}\\
\sin \frac{\psi}{2} \cos \frac{\theta}{2} \cos \frac{\phi}{2} - \cos \frac{\psi}{2} \sin \frac{\theta}{2} \sin \frac{\phi}{2}
\end{bmatrix}
\end{array}
$$

The function name is `quaternion_from_euler`.

The inverse conversion, quaternion to Euler angles, is as follows(eq. 1.74-1.77).
But the gimbal lock problem is not described in the book, and I referred to the article by [@aa_debdeb(Atsushi Asakura)](https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf).

$$
\begin{array}{l}
\begin{bmatrix} \phi\\  \theta\\ \psi \end{bmatrix} =
\begin{bmatrix}
\arctan \left(2(q_2 q_3 + q_0 q_1), 2(q_0^2 + q_3^2) - 1 \right)\\
\arcsin \left(2(q_0 q_2 - q_1 q_3) \right)\\
\arctan \left(2(q_1 q_2 + q_0 q_3), 2(q_0^2 + q_1^2) - 1 \right)
\end{bmatrix}
\end{array}
$$

$\arctan$ is calculated by the standard math library `std::atan2(y, x)`.
In the implementation, first $\theta$ is obtained, and if $\cos \theta$ becomes zero, the calculation method
is changed, and the solution with $\phi=0$ is obtained (the other solution with $\psi=0$ is also available).
That is, if $\cos \theta = 0$, it becomes as follows.

$$
\begin{array}{l}
\begin{bmatrix} \phi\\  \theta\\ \psi \end{bmatrix} =
\begin{bmatrix}
0 \\
\arcsin \left(2(q_0 q_2 - q_1 q_3) \right) \quad (\pm \pi/2) \\
\arctan \left(2(q_0 q_3 - q_1 q_2), 2(q_0^2 + q_1^2) - 1 \right)
\end{bmatrix}
\end{array}
$$

The function name is `euler_from_quaternion`.

#### time derivative of the quaternion

The time derivative of the quaternion is obtained from the angular velocity $(p, q, r)^T$ as follows(eq. 1.86, 1.87).

$$
\begin{array}{l}
\begin{bmatrix} \dot{q}_0\\  \dot{q}_1\\ \dot{q}_2\\ \dot{q}_3 \end{bmatrix} =
\begin{bmatrix}
0 & -p & -q & -r \\
p & 0 & r & -q \\
q & -r & 0 & p \\
r & q & -p & 0
\end{bmatrix} \begin{bmatrix} q_0\\  q_1\\ q_2\\ q_3 \end{bmatrix}
\end{array}
$$

The function name is `quaternion_velocity_from_angular_velocity`.

### One Rotor dynamics

There are two versions of the rotor dynamics in this library.
Note that the function name is the same for both versions, but the parameters are different
(using C++ funtion overlading).

Each rotor can be modeled as a first-order lag system, in which the rotor angular rate
is $\omega(t)$. We use variable name $\omega$ as the rotation speed in radian, in this section.
don't confuse with $\omega$ as the angular velocity of the whole body in other sections
(Nonami's book use capital $\Omega$ for this name,
but, we prioritize readability and use the lowercase $\omega(t)$ ).

#### One rotor rotation speed(1st-order lag system version)

$\omega(t)$ is is controlled by the duty rate $d(t)$, described as transfer function G(s)
eq.(2.48) in the book,

$\Omega(s)/D(s) = G(s) = K_r/(T_r s + 1)$

and the time domain differential equation is as follows.

$\dot{\omega}(t) = (K_r d(t) - \omega(t))/T_r$

where;

- $K_r$ - rotor gain constant.
- $T_r$ - rotor time constant.
- $d(t)$ - duty rate of the rotor. ($0.0 \le d(t) \le 1.0$)

This model is an approximation of the non-linear model described later, with two parameters $K_r, T_r$.
The function name is `rotor_omega_acceleration`.

#### One rotor rotation speed(non-linear using battery voltage version)
Another model is that the rotor angular rate $\omega(t)$ is controlled by the duty rate $d(t)$
times the battery voltage $V_{bat}$ .
The rotor is composed of a motor(torque generator by voltage) and a propeller(thrust generator using the torque).

See https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p2 eq (3)

$$
\begin{array}{l}
J \dot{\omega}(t) + D \omega(t) + C_q \omega(t)^2 &= K i(t) \\
L \dot{i}(t) + R i(t) + K \omega(t) &= e(t)
\end{array}
$$

where;
- $J$ - The inertia of the propeller. [ $kg m^2$ ]
- $D$ - The damping coefficient of the propeller. [ $N m s/rad$ ]
- $C_q$ - The torque coefficient of the propeller. [ $N m^2/s^2$ ]
- $L$ - The inductance of the motor. [ $H = Vs/A$ (Henry)]
- $R$ - The resistance of the motor. [ $\Omega$ (Ohm)]
- $K$ - The torque constant of the rotor. [ $N m/A$ ]
- $d(t)$ - The duty rate of the motor. ( $0.0 \le d(t) \le 1.0$ )
- $V_{bat}$ - The battery voltage. [ $V$ ]
- $e(t)$ - The voltage applied to the motor, equals to $V_{bat}d(t)$. [ $V$ ]
- $i(t)$ - The current of the motor. [ $A$ ]
- $\omega(t)$ - The angular velocity of the propeller. [ $rad/s$ ]

Neglecting the inductance $L$ which is very small, we have;

$$
i(t) = \frac{e(t) - K \omega(t)}{R}
$$

Then the differential equation for $\omega(t)$ is obtained as follows, by erasing the current $i(t)$.

$$
J \dot{\omega}(t) + (D + \frac{K^2}{R}) \omega(t) + C_q \omega(t)^2 = \frac{K}{R} e(t)
$$

Soving this differential equation for $\omega$ , we have the following result.

$$
\dot{\omega}(t) = \frac{e(t) - (K^2+DR) \omega(t) - C_q R \omega(t)^2}{JR}
$$

Let $e(t) = V_{bat} d(t)$, the differential equation of $\omega(t)$ for the input $d(t)$  is as follows.

$$
\dot{\omega}(t) = \frac{V_{bat}d(t) - (K^2+DR) \omega(t) - C_q R \omega(t)^2}{JR}
$$


The function name is (another version of)`rotor_omega_acceleration`(overloaded function name).

And the current $i(t)$ is obtained by the following equation.

$$
i(t) = (e(t) - K \omega(t))/R = (V_{bat} d(t) - K \omega(t))/R
$$

The function name is `rotor_current` ．

Note that when $\omega(t)$ gets larger by some external force, the current may flow back to the battery(charging the battery) by the back EMF(back electromotive force) of the motor.

#### One rotor thrust and anti-torque
The thrust $T$ of the rotor is proportional to the square of the rotor angular velocity
$\omega$ eq.(2.50). $C_T$ is the thrust coefficient, a parameter related to the propeller size and the air density
(In Nonami's book it is denoted as $A$).

$T = C_T \omega^2 $

The anti-torque $\tau_i$ of the rotor (2.56).
 
$\tau_i = C_q \omega^2 + J \dot{\omega}$

where $C_q$, $J$ is parameters related to the rotor properties.
Note that $J$ is the same as the one we used in the rotor propeller inertia.
This makes the drone rotate around the $z$-axis(In Nonami's book $C_q$ is denoted as $B$).

The function name is `rotor_thrust` and `rotor_anti_torque`.

## Overview of variables and functionson
The body location $(x, y, z)^T$ and the euler angles $(\phi, \theta, \psi)^T$ are placed in the
ground frame in the figure so to understand easily(which are the bridge between the two frames).

![archi](physics-architecture.png)

## Experiments
We connected Hakoniwa to PX4 SITL simulator and tested the library with the following experiments.
The architecture of the simulation is described here.

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/2ffd9cd6-e8b4-4c75-9328-69c6a8aebba4)


Mission:
- Lift off and hover at height 10m.
- Move to the right 10m.

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/afa89bfd-e873-4cee-b4f1-606c6fed409e)


![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/ef02a826-4ba8-4dbb-86d2-090b9de1919e)


## Tests
`utest.cpp` has unit tests for all the functions. It is not easy to read, but you can use it as a reference.
`ctest.c` has C interface tests.

## Implementation Policy
- All the functions are implemented in standard C++17.
- No external libraries are used other than ones in the std:: namespace.
- Implemented as functions, not classes. Meaning stateless.

## Acknowledgement
I thank Dr. Nonami for writing the detailed description of the math around the drone development. And Kohei Ito([@Kouhei_Ito](https://www.docswell.com/user/Kouhei_Ito)) for the detailed explanation of the drone dynamics in his blog and also kind replies to my questions.
And also I thank （[@tmori](https://github.com/tmori)）for connecting Hakoniwa to
PX4, QGroundControl, and Unity, and spending a long time testing the drone flight virtually, and also for leading this whole Hakoniwa project.

## References
A lot of good references are available on the web. I list some of them here.
- ["Introduction to Drone Engineering" by Dr. Kenzo Nonami(Japanese)](https://www.coronasha.co.jp/np/isbn/9784339032307/)
- [Model Based Control of UAV by Nonami(Japanese)](https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf)
- [Drone control Lecture by Seongheon Lee](https://github.com/SKYnSPACE/AE450/blob/master/Lec10/AE450_Lec10_Quadcopter_Dynamics_and_Control.pdf)
- [Euler's Equations of Motion(Japanese) by Sky Engineering Laboratory Inc](https://www.sky-engin.jp/blog/eulers-equations-of-motion/)
- [Euler's Angles(Japanese) by Sky Engineering Laboratory Inc](https://www.sky-engin.jp/blog/eulerian-angles/)
- [Flight Dynamics in Body Axes Coordinate System(Japanese) by @mtk_birdman](https://mtkbirdman.com/flight-dynamics-body-axes-system)
- [Basics of Multi-Copter(Japanese) by @Kouhei_Ito](https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343)
- [Euler Angles and the Euler Rotation sequence(Christopher Lum)](https://github.com/clum/YouTube/blob/main/FlightMech07/lecture02c_euler_angles.pdf), [YouTube](https://youtu.be/GJBc6z6p0KQ)
- - [Rotaion Matrix, Quaternion, and Euler Angle by Atsushi Asakura (@aa_debdeb)](https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf)
- [3D rotation in Quaterinon by @kenjihiranabe](https://qiita.com/kenjihiranabe/items/945232fbde58fab45681)
- [The art of Linear Algebra by @kenjihiranabe](https://github.com/kenjihiranabe/The-Art-of-Linear-Algebra/blob/main/The-Art-of-Linear-Algebra.pdf)

We have learned a lot from them and also the links are embedded in the source code comments. Thank you all !!


