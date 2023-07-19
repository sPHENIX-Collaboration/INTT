
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"

#include "RooFit.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooGenericPdf.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/DataLoader.h"


TTree* tree = nullptr;
RooGaussian* gauss = nullptr;
RooGenericPdf* model = nullptr;
RooFitResult* result = nullptr;

void Setup();
void DoFit();

int main()
{
	Setup();
	printf("before:\n");
	printf("%-5s %8.4lf\n", "mean:", ((RooRealVar*)&(gauss->getMean()))->getValV());
	printf("%-5s %8.4lf\n", "sigma:", ((RooRealVar*)&(gauss->getSigma()))->getValV());

	DoFit();
	printf("after:\n");
	printf("%-5s %8.4lf\n", "mean:", ((RooRealVar*)&(gauss->getMean()))->getValV());
	printf("%-5s %8.4lf\n", "sigma:", ((RooRealVar*)&(gauss->getSigma()))->getValV());

	return 0;
}

void Setup()
{
	Double_t mu = 32.0;
	Double_t sg = 8.0;

	gauss = new RooGaussian
	(
		"gauss", "gauss",
		*(new RooRealVar("gtm", "gtm", -DBL_MAX, DBL_MAX)),
		*(new RooRealVar("mu", "mu", -DBL_MAX, DBL_MAX)),
		*(new RooRealVar("sg", "sg", 0, DBL_MAX))
	);

	((RooRealVar*)&(gauss->getMean()))->setVal(mu + 1.1);
	((RooRealVar*)&(gauss->getSigma()))->setVal(sg - 0.5);

	tree = new TTree("tree", "tree");

	Double_t gtm;
	tree->Branch("gtm", &gtm);
	TRandom rng;
	rng.SetSeed(1234);

	for(Long64_t n = 0; n < 1000; ++n)
	{
		gtm = rng.Gaus(mu, sg);
		if(36 < gtm && gtm < 40)continue;
		tree->Fill();
	}
}

void DoFit()
{
	RooDataSet data
	(
		"data", "data",
		RooArgSet((RooRealVar&)(gauss->getX())),
		RooFit::Import(*tree)
	);

	RooMsgService::instance().setSilentMode(true);
	result = gauss->fitTo(data, RooFit::PrintEvalErrors(-1));
}
