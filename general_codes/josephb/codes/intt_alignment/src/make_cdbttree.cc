#include "eigen/InttSensorReader.h"
#include "eigen/InttLadderReader.h"
#include "eigen/Debug.h"

#include <cdbobjects/CDBTTree.h>
#include <intt/InttMap.h>
#include <intt/InttMapping.h>

#include <cstdio>
#include <iostream>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

const float GEANT_SHIFT = 0.2282; // mm
const float ENDCAP_SHIFT = 2.395; // mm

std::string sensor_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/sensor_survey_data/";
std::string ladder_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/";
std::string cdbttree_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/intt_survey_cdbttree.root";

struct write_entry_s {
	CDBTTree* cdbttree = nullptr;
	InttNameSpace::Offline_s const* ofl = nullptr;
	Eigen::Affine3d const* abs_eigen_trans = nullptr;
	Eigen::Affine3d const* rel_eigen_trans = nullptr;
	int* n = nullptr;
};
void write_entry(write_entry_s const&);

int main() {
	InttLadderReader ilr;
	ilr.SetMarksDefault();
	ilr.ReadFile(ladder_path + "EAST_INTT.txt");
	ilr.ReadFile(ladder_path + "WEST_INTT.txt");

	InttSensorReader isr;
	isr.SetMarksDefault();

	char buff[256];
	Eigen::Affine3d sensor_to_ladder;
	Eigen::Affine3d ladder_to_global;
	Eigen::Affine3d sensor_to_global;
	Eigen::Affine3d identity;
	for(int i = 0; i < 16; ++i) {
		identity.matrix()(i / 4, i % 4) = (i / 4) == (i % 4);
	}

	InttNameSpace::Offline_s ofl;
	InttNameSpace::Online_s onl;

	CDBTTree cdbttree(cdbttree_path);
	int size = 0;

	goto LOOP;
	while(true) {
		if (++ofl.ladder_z < 4)goto LADDER_Z_INC;
		if (++ofl.ladder_phi < (ofl.layer < 5 ? 12 : 16))goto LADDER_PHI_INC;
		if (++ofl.layer < 7)goto LAYER_INC;
		break;

		LOOP:
		ofl.layer = 3;

		LAYER_INC:
		ofl.ladder_phi = 0;

		LADDER_PHI_INC:
		onl = InttNameSpace::ToOnline(ofl);
		ladder_to_global = ilr.GetLadderTransform(onl);
	
		// This transform treats the ladder's origin as the center of its face
		// GEANT require the ladder's origin at the center of its volume
		if(true) {
			Eigen::Vector3d y_axis (
				ladder_to_global.matrix()(0, 1),
				ladder_to_global.matrix()(1, 1),
				ladder_to_global.matrix()(2, 1)
			);
			y_axis /= y_axis.norm();
			for(int i = 0; i < 3; ++i) {
				// y-axis points radially inward
				// so += is a radially inward shift
				ladder_to_global.matrix()(i, 3) += y_axis(i) * GEANT_SHIFT;
			}
		}
		ofl.ladder_z = InttMap::Wildcard;
		write_entry ({
			.cdbttree = &cdbttree,
			.ofl = &ofl,
			.abs_eigen_trans = &ladder_to_global,
			.rel_eigen_trans = &identity,
			.n = &size
		});
		// The sensor transforms still assume, however, that the ladder's origin is the center of its face
		// Furthermore, there is an additional shift due to endcap hole thickness
		//     (Opposite sides were surveyed, for sensor and post-installation)
		// Undo previous shift and also shift by endcap thickness
		if(true) {
			Eigen::Vector3d y_axis (
				ladder_to_global.matrix()(0, 1),
				ladder_to_global.matrix()(1, 1),
				ladder_to_global.matrix()(2, 1)
			);
			y_axis /= y_axis.norm();
			for(int i = 0; i < 3; ++i) {
				// y-axis points radially inward
				// so += is a radially inward shift
				ladder_to_global.matrix()(i, 3) -= y_axis(i) * GEANT_SHIFT;
				ladder_to_global.matrix()(i, 3) += y_axis(i) * ENDCAP_SHIFT;
			}
		}

		snprintf(buff, sizeof(buff), "B%01dL%03d.txt", onl.lyr / 2, (onl.lyr % 2) * 100 + onl.ldr);
		isr.ReadFile(sensor_path + buff);
		ofl.ladder_z = 0;

		LADDER_Z_INC:
		sensor_to_ladder = isr.GetSensorTransform(ofl.ladder_z);
		sensor_to_global = ladder_to_global * sensor_to_ladder;
		write_entry ({
			.cdbttree = &cdbttree,
			.ofl = &ofl,
			.abs_eigen_trans = &sensor_to_global,
			.rel_eigen_trans = &identity,
			.n = &size
		});
	}

	cdbttree.SetSingleIntValue("size", size);
	cdbttree.Commit();
	cdbttree.CommitSingle();
	cdbttree.WriteCDBTTree();

	return 0;
}

void write_entry (
	write_entry_s const& args
) {
	args.cdbttree->SetIntValue(*args.n,	"layer",	args.ofl->layer);
	args.cdbttree->SetIntValue(*args.n,	"ladder_phi",	args.ofl->ladder_phi);
	args.cdbttree->SetIntValue(*args.n,	"ladder_z",	args.ofl->ladder_z);
	args.cdbttree->SetIntValue(*args.n,	"strip_phi",	InttMap::Wildcard);
	args.cdbttree->SetIntValue(*args.n,	"strip_z",	InttMap::Wildcard);

	char buff[32];
	for(int i = 0; i < 16; ++i) {
		snprintf(buff, sizeof(buff), "m_abs_%01d_%01d", i / 4, i % 4);
		args.cdbttree->SetDoubleValue(*args.n, buff, args.abs_eigen_trans->matrix()(i / 4, i % 4));
	}
	for(int i = 0; i < 16; ++i) {
		snprintf(buff, sizeof(buff), "m_rel_%01d_%01d", i / 4, i % 4);
		args.cdbttree->SetDoubleValue(*args.n, buff, args.rel_eigen_trans->matrix()(i / 4, i % 4));
	}

	++*args.n;
}
