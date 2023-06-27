#ifndef INTT_PARAMETERS_H
#define INTT_PARAMETERS_H

#include <cmath>
#include <array>

namespace INTT
{
	const double PI = atan(1.0) * 4;

	constexpr	int			LAYER=		4;
	constexpr	std::array<int, 4>	LADDER=		{12,		12,		16,		16};
	constexpr	int			SENSOR=		4;
	constexpr	std::array<int, 4>	STRIP_Z=	{5,		8,		8,		5};
	constexpr	int			STRIP_X=	256;

	constexpr 	std::array<double, 4> 	RADIUS=	{7.187489,	7.799527,	9.880091,	10.328663};
	constexpr 	std::array<double, 4>	PHI_M=	{PI / 6.0,	PI / 6.0,	PI / 8.0,	PI / 8.0};
	constexpr 	std::array<double, 4>	PHI_B=	{PI / 12.0,	0.0,		PI / 16.0,	0.0};

	//crude but close
	constexpr	std::array<double, 4>	STRIP_W=	{115.0 - 13.5,	245.2 - 115.7, 376.3 - 246.8,	478.5 - 377.0};
	constexpr	double			STRIP_H=	28.0 - 6.0;

	constexpr	std::array<double, 4>	MAKE_STRIP_ZM()
	{
		int i = 0;
		std::array<double, 4> strip_zm = {0};
		for(i = 0; i < 4; ++i)strip_zm[i] = (STRIP_W[i]) / (STRIP_Z[i]);
		return strip_zm;
	}
	constexpr	std::array<double, 4>	STRIP_ZM=	MAKE_STRIP_ZM();

	constexpr	std::array<double, 4>	MAKE_STRIP_ZB()
	{
		int i = 0;
		std::array<double, 4> strip_zb = {0};
		for(i = 0; i < 4; ++i)strip_zb[i] = (STRIP_ZM[i] - STRIP_W[i]) / 2;
		return strip_zb;
	}
	constexpr	std::array<double, 4>	STRIP_ZB=	MAKE_STRIP_ZB();

	constexpr double STRIP_XM=	STRIP_H / STRIP_X;
	constexpr double STRIP_XB=	(STRIP_XM - STRIP_H) / 2;
};

#endif
