#ifndef _PRISM_MATH_RAY_
#define _PRISM_MATH_RAY_

#include <math/vector.h>

typedef struct
{
    Vec3 org;
    Vec3 dir;
    double tnear;
    double tfar;
    double time;
} Ray;

#endif // _PRISM_MATH_RAY_