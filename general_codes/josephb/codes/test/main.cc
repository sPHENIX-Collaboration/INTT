#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <filesystem>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include <intt/InttMap.h>
#include <intt/InttMaskedChannelSet.h>
#include <intt/InttMaskedChannelSetv1.h>
#include <intt/InttSurveyMap.h>
#include <intt/InttSurveyMapv1.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

#include <TFile.h>
#include <TTree.h>
#include <Math/Transform3D.h>

#include <cdbobjects/CDBTTree.h>

int make_cdb();
int check_cdb();

int main() {
	// make_cdb();
	check_cdb();
	return 0;
}

int check_cdb() {
	std::string filename = "intt_transforms.root";
	std::string treename = "intt_transforms";

	std::string cdbttree = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/intt_survey_cdbttree.root";
	// std::string cdbttree = "intt_survey_cdb.root";

	if(!std::filesystem::exists(filename)) {
		std::cout << "Path: " << filename << std::endl;
		std::cout << "Does not exist" << std::endl;
		return 0;
	}
	TFile* file = TFile::Open(filename.c_str(), "READ");
	if(!file) {
		std::cout << "Couldn't open file: " << filename << std::endl;
		return 0;
	}
	TTree* tree = (TTree*)file->Get(treename.c_str());
	if(!tree) {
		std::cout << "Couldn't get tree: " << treename << std::endl;
		std::cout << "From file: " << filename << std::endl;
	}

	Eigen::Affine3d abs_eigen_trans;
	ROOT::Math::Transform3D* root_trans = new ROOT::Math::Transform3D();
	InttMap::Offline_s ofl;

	tree->SetBranchAddress("transform",     &root_trans);
	tree->SetBranchAddress("layer",         &ofl.layer);
	tree->SetBranchAddress("ladder_phi",    &ofl.ladder_phi);
	tree->SetBranchAddress("ladder_z",      &ofl.ladder_z);

	InttSurveyMap* sm = new InttSurveyMapv1();
	if(sm->LoadFromFile(cdbttree))return 1;

	std::vector<Int_t> checks = {7, 42, 111, 212};
	for(auto const& i : checks) {
		tree->GetEntry(i);

		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << ofl.layer << "\t" << ofl.ladder_phi << "\t" << "\t" << ofl.ladder_z << std::endl;
		root_trans->GetTransformMatrix(abs_eigen_trans);
		std::cout << "From TFile:" << std::endl;
		std::cout << abs_eigen_trans.matrix() << std::endl;
		std::cout << "From CDBTTree:" << std::endl;
		std::cout << sm->GetAbsoluteTransform(ofl)->matrix() << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;

	delete root_trans;
	delete sm;

	return 0;
}


int make_cdb() {
	std::string filename = "intt_transforms.root";
	std::string treename = "intt_transforms";
	if(!std::filesystem::exists(filename)) {
		std::cout << "Path: " << filename << std::endl;
		std::cout << "Does not exist" << std::endl;
		return 0;
	}
	TFile* file = TFile::Open(filename.c_str(), "READ");
	if(!file) {
		std::cout << "Couldn't open file: " << filename << std::endl;
		return 0;
	}
	TTree* tree = (TTree*)file->Get(treename.c_str());
	if(!tree) {
		std::cout << "Couldn't get tree: " << treename << std::endl;
		std::cout << "From file: " << filename << std::endl;
	}

	Eigen::Affine3d abs_eigen_trans;
	Eigen::Affine3d rel_eigen_trans;

	for(int i = 0; i < 16; ++i) {
		rel_eigen_trans.matrix()(i / 4, i % 4) = i / 4 == i % 4 ? 1 : 0;
	}

	ROOT::Math::Transform3D* root_trans = new ROOT::Math::Transform3D();
	InttMap::Offline_s ofl;
	char buff[64];
	Float_t m[16];

	tree->SetBranchAddress("transform",     &root_trans);
	tree->SetBranchAddress("layer",         &ofl.layer);
	tree->SetBranchAddress("ladder_phi",    &ofl.ladder_phi);
	tree->SetBranchAddress("ladder_z",      &ofl.ladder_z);

	CDBTTree cdbttree("intt_survey_cdb.root");

	Int_t n = 0;
	for(n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		ofl.strip_phi = InttMap::Wildcard;
		ofl.strip_z = InttMap::Wildcard;

		root_trans->GetTransformMatrix(abs_eigen_trans);

		cdbttree.SetIntValue(n, "layer",        ofl.layer);
		cdbttree.SetIntValue(n, "ladder_phi",   ofl.ladder_phi);
		cdbttree.SetIntValue(n, "ladder_z",     ofl.ladder_z);
		cdbttree.SetIntValue(n, "strip_phi",    ofl.strip_phi);
		cdbttree.SetIntValue(n, "strip_z",      ofl.strip_z);

		for(int i = 0; i < 16; ++i) {
			snprintf(buff, sizeof(buff), "m_rel_%01d_%01d", i / 4, i % 4);
			cdbttree.SetDoubleValue(n, buff, rel_eigen_trans.matrix()(i / 4, i % 4));
		}
		for(int i = 0; i < 16; ++i) {
			snprintf(buff, sizeof(buff), "m_abs_%01d_%01d", i / 4, i % 4);
			cdbttree.SetDoubleValue(n, buff, abs_eigen_trans.matrix()(i / 4, i % 4));
		}

		// std::cout << "layer: " << ofl.layer <<
		// 	"\tladder_phi: " << ofl.ladder_phi <<
		// 	"\tladder_z: " << ofl.ladder_z
		// << std::endl;
		// std::cout << eigen_trans.matrix() << std::endl;
		// std::cout << std::endl;
	}
	cdbttree.SetSingleIntValue("size", n);

	cdbttree.Commit();
	cdbttree.CommitSingle();
	cdbttree.WriteCDBTTree();

	file->Close();
	delete root_trans;

	return 0;
}

