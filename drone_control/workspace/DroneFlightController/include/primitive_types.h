#ifndef _PRIMITIVE_TYPES_H_
#define _PRIMITIVE_TYPES_H_

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

#endif /* _PRIMITIVE_TYPES_H_ */