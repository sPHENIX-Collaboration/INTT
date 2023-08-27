#include "Debug.h"

Eigen::Affine3d Debug::TransformFromAngles(double const a, double const b, double const g)
{
	//same as
	//https://github.com/sPHENIX-Collaboration/coresoftware/blob/19ad20b2ee6d46c2b78f83e5557d54e03a26e797/offline/packages/trackbase/AlignmentTransformation.cc#L209
	//assuming survey

	Eigen::Affine3d transform;

	Eigen::AngleAxisd alpha(a, Eigen::Vector3d::UnitX());
	Eigen::AngleAxisd beta(b, Eigen::Vector3d::UnitY());
	Eigen::AngleAxisd gamma(g, Eigen::Vector3d::UnitZ());
	Eigen::Quaternion<double> quat = gamma * beta * alpha;

	transform.linear() = quat.matrix();

	return transform;
}


void Debug::AnglesFromTransform(double& a, double& b, double& g, Eigen::Affine3d const& t)
{
	//verified that the linear part of a transform is consisten with these
	//tested round trip with angles randomly generated in [-pi/2, pi/2)
	//see https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix

	g = atan(t.linear()(1, 0) / t.linear()(0, 0));
	b = asin(-t.linear()(2, 0));
	a = atan(t.linear()(2, 1) / t.linear()(2, 2));
}
