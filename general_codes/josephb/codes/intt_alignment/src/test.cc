#include "dtctr/InttSensorReader.h"
#include <intt/InttMapping.h>
#include <trackbase/InttDefs.h>

#include <cstdio>

std::string path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";

int main()
{
	InttSensorReader isr;
	isr.SetMarksDefault();

	isr.ReadFile(path + "B0L001.txt");
	for(InttSensorReader::Map_t::const_iterator itr = isr.marks.begin(); itr != isr.marks.end(); ++itr)
	{
		isr.PrintMark(itr);
		//isr.GetTransformFromCorners(itr->second).Inverse().Print();
		isr.GetTransformFromCorners(itr->second).Print();
		printf("\n\n");
	}

	Intt::Offline_s ofl;
	ofl.layer = 3;
	ofl.ladder_phi = 1;
	ofl.ladder_z = 0;
	ofl.strip_x = 0;
	ofl.strip_y = 0;

	printf("\n\n\n\n\n\n");
	for(ofl.ladder_z = 0; ofl.ladder_z < 4; ++ofl.ladder_z)
	{
		std::cout << InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0) << std::endl;
		isr.GetSensorTransform(ofl.ladder_z).Print();
		printf("\n\n");
	}

	return 0;
}
