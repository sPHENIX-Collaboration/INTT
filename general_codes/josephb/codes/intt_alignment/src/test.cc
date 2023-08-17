#include "eigen/InttSensorReader.h"
#include "eigen/InttLadderReader.h"

#include <intt/InttMapping.h>
#include <trackbase/InttDefs.h>

#include <cstdio>
#include <iostream>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>


std::string sensor_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";
std::string ladder_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/";

std::string output_file = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/transforms.txt";

int main()
{
	char buff[256];
	TrkrDefs::hitsetkey key;
	Eigen::Affine3d sensor_to_ladder;
	Eigen::Affine3d ladder_to_global;
	Eigen::Affine3d sensor_to_global;

	InttLadderReader ilr;
	ilr.SetMarksDefault();
	ilr.ReadFile(ladder_path + "EAST_INTT.txt");
	ilr.ReadFile(ladder_path + "WEST_INTT.txt");

	InttSensorReader isr;
	isr.SetMarksDefault();

	Intt::Offline_s ofl;
	Intt::Online_s onl;
	onl.lyr = 0;
	onl.ldr = 0;
	while(true)
	{
		ofl = Intt::ToOffline(onl);

		ladder_to_global = ilr.GetLadderTransform(onl);
		std::cout << "lyr: " << onl.lyr << "ldr: " << onl.ldr << std::endl;

		Eigen::Quaternion<double> q(ladder_to_global.rotation());
		Eigen::AngleAxis<double> aa(q);
		std::cout << "angle:" << aa.angle() / 3.141592653589793238 << "pi" << std::endl;
		std::cout << "axis:" << std::endl;
		std::cout << aa.axis() << std::endl;

		std::cout << "linear:" << std::endl;
		std::cout << ladder_to_global.linear() << std::endl;
		std::cout << std::endl;
		std::cout << "translation:" << std::endl;
		std::cout << ladder_to_global.translation() << std::endl;
		std::cout << std::endl;

		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		isr.ReadFile(sensor_path + buff);

		for(ofl.ladder_z = 0; ofl.ladder_z < 4; ++ofl.ladder_z)
		{
			key = InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0);
			sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);
			sensor_to_global = ladder_to_global * sensor_to_ladder;

			//std::cout << "lyr: " << onl.lyr << "ldr: " << onl.ldr << "ladder_z: " << ofl.ladder_z << std::endl;
			//std::cout << "linear:" << std::endl;
			//std::cout << sensor_to_ladder.linear() << std::endl;
			//std::cout << std::endl;
			//std::cout << "translation:" << std::endl;
			//std::cout << sensor_to_ladder.translation() << std::endl;
			//std::cout << std::endl;
		}

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
