#ifndef DEBUG_TRANSFORMS_C
#define DEBUG_TRANSFORMS_C

#include <iostream>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

#include <TFile.h>
#include <TTree.h>
#include <Math/Transform3D.h>

R__LOAD_LIBRARY(libintt.so)
#include <intt/InttMapping.h>

std::string file_full_path = "/sphenix/u/jbertaux/sphnx_software/INTT/general_codes/josephb/codes/intt_alignment/dat/intt_transforms.root";

void DebugTransforms()
{
	TFile* file = TFile::Open(file_full_path.c_str(), "READ");
	if(!file)
	{
		std::cout << "Could not get file:" << std::endl;
		std::cout << "\t" << file_full_path << std::endl;
		std::cout << "Exiting" << std::endl;

		return;
	}

	TTree* tree = (TTree*)file->Get("intt_transforms");
	if(!tree)
	{
		std::cout << "Could not get tree:" << std::endl;
		std::cout << "\t" << "intt_transforms" << std::endl;
		std::cout << "From file:" << std::endl;
		std::cout << "\t" << file_full_path << std::endl;

		return;
	}

	Int_t hitsetkey;
	ROOT::Math::Transform3D* transform = new ROOT::Math::Transform3D();

	tree->SetBranchAddress("hitsetkey", &hitsetkey);
	tree->SetBranchAddress("transform", &transform);

	Intt::Offline_s s;
	s.layer = 4;
	s.ladder_phi = 6;
	s.ladder_z = 3;

	Eigen::Affine3d t = Intt::GetTransform(tree, s);
	std::cout << t.matrix() << std::endl;
}

#endif//DEBUG_TRANSFORMS_C
