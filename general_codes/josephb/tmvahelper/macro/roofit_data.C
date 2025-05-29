#ifndef ROOFIT_DATA_C
#define ROOFIT_DATA_C

#include "config.C"

#include <tmvahelper/TMVAHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <filesystem>
#include <limits>
#include <boost/format.hpp>

void
roofit_data (
) {
	// Helper
	TMVAHelper tmva_helper;
	tmva_helper.read_branches(config::branches);
	tmva_helper.read_training(config::training);
	tmva_helper.read_cuts(config::cuts);

	TTree* selection_tree = TMVAHelper::get_tree("selection.root", "DecayTree");
	if (!selection_tree) {
		std::cerr << "expected file 'selection.root' not present" << std::endl;
		return;
	}

	RooRealVar mass(config::mass_branch.c_str(), config::mass_branch.c_str(), config::min_mass, config::max_mass);
	RooDataSet ds("ds", "ds", RooArgSet(mass), RooFit::Import(*selection_tree));

	int chebychev_deg = 2;
	RooArgList chebychev_coefficients;

	RooRealVar** chebychev_coeff = new RooRealVar*[chebychev_deg];
	for (int n = 0; n < chebychev_deg; ++n) {
		std::string name;
		name = (boost::format("chebychev_coeff_%d") % n).str();
		chebychev_coeff[n] = new RooRealVar(name.c_str(), name.c_str(), 1.0);
		chebychev_coeff[n]->setConstant(kFALSE);
		chebychev_coefficients.addOwned(*chebychev_coeff[n]);
	}
	RooChebychev chebychev("chebychev", "chebychev", mass, chebychev_coefficients);
	chebychev.fitTo(ds);
	for (int n = 0; n < chebychev_deg; ++n) {
		chebychev_coeff[n]->setConstant(kTRUE);
	}

	TFile* file = TFile::Open("signal_fit.root", "READ");
	if (!file) {
		std::cerr << "expected file 'signal_fit.root' not present" << std::endl;
		return;
	}
	RooWorkspace* w = dynamic_cast<RooWorkspace*>(file->Get("w"));
	w->Print();
	RooAddPdf* sum_gauss = dynamic_cast<RooAddPdf*>(w->pdf("sum_gauss"));
	for (auto& var : w->allVars()) {
		RooRealVar* real_var = dynamic_cast<RooRealVar*>(var);
		std::cout << real_var->GetTitle() << ": " << real_var->getValV() << std::endl;
		if (std::string{real_var->GetTitle()} == config::mass_branch) continue;

		real_var->setConstant(kTRUE);
	}

	RooRealVar signal_count("signal_count", "signal_count", 0.0, 0.0, ds.numEntries());
	RooRealVar background_count("background_count", "background_count", ds.numEntries(), 0.0, ds.numEntries());
	RooAddPdf model("model", "model", RooArgList(*sum_gauss, chebychev), RooArgList(signal_count, background_count));
	model.fitTo(ds);

	Double_t sigma_eff = 0;
	Double_t sum_coeff = 0;
	for (int n = 0; n < 2; ++n) {
		std::string name;

		name = (boost::format("coeff_%d") % n).str();
		RooRealVar* coeff = dynamic_cast<RooRealVar*>(w->arg(name.c_str()));

		name = (boost::format("sigma_%d") % n).str();
		RooRealVar* sigma = dynamic_cast<RooRealVar*>(w->arg(name.c_str()));

		if (!coeff || !sigma) break;

		sigma_eff += coeff->getValV() * sigma->getValV() * sigma->getValV();
		sum_coeff += coeff->getValV();
	}
	sigma_eff /= sum_coeff;
	sigma_eff = sqrt(sigma_eff);

	Double_t bin_width = 3.49 * sigma_eff / pow(signal_count.getValV(), 0.3333); // Freedman-Diaconis rule
	Int_t num_bins = (config::max_mass - config::min_mass) / bin_width;
	std::cout << "num bins: " << num_bins << std::endl;

	RooPlot* frame = mass.frame();
	ds.plotOn(frame, RooFit::Binning(num_bins));
	model.plotOn(frame, RooFit::LineColor(kRed));
	chebychev.plotOn(frame, RooFit::LineColor(kBlue));

	std::string cnvs_name = (boost::format("%s_background_fit.png") % config::mass_branch.c_str()).str();
	TCanvas* cnvs = new TCanvas(cnvs_name.c_str(), cnvs_name.c_str(), 600, 800);
	cnvs->cd();
	frame->Draw();
	cnvs->Show();
	cnvs->SaveAs(cnvs_name.c_str());
}

#endif//ROOFIT_DATA_C
