#include "InttSensorReader.h"

void InttSensorReader::SetMarksDefault()
{
	marks["Sensor A - Cross %d"]=
	{
		TVector3(13.5,	6.0,	0.0),
		TVector3(115.0,	6.0,	0.0),
		TVector3(115.0,	28.0,	0.0),
		TVector3(13.5,	28.0,	0.0)
	};

	marks["Sensor B - Cross %d"]=
	{
		TVector3(115.7,	6.0,	0.0),
		TVector3(245.2,	6.0,	0.0),
		TVector3(245.2,	28.0,	0.0),
		TVector3(115.7,	28.0,	0.0)
	};

	marks["Sensor C - Cross %d"]=
	{
		TVector3(246.8,	6.0,	0.0),
		TVector3(376.3,	6.0,	0.0),
		TVector3(376.3,	28.0,	0.0),
		TVector3(246.8,	28.0,	0.0)
	};

	marks["Sensor D - Cross %d"]=
	{
		TVector3(377.0,	6.0,	0.0),
		TVector3(478.5,	6.0,	0.0),
		TVector3(478.5,	28.0,	0.0),
		TVector3(377.0,	28.0,	0.0)
	};

	marks["Endcap Hole %d"]=
	{
		TVector3(0.0,	0.0,	0.0),
		TVector3(492.0,	0.0,	0.0),
		TVector3(492.0,	34.0,	0.0),
		TVector3(0.0,	34.0,	0.0)
	};
}

AlignTransform InttSensorReader::GetTransformFromCorners(Corners_t const& corners)
{
	int i = 0;
	int j = 0;

	TVector3 axes[3];
	AlignTransform t;

	axes[2] = (corners[2] - corners[0]) - (corners[3] - corners[1]);
	axes[0] = (corners[2] - corners[0]) + (corners[3] - corners[1]);
	axes[1] = axes[2].Cross(axes[0]);

	for(i = 0; i < 3; ++i)axes[i] = axes[i].Unit();
	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)t[i][j] = axes[j][i];
		t.Pos(i) = (corners[0][i] + corners[1][i] + corners[2][i] + corners[3][i]) / 4.0;
	}

	t.SetAnglesFromTransform();

	return t;

	//t * v
	//transforms a v in the coordinate system defined by the corners
	//to the coordinate system the corners were measured in
}

AlignTransform InttSensorReader::GetSensorTransform(int ladder_z)
{
	Corners_t endcap_corners = marks["Endcap Hole %d"];
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

	AlignTransform sensor_from_world = GetTransformFromCorners(sensor_corners).Inverse();
	AlignTransform world_from_endcap = GetTransformFromCorners(endcap_corners);

	return sensor_from_world * world_from_endcap;

	//Transforms a vector in the ladder's coordinate system
	//to the sensor's coordinate system
}

void InttSensorReader::ReadFile(std::string const& filename)
{
	int i;
	int j;
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
				std::getline(file, line);
				j = sscanf(line.c_str(), "%*s %*s %*s %f %f", &(f[0]), &(f[1]));
				if(j)itr->second[i].SetX(f[j-1]);

				std::getline(file, line);
				j = sscanf(line.c_str(), "%*s %*s %*s %f %f", &(f[0]), &(f[1]));
				if(j)itr->second[i].SetY(f[j-1]);

				std::getline(file, line);
				j = sscanf(line.c_str(), "%*s %*s %*s %f %f", &(f[0]), &(f[1]));
				if(j)itr->second[i].SetZ(f[j-1]);

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

void InttSensorReader::PrintMark(Map_t::const_iterator const& itr)
{
	printf("%s\n", itr->first.c_str());
	for(int i = 0; i < 4; ++i)
	{
		printf("\t%d\tx:%6.1lf\ty:%6.1lf\tz:%6.1lf\n", i + 1, itr->second[i].X(), itr->second[i].Y(), itr->second[i].Z());
	}
	printf("\n");
}

void InttSensorReader::PrintMarks()
{
	for(Map_t::const_iterator itr = marks.begin(); itr != marks.end(); ++itr)PrintMark(itr);
}
