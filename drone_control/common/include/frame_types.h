#ifndef _FRAME_TYPES_HPP_
#define _FRAME_TYPES_HPP_

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define RPM2EULER_RATE(v)   ( ((v) * 2 * M_PI) / 60.0 )
#define TWO_PI (2.0 * M_PI)
#define NORMALIZE_RADIAN(x) (fmod((x), TWO_PI))
#define ALMOST_EQUAL(target, current, range) ( ( (current) >= ((target) - (range)) ) &&  ( (current) <= ((target) + (range)) ) )


struct EulerType {
    double phi;   // rotation round x-axis
    double theta; // rotation round y-axis
    double psi;   // rotation round z-axis
};
typedef EulerType EulerRateType;

struct VectorType {
    double x, y, z;
};
typedef VectorType PositionType;
typedef VectorType VelocityType;
/* angular VECTORS omega=(p,q,r) in x, y, z , NOT PHI, THTEA, PSI */
typedef VectorType AngularVectorType;
typedef AngularVectorType AngularVelocityType;


#endif /* _FRAME_TYPES_HPP_ */