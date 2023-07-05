#include "align/AlignTransform.h"
#include "dtctr/InttSensorReader.h"

#include <intt/InttMapping.h>
#include <trackbase/InttDefs.h>

#include <cstdio>

std::string path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";

int main()
{
	char buff[256];
	TrkrDefs::hitsetkey key;
	AlignTransform sensor_to_ladder;
	AlignTransform ladder_to_global;

	InttSensorReader isr;
	isr.SetMarksDefault();

	Intt::Offline_s ofl;
	ofl.layer = 3;
	ofl.ladder_phi = 0;
	ofl.ladder_z = 0;
	while(true)
	{
		isr.SetMarksDefault();
		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", (ofl.layer - 3) / 2, ((ofl.layer - 3) % 2 * 100) + ofl.ladder_phi);
		printf("%s\n", buff);

		isr.ReadFile(path + buff);

		for(ofl.ladder_z = 0; ofl.ladder_z < 4; ++ofl.ladder_z)
		{
			key = InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0);
			sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);

			//here
			printf("%d\n", key);
			sensor_to_ladder.Print();

			printf("\n\n");
		}

		++ofl.ladder_phi;
		if(ofl.ladder_phi < ((ofl.layer - 3) < 2 ? 12 : 16))continue;
		ofl.ladder_phi = 0;

		++ofl.layer;
		if(ofl.layer < 7)continue;
		ofl.layer = 3;

		break;
	}

	return 0;
}
