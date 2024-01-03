#include "drone_physics_c.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

/** Extract some tests from utest.cpp, via C interface */

#define assert_almost_equal(a, b) \
    assert(fabs(a.x - b.x) < 0.0001 || fprintf(stderr, "%s=%g,%s=%g\n", #a, a.x, #b, b.x)); \
    assert(fabs(a.y - b.y) < 0.0001 || fprintf(stderr, "%s=%g, %s=%g\n", #a, a.y, #b, b.y)); \
    assert(fabs(a.z - b.z) < 0.0001 || fprintf(stderr, "%s=%g, %s=%g\n", #a, a.z, #b, b.z));

#define print_vec(v) \
    fprintf(stderr, "%s=(%g,%g,%g)\n", #v, v.x, v.y, v.z);

#define T(f) do {fprintf(stderr, #f); f(); fprintf(stderr, "... PASS\n");} while(0)

const double PI = M_PI;

void test_frame_all_unit_vectors_with_some_angles() {
    const dp_velocity_t v1 = {1, 0, 0};
    dp_velocity_t v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){0, 0, 0});
    assert_almost_equal(v1, v2);

    for (int i = 0; i < 360; i++) {  // 0 to 360 degree x-axis
        v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){i * PI / 180, 0, 0});
        assert_almost_equal(v1, v2);
    }

    v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){0, PI/6, 0}); // 30 degree head up
    assert_almost_equal(((dp_vector_t){sqrt(3)/2, 0, -0.5}), v2);

    v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){0, PI/3, 0}); // 60 degree
    assert_almost_equal(((dp_angle_t){0.5, 0, -sqrt(3)/2}), v2);

    v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){0, 0, PI/6}); // yaw 30 degree
    assert_almost_equal(((dp_angle_t){sqrt(3)/2, 0.5, 0}), v2);
}

void test_frame_roundtrip() {
    dp_velocity_t v1 = {1, 0, 0};
    dp_velocity_t v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){PI/6, PI/4, 0});
    dp_velocity_t v3 = dp_velocity_ground_to_body(&v2, &(dp_angle_t){PI/6, PI/4, 0});
    assert_almost_equal(v1, v3);

    v1 = (dp_velocity_t){0, 1, 0};
    v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){PI/6, PI/3, PI/2});
    v3 = dp_velocity_ground_to_body(&v2, &(dp_angle_t){PI/6, PI/3, PI/2});
    assert_almost_equal(v1, v3);

    v1 = (dp_velocity_t){0, 0, 1};
    v2 = dp_velocity_body_to_ground(&v1, &(dp_angle_t){PI/8, -PI/3, PI/6});
    v3 = dp_velocity_ground_to_body(&v2, &(dp_angle_t){PI/8, -PI/3, PI/6});
    assert_almost_equal(v1, v3);
}


int main() {
//    T(test_frame_all_unit_vectors_with_angle0);
    T(test_frame_all_unit_vectors_with_some_angles);
    T(test_frame_roundtrip);
    dp_velocity_t v = {1, 2, 3};
    dp_angle_t a = {0, 0, 0};
    dp_velocity_t v2 = dp_velocity_body_to_ground(&v, &a);
    print_vec(v2)
    return 0;
}