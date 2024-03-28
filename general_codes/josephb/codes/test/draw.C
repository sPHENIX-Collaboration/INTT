#ifndef DRAW_C
#define DRAW_C

#include <iostream>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

#include <intt/InttMap.h>

#include <intt/InttSurveyMap.h>
#include <intt/InttSurveyMapv1.h>

R__LOAD_LIBRARY(libintt.so)

void draw() {
	std::unique_ptr<InttSurveyMap> survey = std::make_unique<InttSurveyMapv1>();
	if(survey->LoadFromFile("intt_survey_cdb.root"))return;

	Eigen::Affine3d const* t = nullptr;
	InttMap::Offline_s ofl = {
		.layer = 3,
		.ladder_phi = 0,
		.ladder_z = InttMap::Wildcard,
		.strip_phi = InttMap::Wildcard,
		.strip_z = InttMap::Wildcard,
	};

	double width = 10;	// really half the true width
	double height = 2.5;	// really half the true height 
	double def_corners_x[4] = {-width, width, width, -width};
	double def_corners_y[4] = {-height, -height, height, height};

	double c, s, x, y, corners_x[4], corners_y[4];
	TPolyLine* box = nullptr;

	TCanvas* cnvs = new TCanvas("cnvs", "cnvs");
	cnvs->Range(-200, -200, 200, 200);
	cnvs->Draw();
	cnvs->cd();

	while(true) {
		t = survey->GetAbsoluteTransform(ofl);
		if(!t) {
			std::cout << "Couldn't get transform" << std::endl;
			std::cout << "\tlayer:      " << ofl.layer << std::endl;
			std::cout << "\tladder_phi: " << ofl.ladder_phi << std::endl;
			goto INC;
		}

		c = t->matrix()(0, 0); // [ [ cos, -sin ]
		s = t->matrix()(1, 0); //   [ sin,  cos ] ]
		x = t->matrix()(0, 3);
		y = t->matrix()(1, 3);

		for(int i = 0; i < 4; ++i) {
			corners_x[i] = x + c * def_corners_x[i] - s * def_corners_y[i];
			corners_y[i] = y + s * def_corners_x[i] + c * def_corners_y[i];
		}

		box = new TPolyLine(4, corners_x, corners_y);
		box->SetFillColor(kRed);
		box->Draw("f");

		INC:
		if(++ofl.ladder_phi < (ofl.layer < 5  ? 12 : 16))continue;
		ofl.ladder_phi = 0;
		if(++ofl.layer < 7)continue;
		ofl.layer = 3;
		break;
	}

	box = new TPolyLine(4, corners_x, corners_y);
	box->SetFillColor(kRed);
	box->Draw("f");
	std::cout << "here" << std::endl;
}

#endif//DRAW_C
