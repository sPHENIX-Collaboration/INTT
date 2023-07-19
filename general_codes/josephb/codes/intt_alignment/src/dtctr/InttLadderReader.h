#ifndef INTT_LADDER_READER_H
#define INTT_LADDER_READER_H

#include "../align/AlignTransform.h"
#include <intt/InttMapping.h>

#include <cstdio>

#include <array>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "TVector3.h"

class InttLadderReader
{
public:
	typedef std::vector<TVector3> Corners_t;
	typedef std::map<std::string, Corners_t> Map_t;
	Map_t marks;

	void SetMarksDefault();
	void ReadFile(std::string const&);

	AlignTransform GetTransformFromCorners(Corners_t const&);
	AlignTransform GetLadderTransform(Intt::Online_s const&);

	void PrintMark(Map_t::const_iterator const&);
	void PrintMarks();
};

#endif//INTT_LADDER_READER_H
