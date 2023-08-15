#include "InttSensorReader.h"

void InttSensorReader::SetMarksDefault()
{
	marks["Sensor A - Cross %d"]=
	{
		Eigen::Vector3d(13.5,	6.0,	0.0),
		Eigen::Vector3d(115.0,	6.0,	0.0),
		Eigen::Vector3d(115.0,	28.0,	0.0),
		Eigen::Vector3d(13.5,	28.0,	0.0)
	};

	marks["Sensor B - Cross %d"]=
	{
		Eigen::Vector3d(115.7,	6.0,	0.0),
		Eigen::Vector3d(245.2,	6.0,	0.0),
		Eigen::Vector3d(245.2,	28.0,	0.0),
		Eigen::Vector3d(115.7,	28.0,	0.0)
	};

	marks["Sensor C - Cross %d"]=
	{
		Eigen::Vector3d(246.8,	6.0,	0.0),
		Eigen::Vector3d(376.3,	6.0,	0.0),
		Eigen::Vector3d(376.3,	28.0,	0.0),
		Eigen::Vector3d(246.8,	28.0,	0.0)
	};

	marks["Sensor D - Cross %d"]=
	{
		Eigen::Vector3d(377.0,	6.0,	0.0),
		Eigen::Vector3d(478.5,	6.0,	0.0),
		Eigen::Vector3d(478.5,	28.0,	0.0),
		Eigen::Vector3d(377.0,	28.0,	0.0)
	};

	marks["Endcap Hole %d"]=
	{
		Eigen::Vector3d(0.0,	0.0,	0.0),
		Eigen::Vector3d(492.0,	0.0,	0.0),
		Eigen::Vector3d(492.0,	34.0,	0.0),
		Eigen::Vector3d(0.0,	34.0,	0.0)
	};
}

Eigen::Affine3d InttSensorReader::GetTransformFromCorners(Corners_t const& corners)
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
	//(e.g., sensor to world/ladder to world)
}

Eigen::Affine3d InttSensorReader::GetSensorTransform(int ladder_z)
{
	Corners_t ladder_corners = marks["Endcap Hole %d"];
	Corners_t sensor_corners;
	switch(ladder_z)
	{
		case 1:
		sensor_corners = marks["Sensor A - Cross %d"];
		break;

		case 0:
		sensor_corners = marks["Sensor B - Cross %d"];
		break;

		case 2:
		sensor_corners = marks["Sensor C - Cross %d"];
		break;

		case 3:
		sensor_corners = marks["Sensor D - Cross %d"];
		break;

		default:
		break;
	}

	Eigen::Affine3d world_from_sensor = GetTransformFromCorners(sensor_corners);
	Eigen::Affine3d ladder_from_world = GetTransformFromCorners(ladder_corners).inverse();

	return ladder_from_world * world_from_sensor;

	//Transforms a vector in the sensor's coordinate system
	//to the ladder's coordinate system

	//The same transform that puts a sensor aligned with the ladder
	//to its position relative to the ladder
}

void InttSensorReader::ReadFile(std::string const& filename)
{
	int i;
	int j;
	int k;
	float f[2];
	char buff[256];
	std::string line;
	std::ifstream file;
	Map_t::iterator itr;

	if(filename.empty())
	{
		printf("InttSensorReader::ReadFile(std::string const& filename)\n");
		printf("\tArgument \"filename\" is empty string\n");

		return;
	}

	file.open(filename.c_str(), std::ios_base::in);

	if(!file.good())
	{
		printf("InttSensorReader::ReadFile(std::string const& filename)\n");
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
				for(j = 0; j < 3; ++j)
				{
					std::getline(file, line);
					k = sscanf(line.c_str(), "%*s %*s %*s %f %f", &(f[0]), &(f[1]));
					if(k)itr->second[i](j) = f[k-1];
				}

				break;
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

	file.close();
}
