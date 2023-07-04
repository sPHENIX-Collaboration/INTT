#ifndef CONVENTION_CHECK_C
#define CONVENTION_CHECK_C

#include "TH2D.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <intt/InttMapping.h>
R__LOAD_LIBRARY(libintt.so)



Double_t T_MRGN = 0.1;
Double_t B_MRGN = 0.1;
Double_t L_MRGN = 0.1;
Double_t R_MRGN = 0.1;

void SetStyle();

typedef Double_t(*FillFunc_t)(struct Intt::Online_s const&);

struct Params_s
{
	std::string name;
	std::string description;
	FillFunc_t func;
};
void MakeLadderHist(struct Params_s const&);

Double_t GetChp(struct Intt::Online_s const& s){return s.chp;}
Double_t GetChn(struct Intt::Online_s const& s){return s.chn;}
Double_t GetStripX(struct Intt::Online_s const& s){return Intt::ToOffline(s).strip_x;}
Double_t GetStripY(struct Intt::Online_s const& s){return Intt::ToOffline(s).strip_y;}
Double_t GetLadderZ(struct Intt::Online_s const& s){return Intt::ToOffline(s).ladder_z;}


void InttToBin(struct Intt::Online_s const&, int&, int&);
void BinToIntt(struct Intt::Online_s&, int, int);



void ConventionCheck()
{
	SetStyle();

	std::vector<struct Params_s> v =
	{
		(struct Params_s){.name = "online_chp", .description = "Online Chip", .func = &GetChp},
		(struct Params_s){.name = "online_chn", .description = "Online Channel", .func = &GetChn},
		(struct Params_s){.name = "offline_strip_x", .description = "Offline Strip X", .func = &GetStripX},
		(struct Params_s){.name = "offline_strip_y", .description = "Offline Strip Y", .func = &GetStripY},
		(struct Params_s){.name = "offline_ladder_z", .description = "Offline Ladder Z", .func = &GetLadderZ},
	};

	for(std::vector<struct Params_s>::const_iterator itr = v.begin(); itr != v.end(); ++itr)MakeLadderHist(*itr);
}



void SetStyle()
{
	gStyle->SetOptStat(0);
}

void MakeLadderHist(struct Params_s const& params)
{
	TCanvas* cnvs = new TCanvas(params.name.c_str(), params.name.c_str());
	cnvs->Range(0.0, 0.0, 1.0, 1.0);
	cnvs->SetTopMargin(T_MRGN);
	cnvs->SetBottomMargin(B_MRGN);
	cnvs->SetLeftMargin(L_MRGN);
	cnvs->SetRightMargin(R_MRGN);

	TPad* hist_pad = new TPad((params.name + "_hist_pad").c_str(), (params.name + "_hist_pad").c_str(), 0.0, 0.0, 1.0, 1.0);
	hist_pad->SetFillStyle(4000);
	hist_pad->Range(0.0, 0.0, 1.0, 1.0);
	hist_pad->SetTopMargin(T_MRGN);
	hist_pad->SetBottomMargin(B_MRGN);
	hist_pad->SetLeftMargin(L_MRGN);
	hist_pad->SetRightMargin(R_MRGN);
	cnvs->cd();
	hist_pad->Draw();
	hist_pad->cd();

	TH2D* hist = new TH2D
	(
		params.name.c_str(),
		params.description.c_str(),
		26,
		-0.5,
		25.5,
		256,
		-0.5,
		255.5
	);
	hist->SetMinimum(-1);

	int bin = 0;
	int bin_x = 0;
	int bin_y = 0;
	Intt::Online_s s;
	s.lyr = 0;
	s.ldr = 0;
	s.arm = 0;
	s.chp = 0;
	s.chn = 0;

	while(true)
	{
		InttToBin(s, bin_x, bin_y);
		bin = hist->GetBin(bin_x, bin_y);
		hist->SetBinContent(bin, (*params.func)(s));

		++s.chn;
		if(s.chn < 128)continue;
		s.chn = 0;

		++s.chp;
		if(s.chp < 26)continue;
		s.chp = 0;

		++s.arm;
		if(s.arm < 2)continue;
		s.arm = 0;

		break;
	}

	cnvs->cd();
	hist->DrawCopy("COLZ");
	delete hist;

	TPad* grid_pad = new TPad((params.name + "_grid_pad").c_str(), (params.name + "_grid_pad").c_str(), 0.0, 0.0, 1.0, 1.0);
	grid_pad->SetFillStyle(4000);
	grid_pad->Range(0.0, 0.0, 1.0, 1.0);
	grid_pad->SetTopMargin(T_MRGN);
	grid_pad->SetBottomMargin(B_MRGN);
	grid_pad->SetLeftMargin(L_MRGN);
	grid_pad->SetRightMargin(R_MRGN);
	cnvs->cd();
	grid_pad->Draw();
	grid_pad->cd();

	Double_t temp = 0.0;
	Double_t lower = 0.0;
	Double_t upper = 1.0;
	for(int i = 0; i <= 26; ++i)
	{
		temp = R_MRGN + i * (1.0 - L_MRGN - R_MRGN) / 26.0;

		lower = B_MRGN;
		upper = 1.0 - T_MRGN;

		TLine* line = new TLine(temp, lower, temp, upper);
		line->SetLineStyle(i % 13 == 0 ? 1 : 3);
		line->SetLineWidth(i % 13 == 0 ? 2 : 1);
		line->Draw();
	}
	for(int i = 0; i <= 256; i += 4)
	{
		temp = B_MRGN + i * (1.0 - T_MRGN - B_MRGN) / 256.0;

		lower = L_MRGN;
		upper = 1.0 - R_MRGN;

		TLine* line = new TLine(lower, temp, upper, temp);
		line->SetLineStyle(i % 128 == 0 ? 1 : 3);
		line->SetLineWidth(i % 128 == 0 ? 2 : 1);
		line->Draw();
	}
}

void InttToBin(struct Intt::Online_s const& s, int& bin_x, int& bin_y)
{
	bin_x = s.arm ? 25 - s.chp % 13 : s.chp % 13;
	bin_y = (s.arm == s.chp / 13) ? 255 - s.chn : s.chn;

	++bin_x;
	++bin_y;
}

void BinToIntt(struct Intt::Online_s& s, int bin_x, int bin_y)
{
	--bin_x;
	--bin_y;

	s.arm = bin_x / 13;
	s.chp = s.arm ? 12 - bin_x % 13 : bin_x % 13;
	s.chn = (s.arm == s.chp / 13) ? 155 - bin_y % 128 : bin_y;
}


//     <- South                                                               North ->     //
//     =======================================+=======================================     //
//  0                                         |                                         0  //
//  v    0  1  2  3  4  5  6  7  8  9 10 11 12|25 24 23 22 21 20 19 18 17 16 15 14 13   v  //
// 128                                        |                                        128 //
//     ---------------------------------------+---------------------------------------     //
// 128                                        |                                        128 //
//  ^   13 14 15 16 17 18 19 20 21 22 23 24 25|12 11 10  9  8  7  6  5  4  3  2  1  0   ^  //
//  0                                         |                                         0  //
//     =======================================+=======================================     //

#endif//CONVENTION_CHECK_C
