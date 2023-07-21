#include "align/AlignTransform.h"
#include "dtctr/InttSensorReader.h"
#include "dtctr/InttLadderReader.h"

#include <intt/InttMapping.h>
#include <trackbase/InttDefs.h>

#include <cstdio>
#include <iostream>
#include <fstream>

std::string sensor_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";
std::string ladder_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/";

std::string output_file = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/transforms.txt";

int main()
{
	char buff[256];
	TrkrDefs::hitsetkey key;
	AlignTransform sensor_to_ladder;
	AlignTransform ladder_to_global;
	AlignTransform sensor_to_global;

	InttLadderReader ilr;
	ilr.SetMarksDefault();
	ilr.ReadFile(ladder_path + "EAST_INTT.txt");
	ilr.ReadFile(ladder_path + "WEST_INTT.txt");

	InttSensorReader isr;
	isr.SetMarksDefault();

	if(output_file.empty())
	{
		printf("Variable \"output_file\" is empty string\n");

		return 1;
	}
	std::ofstream file(output_file.c_str(), std::ios_base::out | std::ios_base::trunc);
	if(!file.good())
	{

		printf("Could not produce good stream from file:\n");
		printf("\n%s\n", output_file.c_str());

		return 1;
	}

	snprintf(buff, sizeof(buff), "%13s\t%+13s%+13s%+13s%+13s%+13s%+13s",
		"hitsetkey",
		"alpha",
		"beta",
		"gamma",
		"x",
		"y",
		"z"
	);
	file << buff << std::endl;

	Intt::Offline_s ofl;
	Intt::Online_s onl;
	onl.lyr = 0;
	onl.ldr = 0;
	while(true)
	{
		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		isr.ReadFile(sensor_path + buff);

		ladder_to_global = ilr.GetLadderTransform(onl);

		printf("\n\n");
		printf("%s\n", buff);
		printf("lyr: %d\n", onl.lyr);
		printf("ldr: %d\n", onl.ldr);
		printf("\tladder:\n");
		ladder_to_global.Print();
		printf("\n");

		ofl = Intt::ToOffline(onl);
		for(ofl.ladder_z = 0; ofl.ladder_z < 4; ++ofl.ladder_z)
		{
			key = InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0);
			sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);
			sensor_to_global = ladder_to_global * sensor_to_ladder;
			sensor_to_global.SetAnglesFromTransform();

			snprintf(buff, sizeof(buff), "%13d\t%+13.5f%+13.5f%+13.5f%+13.5f%+13.5f%+13.5f",
				key,
				sensor_to_global.Ang(0),
				sensor_to_global.Ang(1),
				sensor_to_global.Ang(2),
				sensor_to_global.Pos(0),
				sensor_to_global.Pos(1),
				sensor_to_global.Pos(2)
			);
			file << buff << std::endl;

			//Do things
			printf("\tladder_z: %d\n", ofl.ladder_z);
			sensor_to_global.Print();
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
	file.close();

	return 0;
}
