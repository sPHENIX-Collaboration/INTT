#ifndef CONVENTION_CHECK_C
#define CONVENTION_CHECK_C

#include "TH2D.h"

#include <cstdio>
#include <iostream>
#include <string>

#include <intt/InttMapping.h>
R__LOAD_LIBRARY(libintt.so)



void SetStyle();

typedef Double_t(*FillFunc_t)(struct Intt::Online_s const&);
TH2D* MakeLadderHist(std::string const&, FillFunc_t);
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

	TCanvas* online_chp_cnvs = new TCanvas("online_chp_canvas", "online_chp_canvas");
	online_chp_cnvs->cd();
	TH2D* online_chp_hist = MakeLadderHist("online_chp_hist", &GetChp);
	online_chp_hist->DrawCopy("COLZ");
	delete online_chp_hist;

	TCanvas* online_chn_cnvs = new TCanvas("online_chn_canvas", "online_chn_canvas");
	online_chn_cnvs->cd();
	TH2D* online_chn_hist = MakeLadderHist("online_chn_hist", &GetChn);
	online_chn_hist->DrawCopy("COLZ");
	delete online_chn_hist;
}



void SetStyle()
{
	gStyle->SetOptStat(0);
}

TH2D* MakeLadderHist(std::string const& name, FillFunc_t func)
{
	SetStyle();

	TH2D* hist = new TH2D
	(
		name.c_str(),
		name.c_str(),
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
		hist->SetBinContent(bin, (*func)(s));

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

	return hist;
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
