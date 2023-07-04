#include "InttSensorSurveyReader.h"

InttSensorSurveyReader::InttSensorSurveyReader()
{
	for(int i = 0; i < 4; ++i)
	{
		nominal_crosses.push_back(TVector3());
		actual_crosses.push_back(TVector3());

		nominal_endcaps.push_back(TVector3());
		actual_endcaps.push_back(TVector3());
	}

	crosses["Cross 1"]=		{&(nominal_crosses[0]), &(actual_crosses[0])};
	crosses["Cross 2"]=		{&(nominal_crosses[1]), &(actual_crosses[1])};
	crosses["Cross 3"]=		{&(nominal_crosses[2]), &(actual_crosses[2])};
	crosses["Cross 4"]=		{&(nominal_crosses[3]), &(actual_crosses[3])};

	endcaps["Endcap Hole 1"]=	{&(nominal_endcaps[0]), &(actual_endcaps[0])};
	endcaps["Endcap Hole 2"]=	{&(nominal_endcaps[1]), &(actual_endcaps[1])};
	endcaps["Endcap Hole 3"]=	{&(nominal_endcaps[2]), &(actual_endcaps[2])};
	endcaps["Endcap Hole 4"]=	{&(nominal_endcaps[3]), &(actual_endcaps[3])};

	marks["Sensor A - Cross 1"]=	{TVector3	(13.5,	6.0,	0.0),	TVector3(), false};
	marks["Sensor A - Cross 2"]=	{TVector3	(115.0,	6.0,	0.0),	TVector3(), false};
	marks["Sensor A - Cross 3"]=	{TVector3	(115.0,	28.0,	0.0),	TVector3(), false};
	marks["Sensor A - Cross 4"]=	{TVector3	(13.5,	28.0,	0.0),	TVector3(), false};

	marks["Sensor B - Cross 1"]=	{TVector3	(115.7,	6.0,	0.0),	TVector3(), false};
	marks["Sensor B - Cross 2"]=	{TVector3	(245.2,	6.0,	0.0),	TVector3(), false};
	marks["Sensor B - Cross 3"]=	{TVector3	(245.2,	28.0,	0.0),	TVector3(), false};
	marks["Sensor B - Cross 4"]=	{TVector3	(115.7,	28.0,	0.0),	TVector3(), false};

	marks["Sensor C - Cross 1"]=	{TVector3	(246.8,	6.0,	0.0),	TVector3(), false};
	marks["Sensor C - Cross 2"]=	{TVector3	(376.3,	6.0,	0.0),	TVector3(), false};
	marks["Sensor C - Cross 3"]=	{TVector3	(376.3,	28.0,	0.0),	TVector3(), false};
	marks["Sensor C - Cross 4"]=	{TVector3	(246.8,	28.0,	0.0),	TVector3(), false};

	marks["Sensor D - Cross 1"]=	{TVector3	(377.0,	6.0,	0.0),	TVector3(), false};
	marks["Sensor D - Cross 2"]=	{TVector3	(478.5,	6.0,	0.0),	TVector3(), false};
	marks["Sensor D - Cross 3"]=	{TVector3	(478.5,	28.0,	0.0),	TVector3(), false};
	marks["Sensor D - Cross 4"]=	{TVector3	(377.0,	28.0,	0.0),	TVector3(), false};

	marks["Endcap Hole 1"]=		{TVector3	(0.0,	0.0,	0.0),	TVector3(), false};
	marks["Endcap Hole 2"]=		{TVector3	(492.0,	0.0,	0.0),	TVector3(), false};
	marks["Endcap Hole 3"]=		{TVector3	(492.0,	34.0,	0.0),	TVector3(), false};
	marks["Endcap Hole 4"]=		{TVector3	(0.0,	34.0,	0.0),	TVector3(), false};
}

int InttSensorSurveyReader::ReadSurveyFile(const std::string& filename)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::ReadSurveyFile(const std::string& filename)" << std::endl;

	int i = 0;
	float f[2] = {0.0, 0.0};

	std::string line;
	std::ifstream survey_file;
	std::map<std::string, std::tuple<TVector3, TVector3, bool>>::iterator itr = marks.begin();

	if(filename == "")
	{
		out << "\tPassed argument \"filename\" is empty string" << std::endl;
		return_val = 1;
		goto label;
	}
	survey_file.open(filename.c_str(), std::ifstream::in);
	if(!survey_file.good())
	{
		out << "\tCould not produce good std::ifstream from path:" << std::endl;
		out << "\t\t" << filename << std::endl;
		return_val = 1;
		goto label;
	}

	for(itr = marks.begin(); itr != marks.end(); ++itr)
	{
		std::get<2>(itr->second) = false;
	}

	for(line; std::getline(survey_file, line);)
	{
		for(itr = marks.begin(); itr != marks.end(); ++itr)
		{
			if(line.find(itr->first) != std::string::npos)
			{
				std::getline(survey_file, line);
				i = sscanf(line.c_str(), "%*s %*s %*s %f %f", &(f[0]), &(f[1]));
				if(i)
				{
						std::get<1>(itr->second).SetX(f[i - 1]);
						std::get<2>(itr->second) = true;
				}
				else
				{
						out << "\tBad read for coordinate:" << std::endl;
						out << "\t\t" << itr->first << "\tX" << std::endl;
						return_val = 1;
				}

				std::getline(survey_file, line);
				i = sscanf(line.c_str(), "%*s %*s %*s %f %f", &(f[0]), &(f[1]));
				if(i)
				{
						std::get<1>(itr->second).SetY(f[i - 1]);
						std::get<2>(itr->second) = true;
				}
				else
				{
						out << "\tBad read for coordinate:" << std::endl;
						out << "\t\t" << itr->first << "\tY" << std::endl;
						return_val = 1;
				}

				std::getline(survey_file, line);
				i = sscanf(line.c_str(), "%*s %*s %*s %f %f", &(f[0]), &(f[1]));
				if(i)
				{
						std::get<1>(itr->second).SetZ(f[i - 1]);
						std::get<2>(itr->second) = true;
				}
				else
				{
						out << "\tBad read for coordinate:" << std::endl;
						out << "\t\t" << itr->first << "\tZ" << std::endl;
						return_val = 1;
				}
				

				break;
			}
		}
	}

	label:
	if(survey_file.is_open())survey_file.close();

	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::SetMarks(const int& i)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::ComputeTransform(const int& i, AlignTransform& t)" << std::endl;

	std::string s = "";

	std::map<std::string, std::tuple<TVector3, TVector3, bool>>::const_iterator mark = marks.begin();
	std::map<std::string, std::pair<TVector3*, TVector3*>>::const_iterator crosses_itr = crosses.begin();
	std::map<std::string, std::pair<TVector3*, TVector3*>>::const_iterator endcaps_itr = endcaps.begin();

	if(!(0 <= i && i < INTT::SENSOR))
	{
		out << "\tPassed argument i (" << i << ") out valid range [0, 4)" << std::endl;
		return_val = 1;
		goto label;
	}

	for(crosses_itr = crosses.begin(); crosses_itr != crosses.end(); ++crosses_itr)
	{
		s = sensors[i] + " - " + crosses_itr->first;
		mark = marks.find(s);
		if(mark == marks.end())
		{
			out << "\tNo item with key \"" << s << "\" found in marks map (typo in source code?)" << std::endl;
			return_val = 1;
			goto label;
		}
		if(!std::get<2>(mark->second))
		{
			out << "\tMark \"" << s << "\" was not set" << std::endl;
			out << "\tWas this preceded by a successful call to ReadSurveyFile in the main program?" << std::endl;
			return_val = 1;
			goto label;
		}

		*((crosses_itr->second).first) = std::get<0>(mark->second);
		*((crosses_itr->second).second) = std::get<1>(mark->second);
	}

	for(endcaps_itr = endcaps.begin(); endcaps_itr != endcaps.end(); ++endcaps_itr)
	{
		s = endcaps_itr->first;
		mark = marks.find(s);
		if(mark == marks.end())
		{
			out << "\tNo item with key \"" << s << "\" found in marks map (typo in source code?)" << std::endl;
			return_val = 1;
			goto label;
		}
		if(!std::get<2>(mark->second))
		{
			out << "\tMark \"" << s << "\" was not set" << std::endl;
			out << "\tWas this preceded by a successful call to ReadSurveyFile in the main program?" << std::endl;
			return_val = 1;
			goto label;
		}

		*((endcaps_itr->second).first) = std::get<0>(mark->second);
		*((endcaps_itr->second).second) = std::get<1>(mark->second);
	}

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetMark(const int& i, const int& j, TVector3& u, TVector3& v)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetMark(const int& i, const int& j, TVector3& u, TVector3& v)" << std::endl;

	std::string s;
	AlignTransform nominal_ladder_to_world;
	AlignTransform mark_transform;
	AlignTransform nominal_in_ladder;
	AlignTransform actual_in_ladder;

	std::map<std::string, std::tuple<TVector3, TVector3, bool>>::const_iterator mark = marks.begin();

	if(!(0 <= i and i < crosses.size()))
	{
		out << "\tArgument \"i\" out of range" << std::endl;
		return_val = 1;
		goto label;
	}

	if(SetMarks(i))
	{
		return_val = 1;
		goto label;
	}

	if(GetTransformFromMarks(nominal_ladder_to_world, nominal_endcaps))
	{
		return_val = 1;
		goto label;
	}

	s = sensors[i] + " - " + std::next(crosses.begin(), j)->first;
	mark = marks.find(s);

	if(mark == marks.end())
	{
		out << "\tFailed to find mark \"" << s << "\", typo in source code?" << std::endl;
		return_val = 1;
		goto label;
	}
	if(!std::get<2>(mark->second))
	{
			out << "\tMark \"" << s << "\" was not set" << std::endl;
			out << "\tWas this preceded by a successful call to ReadSurveyFile in the main program?" << std::endl;
			return_val = 1;
			goto label;
	}

	mark_transform.Pos(0) = (std::get<0>(mark->second))[0];
	mark_transform.Pos(1) = (std::get<0>(mark->second))[1];
	mark_transform.Pos(2) = (std::get<0>(mark->second))[2];

	nominal_ladder_to_world = nominal_ladder_to_world.Inverse();
	mark_transform = nominal_ladder_to_world * mark_transform;
	//mark transform is now its nominal tranform in the ladder frame

	//using nominal position in ladder frame, find where this mark is and should be in sensor frame
	if(GetNominalSensorToLadder(i, nominal_in_ladder))
	{
		return_val = 1;
		goto label;
	}
	if(GetActualSensorToLadder(i, actual_in_ladder))
	{
		return_val = 1;
		goto label;
	}

	nominal_in_ladder = nominal_in_ladder.Inverse();
	nominal_in_ladder = nominal_in_ladder * mark_transform;
	u[0] = nominal_in_ladder.Pos(0);
	u[1] = nominal_in_ladder.Pos(1);
	u[2] = nominal_in_ladder.Pos(2);

	actual_in_ladder = actual_in_ladder.Inverse();
	actual_in_ladder = actual_in_ladder * mark_transform;
	v[0] = actual_in_ladder.Pos(0);
	v[1] = actual_in_ladder.Pos(1);
	v[2] = actual_in_ladder.Pos(2);

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetWorstCross(double& d)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetWorstCross(double& d)" << std::endl;

	TVector3 v;
	std::map<std::string, std::pair<TVector3*, TVector3*>>::const_iterator crosses_itr = crosses.begin();

	d = 0.0;
	for(crosses_itr = crosses.begin(); crosses_itr != crosses.end(); ++crosses_itr)
	{
		v = *((crosses_itr->second).first) - *((crosses_itr->second).second);
		if(v.Mag() > d)d = v.Mag();
	}

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetTransformFromMarks(AlignTransform& t, const std::vector<TVector3>& transform_marks)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetTransformFromMarks(AlignTransform& t, const std::vector<TVector3>& transform_marks)" << std::endl;
	//local to world

	t.Reset();

	int i = 0;
	int j = 0;
	TVector3 axes[3];

	axes[2] = (transform_marks[2] - transform_marks[0]) - (transform_marks[3] - transform_marks[1]);
	axes[0] = (transform_marks[2] - transform_marks[0]) + (transform_marks[3] - transform_marks[1]);
	axes[1] = axes[2].Cross(axes[0]);

	axes[0] = axes[0].Unit();
	axes[1] = axes[1].Unit();
	axes[2] = axes[2].Unit();

	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			t[i][j] = axes[j][i];
		}

		t.Pos(i) = (transform_marks[0][i] + transform_marks[1][i] + transform_marks[2][i] + transform_marks[3][i]) / 4.0;
	}
	t.SetAnglesFromTransform();

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetTransform(AlignTransform& t, const std::vector<TVector3>& child_marks, const std::vector<TVector3>& parent_marks)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetTransform(AlignTransform& t, const std::vector<TVector3>& child_marks, const std::vector<TVector3>& parent_marks)" << std::endl;
	//child to parent

	t.Reset();

	AlignTransform parent;	//parent to world
	AlignTransform child;	//child to world

	if(GetTransformFromMarks(parent, parent_marks))
	{
		return_val = 1;
		goto label;
	}

	if(GetTransformFromMarks(child, child_marks))
	{
		return_val = 1;
		goto label;
	}

	parent = parent.Inverse();
	t = parent * child;

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetNominalPixelToSensor(const int& i, const int& j, const int& k, AlignTransform& t)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetNominalPixelToSensor(const int& i, const int& j, const int& k, AlignTransform& t)" << std::endl;
	//nominal pixel to sensor

	t.Reset();

	if(!(0 <= j && j < INTT::STRIP_X))
	{
		out << "\tPassed argument j (" << j << ") not a vaild strip_x index [0, " << INTT::STRIP_X << ")" << std::endl;
		return_val = 1;
		goto label;
	}
	if(!(0 <= k && k < INTT::STRIP_Z[i]))
	{
		out << "\tPassed argument k (" << k << ") not a vaild strip_z index [0, " << INTT::STRIP_Z[i] << ")" << std::endl;
		return_val = 1;
		goto label;
	}

	//pixel to sensor (nominal)
	t.Pos(0) = INTT::STRIP_XM * j + INTT::STRIP_XB;
	t.Pos(2) = INTT::STRIP_ZM[i] * k + INTT::STRIP_ZB[i];

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetNominalSensorToLadder(const int& i, AlignTransform& t)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetNominalSensorTransform(const int& i, AlignTransform& t)" << std::endl;
	//nominal sensor to ladder

	t.Reset();

	if(SetMarks(i))
	{
		return_val = 1;
		goto label;
	}

	if(GetTransform(t, nominal_crosses, nominal_endcaps))
	{
		return_val = 1;
		goto label;
	}

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetActualSensorToLadder(const int& i, AlignTransform& t)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetActualSensorTransform(const int& i, AlignTransform& t)" << std::endl;
	//actual sensor to ladder

	t.Reset();

	if(SetMarks(i))
	{
		return_val = 1;
		goto label;
	}

	if(GetTransform(t, actual_crosses, actual_endcaps))
	{
		return_val = 1;
		goto label;
	}

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}


int InttSensorSurveyReader::GetNominalPixelToLadder(const int& i, const int& j, const int& k, AlignTransform& t)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetNominalPixelToLadder(const int& i, const int& j, const int& k, AlignTransform& t)" << std::endl;
	//nominal pixel to ladder

	t.Reset();

	//sensor to ladder
	AlignTransform u;

	if(GetNominalPixelToSensor(i, j, k, t))
	{
		return_val = 1;
		goto label;
	}

	if(GetNominalSensorToLadder(i, u))
	{
		return_val = 1;
		goto label;
	}

	//(ladder <- sensor) * (sensor <- pixel)
	t = u * t;

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

int InttSensorSurveyReader::GetActualPixelToLadder(const int& i, const int& j, const int& k, AlignTransform& t)
{
	int return_val = 0;
	std::stringstream out;
	out << "InttSensorSurveyReader::GetActualPixelToLadder(const int& i, const int& j, const int& k, AlignTransform& t)" << std::endl;
	//nominal pixel to ladder

	t.Reset();

	//sensor to ladder
	AlignTransform u;

	if(GetNominalPixelToSensor(i, j, k, t))
	{
		return_val = 1;
		goto label;
	}

	if(GetActualSensorToLadder(i, u))
	{
		return_val = 1;
		goto label;
	}

	//(ladder <- sensor) * (sensor <- pixel)
	t = u * t;

	label:
	out << std::ends;
	if(return_val)std::cout << out.str() << std::endl;
	return return_val;
}

void InttSensorSurveyReader::PrintMarks()
{
	std::map<std::string, std::tuple<TVector3, TVector3, bool>>::const_iterator itr;

	for(itr = marks.begin(); itr != marks.end(); ++itr)
	{
		std::cout << itr->first << std::endl;
		std::cout << "\tX:\t" << std::get<1>(itr->second).X() << "\t(" << std::get<0>(itr->second).X() << ")" << std::endl;
		std::cout << "\tY:\t" << std::get<1>(itr->second).Y() << "\t(" << std::get<0>(itr->second).Y() << ")" << std::endl;
		std::cout << "\tZ:\t" << std::get<1>(itr->second).Z() << "\t(" << std::get<0>(itr->second).Z() << ")" << std::endl;
	}
}
