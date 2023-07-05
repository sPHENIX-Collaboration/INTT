#include <cstdio>

#include "dtctr/InttSensorReader.h"

int main()
{
	InttSensorReader isr;
	isr.SetMarksDefault();

	for(std::map<std::string, InttSensorReader::Corners_t>::const_iterator itr = isr.marks.begin(); itr != isr.marks.end(); ++itr)
	{
		isr.PrintMark(itr);
		isr.GetTransformFromCorners(itr->second).Inverse().Print();
	}

	return 0;
}
