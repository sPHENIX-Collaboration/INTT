#ifndef INTT_LADDER_READER_H
#define INTT_LADDER_READER_H

#include <intt/InttMapping.h>

#include <cstdio>

#include <array>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

class InttLadderReader
{
public:
	typedef std::vector<Eigen::Vector3d> Corners_t;
	typedef std::map<std::string, Corners_t> Map_t;
	Map_t marks;

	void SetMarksDefault();
	void ReadFile(std::string const&);

	Eigen::Affine3d GetTransformFromCorners(Corners_t const&);
	Eigen::Affine3d GetLadderTransform(Intt::Online_s const&);
};

#endif//INTT_LADDER_READER_H
