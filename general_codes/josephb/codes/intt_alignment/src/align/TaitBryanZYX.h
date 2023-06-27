#ifndef TAIT_BRYAN_ZYX_H
#define TAIT_BRYAN_ZYX_H

#include "AngleConvention.h"

class TaitBryanZYX: public AngleConvention
{
	int SetTransformFromAngles(double (&)[4][4], const double (&)[3]) override;
	int SetAnglesFromTransform(const double (&)[4][4], double (&)[3]) override;
};

#endif
