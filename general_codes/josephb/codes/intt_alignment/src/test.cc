#include "align/AlignTransform.h"
#include "dtctr/InttSensorReader.h"
#include "dtctr/InttLadderReader.h"

#include <intt/InttMapping.h>
#include <trackbase/InttDefs.h>

#include <cstdio>

std::string sensor_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";
std::string ladder_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/";

int main()
{
	char buff[256];
	TrkrDefs::hitsetkey key;
	AlignTransform sensor_to_ladder;
	AlignTransform ladder_to_global;

	InttSensorReader isr;
	isr.SetMarksDefault();

	Intt::Offline_s ofl;
	Intt::Online_s onl;
	onl.lyr = 0;
	onl.ldr = 0;
	while(true)
	{
		isr.SetMarksDefault();
		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		printf("%s\n", buff);

		isr.ReadFile(sensor_path + buff);

		ofl = Intt::ToOffline(onl);
		for(ofl.ladder_z = 0; ofl.ladder_z < 4; ++ofl.ladder_z)
		{
			key = InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0);
			sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);

			//here
			printf("%d\n", key);
			sensor_to_ladder.Print();

			printf("\n\n");
		}

		++onl.ldr;
		if(onl.ldr < (onl.lyr < 2 ? 12 : 16))continue;
		onl.ldr = 0;

		++onl.lyr;
		if(onl.lyr < 4)continue;
		onl.lyr = 0;

		break;
	}

	for(int i = 0; i < 20; ++i)printf("\n");
	
	InttLadderReader ilr;
	ilr.SetMarksDefault();
	ilr.ReadFile(ladder_path + "EAST_INTT.txt");
	ilr.ReadFile(ladder_path + "WEST_INTT.txt");
	ilr.PrintMarks();

	onl.lyr = 0;
	onl.ldr = 0;
	while(true)
	{
		snprintf(buff, sizeof(buff), "B%01dL%03d:", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		printf("%s\n", buff);

		ladder_to_global = ilr.GetLadderTransform(onl);
		ladder_to_global.Print();
		printf("\n\n");

		++onl.ldr;
		if(onl.ldr < (onl.lyr < 2 ? 12 : 16))continue;
		onl.ldr = 0;

		++onl.lyr;
		if(onl.lyr < 4)continue;
		onl.lyr = 0;

		break;
	}
	return 0;
}
