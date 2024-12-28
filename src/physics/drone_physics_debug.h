#ifndef _DRONE_PHYSICS_DEBUG_H_
/* Only for test files in this directory. User of this library do not use this. */

const double almost_tolerance = 0.0001;

#if defined(__cplusplus)
#include <iostream>
#include <cassert>
#include <cmath>
#include "drone_physics.hpp"

inline double diff(double a, double b) {
    return (a-b)*(a-b);
}

inline double diff(const hako::drone_physics::VectorType& v, const hako::drone_physics::VectorType& w) {
    return length_squared(v - w);
}

inline double diff(const hako::drone_physics::EulerType& a, const hako::drone_physics::EulerType b) {
    auto [phi, theta, psi] = a;
    auto [phi2, theta2, psi2] = b;
    return (phi-phi2)*(phi-phi2) + (theta-theta2)*(theta-theta2) + (psi-psi2)*(psi-psi2);
}
inline double diff(const hako::drone_physics::QuaternionType& a, const hako::drone_physics::QuaternionType& b) {
    auto [w, x, y, z] = a;
    auto [w2, x2, y2, z2] = b;
    auto d = (w-w2)*(w-w2) + (x-x2)*(x-x2) + (y-y2)*(y-y2) + (z-z2)*(z-z2);
    auto d2 = (w+w2)*(w+w2) + (x+x2)*(x+x2) + (y+y2)*(y+y2) + (z+z2)*(z+z2); // alow negation
    return std::min(d, d2);
}

static int AssertCount = 0;
#define assert_almost_equal(a, b) (++AssertCount, (diff((a), (b)) < almost_tolerance) ? true : (std::cerr << std::endl << "Failed: " #a "=" << (a) << "<=!!!!=>" #b "=" << (b) << std::endl, assert((diff((a), (b)) < almost_tolerance)), false))

#define print_vec(v) std::cerr << #v "=" << v << std::endl

/* for testing. use like T(test_func_name). */
#define T(f) do {std::cerr<< #f ; f(); std::cerr << "... PASS" << std::endl;} while(false)

#define END_TEST() do {std::cerr << "All(" << AssertCount << ")asserts passed." << std::endl;} while(false)

#else /* __cplusplus */

/* C lang section */
#include "drone_physics_c.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

static inline double diff(const dp_vector_t* v, const dp_vector_t* w) {
    double x = v->x;
    double y = v->y;
    double z = v->z;
    double x2 = w->x;
    double y2 = w->y;
    double z2 = w->z;
  
    return (x-x2)*(x-x2) + (y-y2)*(y-y2) + (z-z2)*(z-z2);
}

/* TOBE INCLUDED LATERS. just to erase compile -unused warning.
static double diff_a(const dp_euler_t* a, const dp_euler_t* b) {
    double phi = a->phi;
    double theta = a->theta;
    double psi = a->psi;
    double phi2 = b->phi;
    double theta2 = b->theta;
    double psi2 = b->psi;
    return (phi-phi2)*(phi-phi2) + (theta-theta2)*(theta-theta2) + (psi-psi2)*(psi-psi2);
}
*/

/** Extract some tests from utest.cpp, via C interface */

static int AssertCount = 0;

#define assert_almost_equal(a, b) \
    (++AssertCount, (diff(&(a), &(b)) < almost_tolerance) ?  1 : (print_vec(a), fprintf(stderr, "<=!!!!=>"), print_vec(b), fprintf(stderr, "!!\n")), assert ((diff(&(a), &(b)) < almost_tolerance)))

#define print_vec(v) \
    fprintf(stderr, "%s=(%g,%g,%g)", #v, v.x, v.y, v.z)

#define print_ang(a) \
    fprintf(stderr, "%s=(%g r,%g r,%g r)", #a, a.phi, a.theta, a.psi)

#define T(f) do {fprintf(stderr, #f " "); f(); fprintf(stderr, "... PASS\n");} while(0)

#define END_TEST() do {fprintf(stderr, "All(%d))asserts passed.\n", AssertCount);} while(0)


#endif /* __cplusplus */

#endif /* _DRONE_PHYSICS_DEBUG_H_ */