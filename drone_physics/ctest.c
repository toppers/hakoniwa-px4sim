#include "drone_physics_c.h"
#include <stdio.h>

int main() {
    dp_velocity_t v = {1, 2, 3};
    dp_angle_t a = {0, 0, 0};
    dp_velocity_t v2 = dp_velocity_body_to_ground(&v, &a);
    printf("v2 = (%f, %f, %f)\n", v2.x, v2.y, v2.z);
    return 0;
}