#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>
#include <cmath>

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

class Debug
{
private:

public:
	static Eigen::Affine3d TransformFromAngles(double const, double const, double const);
	static void AnglesFromTransform(double& a, double& b, double& g, Eigen::Affine3d const& t);
};

#endif//DEBUG_H
