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

#include <TFile.h>
#include <TTree.h>
#include <Math/Transform3D.h>

bool verbose = false;

const float ENDCAP_SHIFT = 2.395; // mm
const float GEANT_SHIFT = 0.2282; // mm

std::string sensor_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";
std::string ladder_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/";

std::string root_file = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/intt_transforms.root";

int main()
{
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
	InttNameSpace::RawData_s raw;
	InttNameSpace::Online_s onl;

	Int_t k;
	ROOT::Math::Transform3D t;
	Int_t sensor_type;
	Double_t x;
	Double_t y;
	Double_t z;
	Double_t a;
	Double_t b;
	Double_t c;

	TFile* file = TFile::Open(root_file.c_str(), "RECREATE");
	file->cd();
	TTree* tree = new TTree("intt_transforms", "intt_transforms");
	tree->SetDirectory(file);
	tree->Branch("hitsetkey", &k);
	tree->Branch("transform", &t);
	tree->Branch("layer", &(ofl.layer));
	tree->Branch("ladder_phi", &(ofl.ladder_phi));
	tree->Branch("ladder_z", &(ofl.ladder_z));
	tree->Branch("server", &(raw.felix_server));
	tree->Branch("module", &(raw.felix_channel));
	tree->Branch("sensor_type", &sensor_type);
	tree->Branch("x", &x);
	tree->Branch("y", &y);
	tree->Branch("z", &z);
	tree->Branch("a", &a);
	tree->Branch("b", &b);
	tree->Branch("c", &c);

	goto LOOP;
	while(true)
	{
		k = key;
		t.SetTransformMatrix(sensor_to_global);
		tree->Fill();

		//------------------------------//
		//	increment logic:	//
		//------------------------------//

		if(++ofl.ladder_z < 4)goto LADDER_Z_INC;
		if(++ofl.ladder_phi < (ofl.layer < 5 ? 12 : 16))goto LADDER_PHI_INC;
		if(++ofl.layer < 7)goto LAYER_INC;

		break;

		LOOP:
		ofl.layer = 3;

		LAYER_INC:
		ofl.ladder_phi = 0;

		LADDER_PHI_INC:
		onl = InttNameSpace::ToOnline(ofl);
		raw = InttNameSpace::ToRawData(ofl);
		ladder_to_global = ilr.GetLadderTransform(onl);

		// survey finds the center of ladder face
		// preferable to use center of ladder volume; shift radially outward
		if(0 < GEANT_SHIFT) {
			Eigen::Vector3d y_axis (
				ladder_to_global.matrix()(0, 1),
				ladder_to_global.matrix()(1, 1),
				ladder_to_global.matrix()(2, 1)
			);
			y_axis /= y_axis.norm();
			for(int i = 0; i < 3; ++i)
			{
				// y-axis points radially inward
				// -= for a radially outward shift (GEANT_SHIFT is positive if valid)
				ladder_to_global.matrix()(i, 3) -= y_axis(i) * GEANT_SHIFT;
			}
		}

		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		isr.ReadFile(sensor_path + buff);
		ofl.ladder_z = 0;

		LADDER_Z_INC:
		key = InttDefs::genHitSetKey(ofl.layer, ofl.ladder_z, ofl.ladder_phi, 0);
		sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);
		sensor_to_global = ladder_to_global * sensor_to_ladder;

		// ladder endcaps are surveyed at different sides for sensor and post-installation
		// need to shift by thickness of endcap holes
		if(0 < ENDCAP_SHIFT) {
			Eigen::Vector3d y_axis (
				ladder_to_global.matrix()(0, 1),
				ladder_to_global.matrix()(1, 1),
				ladder_to_global.matrix()(2, 1)
			);
			y_axis /= y_axis.norm();
			for(int i = 0; i < 3; ++i)
			{
				// y-axis points radially inward
				// += for a radially inward shift (ENDCAP_SHIFT is positive if valid)
				sensor_to_global.matrix()(i, 3) += y_axis(i) * ENDCAP_SHIFT;
			}
		}

		switch(ofl.ladder_z)
		{
			case 0:
			sensor_type = 0;
			break;

			case 1:
			sensor_type = 1;
			break;

			case 2:
			sensor_type = 0;
			break;

			case 3:
			sensor_type = 1;
			break;

			default:
			break;
		}

		x = sensor_to_global.matrix()(0, 3);
		y = sensor_to_global.matrix()(1, 3);
		z = sensor_to_global.matrix()(2, 3);
		Debug::AnglesFromTransform(a, b, c, sensor_to_global);
	}

	tree->Write();
	file->Write();
	file->Close();

	return 0;
}
