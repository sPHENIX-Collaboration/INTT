#ifndef CHANNEL_CLASSIFIER_H
#define CHANNEL_CLASSIFIER_H

#include <intt/InttMapping.h>

#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdio>

#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TTree.h>

//#include <RooAddPdf.h>
//#include <RooArgList.h>
//#include <RooDataSet.h>
//#include <RooFit.h>
//#include <RooGaussian.h>
//#include <RooPlot.h>
//#include <RooRealVar.h>

class ChannelClassifier
{
public:
	ChannelClassifier();
	~ChannelClassifier();

	Int_t constexpr static NUM_CHANNELS = 56 * 52 * 128;
	Int_t const static NUM_GAUSS = 3;
	Int_t const static NUM_SIG = 5;
	Int_t const static NUM_BINS = 100;

	struct StatParams_s
	{
		Long64_t avg = 0;
		Double_t sig = 0;
		Long64_t min = 0;
		Long64_t max = 0;
		Double_t rho = 0;
	};
	struct InitParams_s
	{
		Double_t m = 0;
		Double_t s = 0;
		Double_t c = 0;
	};
	struct ModeMap_s
	{
		std::size_t s = 0;
		void(*func)(InitParams_s&, StatParams_s const&) = nullptr;
	};
	typedef std::map<std::string, ModeMap_s> ModeMap_t;

	static void ColdInit(InitParams_s&, StatParams_s const&);
	static void HalfInit(InitParams_s&, StatParams_s const&);
	static void GoodInit(InitParams_s&, StatParams_s const&);
	static void WarmInit(InitParams_s&, StatParams_s const&);

	void Reset();

	InttNameSpace::Online_s static struct_from_hash(Int_t);
	Int_t static hash_from_struct(struct InttNameSpace::Online_s const&);
	Int_t static hash_from_struct(struct InttNameSpace::Offline_s const&);
	Int_t static hash_from_struct(struct InttNameSpace::RawData_s const&);

	void static print(InttNameSpace::Online_s const&);

	Double_t static SumGauss(Double_t* x, Double_t* par)
	{
		Double_t d = 0;

		for(std::size_t s = 0; s < par[0]; ++s)
		{
			d += par[3 * s + 3] * TMath::Gaus(*x, par[3 * s + 1], par[3 * s + 2], kTRUE);
		}

		return d;
	}

	int AppendFile(std::string const&);
	int OutputHits(std::string const&);

	int PoissonFit(std::string const&);

protected:
	std::string i_treename = "prdf_tree";
	std::string o_treename = "channel_tree";

	Long64_t* hits;
	ModeMap_t mode_map;
};

#endif//CHANNEL_CLASSIFIER_H
