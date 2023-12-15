#ifndef CHANNEL_CLASSIFIER_HELPER_H
#define CHANNEL_CLASSIFIER_HELPER_H

#include <climits>
#include <cstddef>
#include <cstdint>

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TTree.h>

#include <intt/InttMapping.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/InttDefs.h>

class ChannelClassifierHelper
{
public:
	//...
	enum FitParams_e : uint8_t
	{
		i_avg = 0,
		i_sig,	
		i_rho,

		NUM_PAR
	};
	const static FitParams_e FitParamsArr[];

	struct FitParams_s
	{
		int fit_flag = 1;

		TFile* fit_file = nullptr;

		TTree* fit_tree = nullptr;
			Double_t hits = 0;
			Double_t nll = 0;

			InttNameSpace::Online_s onl;
			InttNameSpace::Offline_s ofl;

			TrkrDefs::hitsetkey hit_set_key = 0;
			TrkrDefs::hitkey hit_key = 0;
			int flx_bco = 0;

		TH1* fit_hist = nullptr;
			Double_t min_hits = INT_MAX;
			Double_t max_hits = 0;

		TF1* fit_func = nullptr;
			Double_t norm = 1.0;

		TFile* out_file = nullptr;

		TTree* out_tree = nullptr;
			int status = 0;

		std::map<Double_t, TF1*> terms;
	};

	struct DbgParams_s
	{
		bool verbose = false;
		std::string fit_plot_name = "";
		std::string pre_fit_plot_name = "";
		std::string post_fit_plot_name = "";
		std::string fit_mode = "";
	};
	//...

	ChannelClassifierHelper();
	~ChannelClassifierHelper();

	Double_t static SumGauss(Double_t*, Double_t*);
	uint8_t static ParIndex(uint8_t const&, FitParams_e const&);

	void SetNumSig(uint8_t const&);
	void SetNumAcceptionTerms(uint8_t const&);
	void SetNumRejectionTerms(uint8_t const&);
	void SetNumBins(Long64_t const&);

	void SetVerbose(bool const&);
	void SetFitPlotName(std::string const&);
	void SetPreFitPlotName(std::string const&);
	void SetPostFitPlotName(std::string const&);
	void SetFitMode(std::string const&);

	int FitFile(std::string const&, std::string const&);
	int Classify(std::string const&, std::string const&);
	//...

protected:
	void GetOfflineFromHit();
	void GetHitFromOffline();

	Double_t GetAdjustedHit(Long64_t const&);
	Double_t GetAdjustedNll(Long64_t const&);

	void FillHist();

	void FitEntire();
	void FitTerm(uint8_t const&);

	void ClassifyHit(Long64_t const&);
	void ClassifyEntire();

	void PrintParams(uint8_t const&);
	void DrawFit(std::string const&);
	void DrawFit(std::string const&, uint8_t const&, bool const&);
	//...

private:
	FitParams_s params;
	DbgParams_s debug;

	uint8_t num_sig = 5;
	uint8_t terms_to_accept = 2;
	uint8_t terms_to_reject = 2;
	Int_t num_bins = 100;
	//...

};

#endif//CHANNEL_CLASSIFIER_HELPER_H
