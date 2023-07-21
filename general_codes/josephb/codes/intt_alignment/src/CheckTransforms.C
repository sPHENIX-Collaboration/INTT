#ifndef CHECK_TRANSFORMS_C
#define CHECK_TRANSFORMS_C

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "TVector3.h"
#include "TCanvas.h"

R__LOAD_LIBRARY(libintt.so)
#include <intt/InttMapping.h>

std::string path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment";

struct Transform_s
{
	TVector3 x_axis;
	TVector3 y_axis;
	TVector3 z_axis;
	TVector3 offset;

	Intt::Offline_s ofl;
};
typedef std::map<std::string, Transform_s> Map_t;
Map_t transforms;

void ReadFile(std::string const&);
void PrintEntry(Map_t::const_iterator const&);

void CheckTransforms()
{
	printf("\n");

	ReadFile(path + "/dat/output.txt");
	for(Map_t::const_iterator itr = transforms.begin(); itr != transforms.end(); ++itr)
	{
		if(itr->first.find("_") == std::string::npos)printf("\n");
		PrintEntry(itr);
	}

	printf("\n");
}

void ReadFile(std::string const& filename)
{
	transforms.clear();

	int i;
	float f[4];
	char buff[256];
	std::string name;
	std::string temp;
	Map_t::iterator itr;
	Intt::Online_s onl;
	Intt::Offline_s ofl;

	std::string line;
	std::ifstream file;

	if(filename.empty())
	{
		printf("\tArgument \"filename\" is empty string\n");

		return;
	}

	file.open(filename.c_str(), std::ios_base::in);

	if(!file.good())
	{
		printf("\tCould not produce good stream from file:\n");
		printf("\t%s\n", filename.c_str());

		return;
	}

	for(line; std::getline(file, line);)
	{
		if(line.find("B") != std::string::npos)
		{
			name = line.substr(line.find("B"), line.find("."));

			std::getline(file, line);
			if(sscanf(line.c_str(), "%*s %d", &i) < 1)continue;
			onl.lyr = i;

			std::getline(file, line);
			if(sscanf(line.c_str(), "%*s %d", &i) < 1)continue;
			onl.ldr = i;

			ofl = Intt::ToOffline(onl);

			continue;
		}

		if(line.find("ladder:") != std::string::npos)
		{
			transforms[name] = (Transform_s){};
			itr = transforms.find(name);

			ofl.ladder_z = -1;

			for(i = 0; i < 3; ++i)
			{
				std::getline(file, line);
				if(sscanf(line.c_str(), "%f %f %f %f", &(f[0]), &(f[1]), &(f[2]), &(f[3])) < 4)
				{
					printf("Bad read:\nline:\n%s\n\n", line.c_str());
				}

				itr->second.x_axis[i] = f[0];
				itr->second.y_axis[i] = f[1];
				itr->second.z_axis[i] = f[2];
				itr->second.offset[i] = f[3];
				itr->second.ofl = ofl;
			}

			continue;
		}

		if(line.find("ladder_z:") != std::string::npos)
		{
			sscanf(line.c_str(), "\tladder_z: %d", &i);
			ofl.ladder_z = i;

			temp = name + "_" + i;
			transforms[temp] = (Transform_s){};
			itr = transforms.find(temp);

			for(i = 0; i < 3; ++i)
			{
				std::getline(file, line);
				if(sscanf(line.c_str(), "%f %f %f %f", &(f[0]), &(f[1]), &(f[2]), &(f[3])) < 4)
				{
					printf("Bad read:\nline:\n%s\n\n", line.c_str());
				}

				itr->second.x_axis[i] = f[0];
				itr->second.y_axis[i] = f[1];
				itr->second.z_axis[i] = f[2];
				itr->second.offset[i] = f[3];
				itr->second.ofl = ofl;
			}

			continue;
		}
	}
}

void PrintEntry(Map_t::const_iterator const& itr)
{
	printf("%-9s (%-12s%3d %-12s%3d %-12s%3d)\n", (itr->first + ":").c_str(), "layer:", itr->second.ofl.layer, "ladder_phi:", itr->second.ofl.ladder_phi, "ladder_z:", itr->second.ofl.ladder_z);
	printf("\tx axis: %+13.8f %+13.8f %+13.8f\n", itr->second.x_axis[0], itr->second.x_axis[1], itr->second.x_axis[2]);
	printf("\ty axis: %+13.8f %+13.8f %+13.8f\n", itr->second.y_axis[0], itr->second.y_axis[1], itr->second.y_axis[2]);
	printf("\tz axis: %+13.8f %+13.8f %+13.8f\n", itr->second.z_axis[0], itr->second.z_axis[1], itr->second.z_axis[2]);
	printf("\toffset: %+13.8f %+13.8f %+13.8f\n", itr->second.offset[0], itr->second.offset[1], itr->second.offset[2]);

}

#endif//CHECK_TRANSFORMS_C
