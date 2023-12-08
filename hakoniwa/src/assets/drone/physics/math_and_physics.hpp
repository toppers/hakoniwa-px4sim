#ifndef MATH_AND_PHYSICS_HPP_
#define MATH_AND_PHYSICS_HPP_
#include <cmath>
#include <tuple>

typedef std::tuple<double, double, double> 
PositionType,  VelocityType, AccelerationType,
AngleType, AngularVelocityType, AngularAccelerationType;

/* maths */
/* think about the sin/cos cache later... check working right first */

VelocityType velocity_body_to_ground(VelocityType body, AngleType angle);
VelocityType velocity_ground_to_body(VelocityType ground, AngleType angle);

AngularVelocityType angular_velocity_body_to_ground(AngularVelocityType angular_veleocy, AngleType angle);
AngularVelocityType angular_velocity_ground_to_body(AngularVelocityType angular_velocity_ground_frame, AngleType angle);

/* physics */

AccelerationType Acceleration_in_body_frame(VelocityType body, AngleType angle, double thrust, double mass);

AngularAccelerationType Angular_acceleration_in_body_frame(AngularVelocityType angular_velocity, AngleType angle, double trust, double mass, double inertia_x, double inertia_y, double inertia_z);

#endif /* MATH_AND_PHYSICS_HPP_ */
