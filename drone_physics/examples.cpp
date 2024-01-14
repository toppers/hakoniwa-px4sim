#include <iostream>
#include <cmath>

// include the library header
#include "drone_physics.hpp"

int main() {
    // the namespace of the library
    using namespace hako::drone_physics;

    // create a body frame from Euler angles.
    EulerType frame = {0, 0, M_PI/2};
    VelocityType body_velocity = {100, 200, 300};
    
    // Convert the body velocity to the ground frame.
    VelocityType ground_velocity = ground_vector_from_body(body_velocity, frame);

    // get the x,y,z components of the velocity.
    auto [u, v, w] = ground_velocity;

    std::cout << "u = " << u << ", v = " << v << ", w = " << w << std::endl;
    // output: u = 200, v = -100, w = 300

    // you can also use stuct explicit initialization.
    // reverse the conversion to the body frame.
    VelocityType body_velocity2 = body_vector_from_ground(
        VelocityType{u, v, w},
        EulerType{0, 0, M_PI/2}
    );

    // another way to get the x,y,z components.
    double u2 = body_velocity2.x;
    double v2 = body_velocity2.y;
    double w2 = body_velocity2.z;
    std::cout << "u2 = " << u2 << ", v2 = " << v2 << ", w2 = " << w2 << std::endl;
    // output: u2 = 100, v2 = 200, w2 = 300, back again.
}