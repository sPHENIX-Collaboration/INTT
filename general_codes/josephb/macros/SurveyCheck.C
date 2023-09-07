#ifndef SURVEY_CHECK_C
#define SURVEY_CHECK_C

#include <cstddef>
#include <cstdio>
#include <cstdint>

#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/SVD>

#include <intt/InttMapping.h>
R__LOAD_LIBRARY(libintt.so)

Double_t scale = 0.008;
Double_t width = 0.06;
Double_t height = 0.02;

Double_t radius = 3.0;

Double_t text_size = 0.06;
Double_t key_space = 0.0;//0.06;

Double_t T_MRGN = 0.0;
Double_t B_MRGN = 0.0;
Double_t L_MRGN = 0.0;
Double_t R_MRGN = 0.0;

Color_t GetColor(int const);
int DrawLadder();
int DrawBarrel();

int SurveyCheck()
{
	DrawLadder();
	//DrawBarrel();
	
	return EXIT_SUCCESS;
}

int DrawLadder()
{
	Double_t x_pts[4] = {0.0, 0.0, 0.0, 0.0};
	Double_t y_pts[4] = {0.0, 0.0, 0.0, 0.0};

	Eigen::Vector4d v;

	Eigen::Vector4d O = {0.0, 0.0, 0.0, 1.0};
	Eigen::Vector4d X = {1.0, 0.0, 0.0, 0.0};
	Eigen::Vector4d Y = {0.0, 1.0, 0.0, 0.0};

	TCanvas* cnvs = new TCanvas("ladder_cnvs", "ladder_cnvs");
	cnvs->Range(0.0, 0.0, 1.0, 1.0);
	cnvs->SetTopMargin(T_MRGN);
	cnvs->SetBottomMargin(B_MRGN);
	cnvs->SetLeftMargin(L_MRGN);
	cnvs->SetRightMargin(R_MRGN);

	TPad* dot_pad = new TPad("dot_pad", "dot_pad", 0.0, 0.0, 0.8, 1.0);
	dot_pad->Range(-492.0 / 2.0 - radius, -19.968 / 2.0 - radius, 492.0 / 2.0 + radius, 19.968 / 2.0 + radius);
	dot_pad->SetTopMargin(T_MRGN);
	dot_pad->SetBottomMargin(B_MRGN);
	dot_pad->SetLeftMargin(L_MRGN);
	dot_pad->SetRightMargin(R_MRGN);
	cnvs->cd();
	dot_pad->Draw();
	dot_pad->cd();

	struct Intt::Online_s onl;
	struct Intt::Offline_s ofl;
	ofl.layer = 4;
	ofl.ladder_phi = 0;
	ofl.ladder_z = 0;
	ofl.strip_x = 0;
	ofl.strip_y = 0;
	while(true)
	{
		v = Intt::GetLocalPos(ofl);
		onl = Intt::ToOnline(ofl);

		switch(ofl.ladder_z)
		{
			case 0:
			v(2) -= 65.55;
			break;
			case 1:
			v(2) -= 181.72;
			break;
			case 2:
			v(2) += 65.55;
			break;
			case 3:
			v(2) += 181.72;
			break;
			default:
			break;
		}

		TEllipse* dot = new TEllipse(v(2), v(0), radius, radius);
		std::cout << "x: " << v(2) << "\ty: " << v(0) << std::endl;
		dot->SetFillColor(GetColor(onl.chp));
		dot->SetLineColor(kBlack);
		dot->SetLineWidth(1);
		dot->Draw("lf");

		//++ofl.strip_x;
		ofl.strip_x += 255;
		if(ofl.strip_x < 256)continue;
		ofl.strip_x = 0;

		++ofl.strip_y;
		if(ofl.strip_y < (ofl.ladder_z % 2 ? 5 : 8))continue;
		ofl.strip_y = 0;

		++ofl.ladder_z;
		if(ofl.ladder_z < 4)continue;
		ofl.ladder_z = 0;

		break;
	}

	TPad* key_pad = new TPad("key_pad", "key_pad", 0.8, 0.0, 1.0, 1.0);
	key_pad->Range(0.0, 0.0, 1.0, 1.0);
	key_pad->SetTopMargin(T_MRGN);
	key_pad->SetBottomMargin(B_MRGN);
	key_pad->SetLeftMargin(L_MRGN);
	key_pad->SetRightMargin(R_MRGN);
	cnvs->cd();
	key_pad->Draw();
	key_pad->cd();

	for(int i = 0; i < 16; ++i)
	{
		O = Eigen::Vector4d(0.5 - key_space, (2.0 * i + 1.0) / 33.0, 0.0, 0.0);
		
		x_pts[0] = (O - width * X - height * Y)(0);
		y_pts[0] = (O - width * X - height * Y)(1);
                            
		x_pts[1] = (O + width * X - height * Y)(0);
		y_pts[1] = (O + width * X - height * Y)(1);
                            
		x_pts[2] = (O + width * X + height * Y)(0);
		y_pts[2] = (O + width * X + height * Y)(1);
                            
		x_pts[3] = (O - width * X + height * Y)(0);
		y_pts[3] = (O - width * X + height * Y)(1);

		TPolyLine* box = new TPolyLine(4, x_pts, y_pts);
		box->SetFillColor(GetColor(i));
		box->SetLineColor(kBlack);
		box->SetLineWidth(1);
		box->Draw("f");

		TText* text = new TText(O(0) + key_space, O(1), Form("%2d", i));
		text->SetTextAlign(22);
		text->SetTextSize(text_size);
		text->Draw();
	}

	return EXIT_SUCCESS;
}

int DrawBarrel()
{
	Double_t x_pts[4] = {0.0, 0.0, 0.0, 0.0};
	Double_t y_pts[4] = {0.0, 0.0, 0.0, 0.0};

	Eigen::Affine3d t;
	Eigen::Vector4d o = {0.0, 0.0, 0.0, 1.0};
	Eigen::Vector4d x = {1.0, 0.0, 0.0, 0.0};
	Eigen::Vector4d y = {0.0, 1.0, 0.0, 0.0};

	Eigen::Vector4d O;
	Eigen::Vector4d X;
	Eigen::Vector4d Y;

	TCanvas* cnvs = new TCanvas("barrel_cnvs", "barrel_cnvs");
	cnvs->Range(0.0, 0.0, 1.0, 1.0);
	cnvs->SetTopMargin(T_MRGN);
	cnvs->SetBottomMargin(B_MRGN);
	cnvs->SetLeftMargin(L_MRGN);
	cnvs->SetRightMargin(R_MRGN);

	TPad* box_pad = new TPad("box_pad", "box_pad", 0.0, 0.0, 0.8, 1.0);
	box_pad->Range(-1.0, -1.0, 1.0, 1.0);
	box_pad->SetTopMargin(T_MRGN);
	box_pad->SetBottomMargin(B_MRGN);
	box_pad->SetLeftMargin(L_MRGN);
	box_pad->SetRightMargin(R_MRGN);
	cnvs->cd();
	box_pad->Draw();
	box_pad->cd();

	struct Intt::Offline_s ofl;
	ofl.layer = 3;
	ofl.ladder_phi = 0;
	ofl.ladder_z = 0;
	while(true)
	{
		t = Intt::GetTransform(ofl);
		O = t * o;
		X = t * x;
		Y = t * y;

		x_pts[0] = (scale * O - width * X - height * Y)(0);
		y_pts[0] = (scale * O - width * X - height * Y)(1);
                            
		x_pts[1] = (scale * O + width * X - height * Y)(0);
		y_pts[1] = (scale * O + width * X - height * Y)(1);
                            
		x_pts[2] = (scale * O + width * X + height * Y)(0);
		y_pts[2] = (scale * O + width * X + height * Y)(1);
                            
		x_pts[3] = (scale * O - width * X + height * Y)(0);
		y_pts[3] = (scale * O - width * X + height * Y)(1);

		std::cout << "\t" << x_pts[0] << ", " << y_pts[0] << " : " << x_pts[2] << ", " << y_pts[2] << std::endl;

		TPolyLine* box = new TPolyLine(4, x_pts, y_pts);
		box->SetFillColor(GetColor(ofl.ladder_phi));
		box->SetLineColor(kBlack);
		box->SetLineWidth(1);
		box->Draw("f");

		++ofl.ladder_phi;
		if(ofl.ladder_phi < (ofl.layer < 5 ? 12 : 16))continue;
		ofl.ladder_phi = 0;

		++ofl.layer;
		if(ofl.layer < 7)continue;
		ofl.layer = 0;

		break;
	}

	TPad* key_pad = new TPad("key_pad", "key_pad", 0.8, 0.0, 1.0, 1.0);
	key_pad->Range(0.0, 0.0, 1.0, 1.0);
	key_pad->SetTopMargin(T_MRGN);
	key_pad->SetBottomMargin(B_MRGN);
	key_pad->SetLeftMargin(L_MRGN);
	key_pad->SetRightMargin(R_MRGN);
	cnvs->cd();
	key_pad->Draw();
	key_pad->cd();

	X = x;
	Y = y;
	for(int i = 0; i < 16; ++i)
	{
		O = Eigen::Vector4d(0.5 - key_space, (2.0 * i + 1.0) / 33.0, 0.0, 0.0);
		
		x_pts[0] = (O - width * X - height * Y)(0);
		y_pts[0] = (O - width * X - height * Y)(1);
                            
		x_pts[1] = (O + width * X - height * Y)(0);
		y_pts[1] = (O + width * X - height * Y)(1);
                            
		x_pts[2] = (O + width * X + height * Y)(0);
		y_pts[2] = (O + width * X + height * Y)(1);
                            
		x_pts[3] = (O - width * X + height * Y)(0);
		y_pts[3] = (O - width * X + height * Y)(1);

		TPolyLine* box = new TPolyLine(4, x_pts, y_pts);
		box->SetFillColor(GetColor(i));
		box->SetLineColor(kBlack);
		box->SetLineWidth(1);
		box->Draw("f");

		TText* text = new TText(O(0) + key_space, O(1), Form("%2d", i));
		text->SetTextAlign(22);
		text->SetTextSize(text_size);
		text->Draw();
	}

	return EXIT_SUCCESS;
}

Color_t GetColor(int const i)
{
	Color_t c = kRed;

	switch(i / 4)
	{
		case 0:
		c = kRed;
		break;

		case 1:
		c = kYellow;
		break;	

		case 2:
		c = kBlue;
		break;	

		case 3:
		c = kGreen;
		break;	

		default:
		break;
	}

	c += i % 4;

	return c;
}

#endif//SURVEY_CHECK_C
