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

    // another way to get the x,y,z components using std::get
    double u2 = std::get<0>(body_velocity2);
    double v2 = std::get<1>(body_velocity2);
    double w2 = std::get<2>(body_velocity2);
    std::cout << "u2 = " << u2 << ", v2 = " << v2 << ", w2 = " << w2 << std::endl;
    // output: u2 = 100, v2 = 200, w2 = 300, back again.
}