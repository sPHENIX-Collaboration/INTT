#include "eigen/InttSensorReader.h"
#include "eigen/InttLadderReader.h"
#include "eigen/Debug.h"

#include <intt/InttMapping.h>
#include <trackbase/InttDefs.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <random>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

const double PI = 3.141592653589793238;

int main()
{
	bool verbose = true;

	std::uniform_real_distribution<double> dis(-0.5 * PI, 0.5 * PI);
	std::mt19937 gen(1234);

	double a;
	double b;
	double g;
	Eigen::Affine3d t;
	Eigen::Affine3d u;
	for(int n = 0; n < 10; ++n)
	{
		a = dis(gen);
		b = dis(gen);
		g = dis(gen);

		t = Debug::TransformFromAngles(a, b, g);
		Debug::AnglesFromTransform(a, b, g, t);
		u = Debug::TransformFromAngles(a, b, g);

		if(verbose)
		{
			std::cout << std::endl;
			std::cout << t.linear() << std::endl;
			std::cout << std::endl;
			std::cout << u.linear() << std::endl;
			std::cout << std::endl;
		}
	}

	return 0;
}
