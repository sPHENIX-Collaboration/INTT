#ifndef INTT_SENSOR_READER_H
#define INTT_SENSOR_READER_H

#include "../align/AlignTransform.h"

#include <cstdio>

#include <array>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "TVector3.h"

class InttSensorReader
{
public:
	//typedef std::array<std::array<double, 3>, 4> Corners_t;
	typedef std::vector<TVector3> Corners_t;
	std::map<std::string, Corners_t> marks;

	void SetMarksDefault();


	AlignTransform GetTransformFromCorners(Corners_t const&);

	//int SetMarksFromFile(std::string const&);
	void PrintMark(std::map<std::string, Corners_t>::const_iterator const&);
	void PrintMarks();
};

#endif//INTT_SENSOR_READER_H
