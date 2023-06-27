#ifndef INTT_SENSOR_SURVEY_READER_H
#define INTT_SENSOR_SURVEY_READER_H

#include "InttParameters.h"
#include "../align/AlignTransform.h"

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"

#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>


class InttSensorSurveyReader
{
protected:
	std::string current_sensor = "";
	std::vector<std::string> sensors = {"Sensor A", "Sensor B", "Sensor C", "Sensor D"};

	std::vector<TVector3> nominal_crosses;
	std::vector<TVector3> actual_crosses;

	std::vector<TVector3> nominal_endcaps;
	std::vector<TVector3> actual_endcaps;

	std::map<std::string, std::pair<TVector3*, TVector3*>> crosses;
	std::map<std::string, std::pair<TVector3*, TVector3*>> endcaps;

	std::map<std::string, std::tuple<TVector3, TVector3, bool>> marks;

	int GetTransformFromMarks(AlignTransform&, const std::vector<TVector3>&);
	int GetTransform(AlignTransform&, const std::vector<TVector3>&, const std::vector<TVector3>&);

public:
	std::string text_output_filename = "sensor_survey_data.root";
	std::string root_output_filename = "sensor_survey_read.out";

	InttSensorSurveyReader();

	int ReadSurveyFile(const std::string&);
	int SetMarks(const int&);
	int GetMark(const int&, const int&, TVector3&, TVector3&);
	int GetWorstCross(double&);

	int GetNominalPixelToSensor(const int&, const int&, const int &, AlignTransform&);

	int GetNominalSensorToLadder(const int&, AlignTransform&);
	int GetActualSensorToLadder(const int&, AlignTransform&);

	int GetNominalPixelToLadder(const int&, const int&, const int&, AlignTransform&);
	int GetActualPixelToLadder(const int&, const int&, const int&, AlignTransform&);

	void PrintMarks();
};

#endif
