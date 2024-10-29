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

std::string sensor_path = "../dat/sensor_survey_data/";
std::string ladder_path = "../dat/";

std::string hpp_file = "../dat/InttSurveyMap.h";
std::string cpp_file = "../dat/InttSurveyMap.cc";

int MakeCPP();
int MakeHPP();

int main()
{
	MakeCPP();
	MakeHPP();

	return 0;
}

int MakeCPP()
{
	FILE* cpp_f = fopen(cpp_file.c_str(), "w");

	fprintf(cpp_f, "#include \"InttSurveyMap.h\"\n");
	fprintf(cpp_f, "\n");
	fprintf(cpp_f, "Eigen::Affine3d InttSurvey::GetTransform(struct InttNameSpace::Offline_s const& ofl)\n");
	fprintf(cpp_f, "{\n");
	fprintf(cpp_f, "\tEigen::Affine3d t;\n");
	fprintf(cpp_f, "\n");

	InttLadderReader ilr;
	ilr.SetMarksDefault();
	ilr.ReadFile(ladder_path + "EAST_INTT.txt");
	ilr.ReadFile(ladder_path + "WEST_INTT.txt");

	InttSensorReader isr;
	isr.SetMarksDefault();

	char buff[256];
	TrkrDefs::hitsetkey key;
	Eigen::Affine3d sensor_to_ladder;
	Eigen::Affine3d ladder_to_global;
	Eigen::Affine3d sensor_to_global;

	InttNameSpace::Offline_s ofl;
	InttNameSpace::Online_s onl;

	goto LOOP;
	while(true)
	{
		++ofl.ladder_z;
		if(ofl.ladder_z < 4)goto LADDER_Z_INC;
		fprintf(cpp_f, "\t\t\t\tdefault:\n");
		fprintf(cpp_f, "\t\t\t\treturn t;\n");
		fprintf(cpp_f, "\t\t\t}\n");

		++ofl.ladder_phi;
		if(ofl.ladder_phi < (ofl.layer < 5 ? 12 : 16))goto LADDER_PHI_INC;
		fprintf(cpp_f, "\t\t\tdefault:\n");
		fprintf(cpp_f, "\t\t\treturn t;\n");
		fprintf(cpp_f, "\t\t}\n");

		++ofl.layer;
		if(ofl.layer < 7)goto LAYER_INC;
		fprintf(cpp_f, "\t\tdefault:\n");
		fprintf(cpp_f, "\t\treturn t;\n");
		fprintf(cpp_f, "\t}\n");

		break;

		LOOP:
		fprintf(cpp_f, "\tswitch (ofl.layer) {\n");

		ofl.layer = 3;
		LAYER_INC:
		fprintf(cpp_f, "\t\tcase %2d:\n", ofl.layer);
		fprintf(cpp_f, "\t\tswitch (ofl.ladder_phi) {\n");

		ofl.ladder_phi = 0;
		LADDER_PHI_INC:
		fprintf(cpp_f, "\t\t\tcase %2d:\n", ofl.ladder_phi);
		fprintf(cpp_f, "\t\t\tswitch (ofl.ladder_z) {\n");

		onl = InttNameSpace::ToOnline(ofl);
		ladder_to_global = ilr.GetLadderTransform(onl);
		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		isr.ReadFile(sensor_path + buff);

		ofl.ladder_z = 0;
		LADDER_Z_INC:
		fprintf(cpp_f, "\t\t\t\tcase %2d:\n", ofl.ladder_z);

		key = InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0);
		sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);
		sensor_to_global = ladder_to_global * sensor_to_ladder;

		fprintf(cpp_f, "\t\t\t\tt.matrix() = Eigen::Matrix4d{\n");
		fprintf(cpp_f, "\t\t\t\t\t{%+14.9lf, %+14.9lf, %+14.9lf, %+14.9lf},\n", sensor_to_global.matrix()(0, 0), sensor_to_global.matrix()(0, 1), sensor_to_global.matrix()(0, 2), sensor_to_global.matrix()(0, 3));
		fprintf(cpp_f, "\t\t\t\t\t{%+14.9lf, %+14.9lf, %+14.9lf, %+14.9lf},\n", sensor_to_global.matrix()(1, 0), sensor_to_global.matrix()(1, 1), sensor_to_global.matrix()(1, 2), sensor_to_global.matrix()(1, 3));
		fprintf(cpp_f, "\t\t\t\t\t{%+14.9lf, %+14.9lf, %+14.9lf, %+14.9lf},\n", sensor_to_global.matrix()(2, 0), sensor_to_global.matrix()(2, 1), sensor_to_global.matrix()(2, 2), sensor_to_global.matrix()(2, 3));
		fprintf(cpp_f, "\t\t\t\t\t{%+14.9lf, %+14.9lf, %+14.9lf, %+14.9lf}};\n", sensor_to_global.matrix()(3, 0), sensor_to_global.matrix()(3, 1), sensor_to_global.matrix()(3, 2), sensor_to_global.matrix()(3, 3));
		//...
		fprintf(cpp_f, "\t\t\t\treturn t;\n", ofl.ladder_z);
	}

	fprintf(cpp_f, "\n");
	fprintf(cpp_f, "\treturn t;\n");
	fprintf(cpp_f, "}\n");

	fclose(cpp_f);

	return 0;
}

int MakeHPP()
{
	FILE* hpp_f = fopen(hpp_file.c_str(), "w");

	fprintf(hpp_f, "#ifndef INTT_SURVEY_MAP_H\n");
	fprintf(hpp_f, "#define INTT_SURVEY_MAP_H\n");
	fprintf(hpp_f, "\n");
	fprintf(hpp_f, "#include \"InttMapping.h\"\n");
	fprintf(hpp_f, "\n");
	fprintf(hpp_f, "#include <Eigen/Dense>\n");
	fprintf(hpp_f, "#include <Eigen/Geometry>\n");
	fprintf(hpp_f, "#include <Eigen/LU>\n");
	fprintf(hpp_f, "#include <Eigen/SVD>\n");
	fprintf(hpp_f, "\n");
	fprintf(hpp_f, "namespace InttSurvey\n");
	fprintf(hpp_f, "{\n");
	fprintf(hpp_f, "\tEigen::Affine3d GetTransform(struct InttNameSpace::Offline_s const&);\n");
	fprintf(hpp_f, "}\n");
	fprintf(hpp_f, "\n");
	fprintf(hpp_f, "#endif//INTT_SURVEY_MAP_H\n");

	fclose(hpp_f);

	return 0;
}
