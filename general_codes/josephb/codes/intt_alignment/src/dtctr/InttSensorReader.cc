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
}

void InttSensorReader::PrintMark(std::map<std::string, Corners_t>::const_iterator const& itr)
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
	for(std::map<std::string, Corners_t>::const_iterator itr = marks.begin(); itr != marks.end(); ++itr)PrintMark(itr);
}
