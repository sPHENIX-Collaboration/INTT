#include "AngleConvention.h"

int AngleConvention::SetTransformFromAngles(double (&t)[4][4], const double (&a)[3])
{
	return 1;
}

int AngleConvention::SetAnglesFromTransform(const double (&t)[4][4], double (&a)[3])
{
	return 1;
}
