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
			TVector3(sin(theta) * r - cos(theta) * 8.5, -1.0 * cos(theta) * r - sin(theta) * 8.5, -246.0),
			TVector3(sin(theta) * r - cos(theta) * 8.5, -1.0 * cos(theta) * r - sin(theta) * 8.5, +246.0),
			TVector3(sin(theta) * r + cos(theta) * 8.5, -1.0 * cos(theta) * r + sin(theta) * 8.5, +246.0),
			TVector3(sin(theta) * r + cos(theta) * 8.5, -1.0 * cos(theta) * r + sin(theta) * 8.5, -246.0)
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
				itr->second[i].SetX(f[0]);
				itr->second[i].SetY(f[2]);
				itr->second[i].SetZ(f[1]);
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

AlignTransform InttLadderReader::GetTransformFromCorners(Corners_t const& corners)
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
	//(e.g., ladder to world)
}

AlignTransform InttLadderReader::GetLadderTransform(Intt::Online_s const& s)
{
	char buff[256];
	snprintf(buff, sizeof(buff), "B%dL%03d", s.lyr / 2, 100 * (s.lyr % 2) + s.ldr);
	std::string name = buff;
	name += "_%d";

	if(marks.find(name) == marks.end())
	{
		printf("AlignTransform InttLadderReader::GetLadderTransform(Intt::Online_s const& s)\n");
		printf("\tError in source code; map key was generated incorrectly\n");

		return AlignTransform();
	}

	return GetTransformFromCorners(marks[name]);
}

void InttLadderReader::PrintMark(Map_t::const_iterator const& itr)
{
	printf("%s\n", itr->first.c_str());
	for(int i = 0; i < 4; ++i)
	{
		printf("\t%d\tx:%6.1lf\ty:%6.1lf\tz:%6.1lf\n", i + 1, itr->second[i].X(), itr->second[i].Y(), itr->second[i].Z());
	}
	printf("\n");
}

void InttLadderReader::PrintMarks()
{
	for(Map_t::const_iterator itr = marks.begin(); itr != marks.end(); ++itr)PrintMark(itr);
}
