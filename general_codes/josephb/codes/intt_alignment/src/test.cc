#include "eigen/InttSensorReader.h"
#include "eigen/InttLadderReader.h"
#include "eigen/Debug.h"

#include <intt/InttMapping.h>
#include <trackbase/InttDefs.h>

#include <cstdio>
#include <iostream>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

bool verbose = false;

std::string sensor_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";
std::string ladder_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/";

std::string output_file = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/transforms.txt";

std::string inner_file = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/local_params_inner.txt";
std::string outer_file = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/local_params_outer.txt";
std::string debug_file = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/josborn/macrosForJoseph/detectors/sPHENIX/localAlignmentParamsFile.txt";

void append_inner(std::fstream&);
void append_outer(std::fstream&);

int main()
{
	std::fstream debug_stream;
	debug_stream.open(debug_file.c_str(), std::ios_base::out | std::ios_base::trunc);
	append_inner(debug_stream);

	InttLadderReader ilr;
	ilr.SetMarksDefault();
	ilr.ReadFile(ladder_path + "EAST_INTT.txt");
	ilr.ReadFile(ladder_path + "WEST_INTT.txt");

	InttSensorReader isr;
	isr.SetMarksDefault();

	char buff[256];
	double a;
	double b;
	double g;
	TrkrDefs::hitsetkey key;
	Eigen::Affine3d sensor_to_ladder;
	Eigen::Affine3d ladder_to_global;
	Eigen::Affine3d sensor_to_global;

	Intt::Offline_s ofl;
	Intt::Online_s onl;
	onl.lyr = 0;
	onl.ldr = 0;
	while(true)
	{
		ofl = Intt::ToOffline(onl);

		ladder_to_global = ilr.GetLadderTransform(onl);
		if(verbose)std::cout << "lyr: " << onl.lyr << "ldr: " << onl.ldr << std::endl;

		if(verbose)
		{
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
		}

		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		isr.ReadFile(sensor_path + buff);

		for(ofl.ladder_z = 0; ofl.ladder_z < 4; ++ofl.ladder_z)
		{
			key = InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0);
			sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);
			sensor_to_global = ladder_to_global * sensor_to_ladder;
			Debug::AnglesFromTransform(a, b, g, sensor_to_global);

			debug_stream << key << " ";
			debug_stream << sensor_to_global.translation()(0) << " ";
			debug_stream << sensor_to_global.translation()(1) << " ";
			debug_stream << sensor_to_global.translation()(2) << " ";
			debug_stream << 0.0 << " ";
			debug_stream << 0.0 << " ";
			debug_stream << g;
			debug_stream << std::endl;

			if(verbose)
			{
				std::cout << "lyr: " << onl.lyr << "ldr: " << onl.ldr << "ladder_z: " << ofl.ladder_z << std::endl;
				std::cout << "linear:" << std::endl;
				std::cout << sensor_to_ladder.linear() << std::endl;
				std::cout << std::endl;
				std::cout << "translation:" << std::endl;
				std::cout << sensor_to_ladder.translation() << std::endl;
				std::cout << std::endl;
			}
		}

		++onl.ldr;
		if(onl.ldr < (onl.lyr < 2 ? 12 : 16))continue;
		onl.ldr = 0;

		++onl.lyr;
		if(onl.lyr < 4)continue;
		onl.lyr = 0;

		break;
	}

	append_outer(debug_stream);

	return 0;
}

void append_inner(std::fstream& o_file)
{
	if(!o_file.good())
	{
		std::cout << "void append_outer(std::fstream& o_file)" << std::endl;
		std::cout << "\tPassed std::fstream not good()" << std::endl;
		std::cout << "Nothing prepended" << std::endl;
		std::cout << std::endl;

		return;
	}

	std::fstream i_file;
	i_file.open(inner_file.c_str(), std::ios_base::in);
	if(!i_file.good())
	{
		std::cout << "void append_inner(std::fstream& o_file)" << std::endl;
		std::cout << "\tCould not for good in stream from file:" << std::endl;
		std::cout << inner_file << std::endl;
		std::cout << "Nothing prepended" << std::endl;
		std::cout << std::endl;

		return;
	}

	std::cout << "Prepending contents of file:" << std::endl;
	std::cout << "\t" << inner_file << std::endl;

	std::string line;
	for(line; std::getline(i_file, line);)
	{
		o_file << line << std::endl;
	}
}

void append_outer(std::fstream& o_file)
{
	if(!o_file.good())
	{
		std::cout << "\tPassed std::fstream not good()" << std::endl;
		std::cout << "Nothing prepended" << std::endl;
		std::cout << std::endl;

		return;
	}

	std::fstream i_file;
	i_file.open(outer_file.c_str(), std::ios_base::in);
	if(!i_file.good())
	{
		std::cout << "void append_inner(std::fstream& o_file)" << std::endl;
		std::cout << "\tCould not for good in stream from file:" << std::endl;
		std::cout << inner_file << std::endl;
		std::cout << "Nothing prepended" << std::endl;
		std::cout << std::endl;
	}

	std::cout << "Appending contents of file:" << std::endl;
	std::cout << "\t" << outer_file << std::endl;

	std::string line;
	for(line; std::getline(i_file, line);)
	{
		o_file << line << std::endl;
	}
}
