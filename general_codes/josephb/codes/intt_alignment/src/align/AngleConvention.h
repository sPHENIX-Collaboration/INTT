#ifndef ANGLE_CONVENTION_H
#define ANGLE_CONVENTION_H

#include <math.h>
#include <iostream>

class AngleConvention
{
public:
	virtual int SetTransformFromAngles(double(&)[4][4], const double(&)[3]);
	virtual int SetAnglesFromTransform(const double(&)[4][4], double(&)[3]);
};

#endif
