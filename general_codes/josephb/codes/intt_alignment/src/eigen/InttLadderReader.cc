#include "InttLadderReader.h"

void InttLadderReader::SetMarksDefault()
{
	char buff[256];
	double two_pi = 2 * 3.141592653589793238;

	double r;
	double theta;
	std::string name;

	marks.clear();

	int lyr = 0;
	int ldr = 0;
	while(true)
	{
		r = 0.0;
		switch(lyr)
		{
			case 0:
			r = 71.87489;
			break;

			case 1:
			r = 77.99527;
			break;

			case 2:
			r = 96.80091;
			break;

			case 3:
			r = 103.28663;
			break;

			default:
			break;
		}

		theta = two_pi / (lyr < 2 ? 12 : 16);
		theta = (ldr * theta) + ((lyr % 2) ? 0.0 : theta / 2.0);

		snprintf(buff, sizeof(buff), "B%dL%03d", lyr / 2, 100 * (lyr % 2) + ldr);
		name = buff;
		name += "_%d";
		marks[name]=
		{
			Eigen::Vector3d(sin(theta) * r - cos(theta) * 8.5, -1.0 * cos(theta) * r - sin(theta) * 8.5, -246.0),
			Eigen::Vector3d(sin(theta) * r - cos(theta) * 8.5, -1.0 * cos(theta) * r - sin(theta) * 8.5, +246.0),
			Eigen::Vector3d(sin(theta) * r + cos(theta) * 8.5, -1.0 * cos(theta) * r + sin(theta) * 8.5, +246.0),
			Eigen::Vector3d(sin(theta) * r + cos(theta) * 8.5, -1.0 * cos(theta) * r + sin(theta) * 8.5, -246.0)
		};

		++ldr;
		if(ldr < (lyr < 2 ? 12 : 16))continue;
		ldr = 0;

		++lyr;
		if(lyr < 4)continue;
		lyr = 0;

		break;
	}
}

void InttLadderReader::ReadFile(std::string const& filename)
{
	int i;
	int j;
	float f[3];
	char buff[256];
	std::string line;
	std::ifstream file;
	Map_t::iterator itr;

	if(filename.empty())
	{
		printf("InttLadderReader::ReadFile(std::string const& filename)\n");
		printf("\tArgument \"filename\" is empty string\n");

		return;
	}

	file.open(filename.c_str(), std::ios_base::in);

	if(!file.good())
	{
		printf("InttLadderReader::ReadFile(std::string const& filename)\n");
		printf("\tCould not produce good stream from file:\n");
		printf("\t%s\n", filename.c_str());

		return;
	}

	for(line; std::getline(file, line);)
	{
		i = 0;
		itr = marks.begin();
		while(true)
		{
			snprintf(buff, sizeof(buff), itr->first.c_str(), i + 1);
			if(line.find(buff) != std::string::npos)
			{
				j = sscanf(line.c_str(), "%*s %f, %f, %f", &(f[0]), &(f[1]), &(f[2]));
				itr->second[i](0) = f[0];
				itr->second[i](1) = f[2];
				itr->second[i](2) = -1.0 * f[1];
			}

			++i;
			if(i < 4)continue;
			i = 0;

			++itr;
			if(itr != marks.end())continue;
			itr = marks.begin();
			
			break;
		}
	}
}

Eigen::Affine3d InttLadderReader::GetTransformFromCorners(Corners_t const& corners)
{
	int i = 0;
	int j = 0;

	Eigen::Vector3d axes[3];
	Eigen::Affine3d t(Eigen::Affine3d::Identity());

	axes[2] = (corners[2] - corners[0]) - (corners[3] - corners[1]);
	axes[0] = (corners[2] - corners[0]) + (corners[3] - corners[1]);
	axes[1] = axes[2].cross(axes[0]);

	for(i = 0; i < 3; ++i)axes[i] = axes[i].normalized();
	for(i = 0; i < 3; ++i)for(j = 0; j < 3; ++j)t.linear()(i, j) = axes[j][i];
	for(i = 0; i < 3; ++i)for(j = 0; j < 4; ++j)t.translation()(i) += corners[j][i];
	t.translation() *= 0.25;

	t.rotation();

	return t;

	//t * v
	//transforms a v in the coordinate system defined by the corners
	//to the coordinate system the corners were measured in
	//(e.g., ladder to world)
}

Eigen::Affine3d InttLadderReader::GetLadderTransform(InttNameSpace::Online_s const& s)
{
	char buff[256];
	snprintf(buff, sizeof(buff), "B%dL%03d", s.lyr / 2, 100 * (s.lyr % 2) + s.ldr);
	std::string name = buff;
	name += "_%d";

	if(marks.find(name) == marks.end())
	{
		printf("AlignTransform InttLadderReader::GetLadderTransform(InttNameSpace::Online_s const& s)\n");
		printf("\tError in source code; map key was generated incorrectly\n");

		return Eigen::Affine3d::Identity();
	}

	return GetTransformFromCorners(marks[name]);
}
