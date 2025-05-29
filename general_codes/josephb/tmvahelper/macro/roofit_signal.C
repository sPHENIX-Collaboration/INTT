#ifndef ROOFIT_SIGNAL_C
#define ROOFIT_SIGNAL_C

#include "config.C"

#include <tmvahelper/TmvaHelper.h>
R__LOAD_LIBRARY(libtmvahelper.so)

#include <filesystem>
#include <boost/format.hpp>

void
roofit_signal (
) {
	// Helper
	TmvaHelper tmva_helper;
	tmva_helper.read_branches(config::branches);
	tmva_helper.read_training(config::training);
	tmva_helper.read_cuts(config::cuts);

	tmva_helper.show();

	TTree* signal_tree = TmvaHelper::get_tree("signal.root", "DecayTree");
	if (!signal_tree) {
		std::cerr << "expected file 'signal.root' not present" << std::endl;
		return;
	}

	RooRealVar mass(config::mass_branch.c_str(), config::mass_branch.c_str(), config::min_mass, config::max_mass);
	RooDataSet ds("ds", "ds", RooArgSet(mass), RooFit::Import(*signal_tree));

	RooRealVar mean("mean", "mean", config::mean, config::min_mass, config::max_mass);

	int num_gauss = 2;
	RooArgList coefficients;
	RooArgList gaussians;

	RooRealVar** sigma = new RooRealVar*[num_gauss];
	RooRealVar** coeff = new RooRealVar*[num_gauss];
	RooGaussian** gauss = new RooGaussian*[num_gauss];
	for (int n = 0; n < num_gauss; ++n) {
		std::string name;
		name = (boost::format("sigma_%d") % n).str();
		sigma[n] = new RooRealVar(name.c_str(), name.c_str(), config::sig / (n + 1.0), 0.0, config::max_mass);

		name = (boost::format("coeff_%d") % n).str();
		coeff[n] = new RooRealVar(name.c_str(), name.c_str(), ds.numEntries() / num_gauss, 0.0, ds.numEntries());
		coefficients.addOwned(*coeff[n]);

		name = (boost::format("gauss_%d") % n).str();
		gauss[n] = new RooGaussian(name.c_str(), name.c_str(), mass, mean, *sigma[n]);
		gaussians.addOwned(*gauss[n]);
	}

	RooAddPdf sum_gauss("sum_gauss", "sum_gauss", gaussians, coefficients);
	sum_gauss.fitTo(ds);

	Double_t sigma_eff = 0;
	Double_t sum_coeff = 0;
	for (int n = 0; n < num_gauss; ++n) {
		sigma_eff += coeff[n]->getValV() * sigma[n]->getValV() * sigma[n]->getValV();
		sum_coeff += coeff[n]->getValV();
	}
	sigma_eff /= sum_coeff;
	sigma_eff = sqrt(sigma_eff);

	Double_t bin_width = 3.49 * sigma_eff / pow(sum_coeff, 0.3333); // Freedman-Diaconis rule
	Int_t num_bins = (config::max_mass - config::min_mass) / bin_width;

	RooPlot* frame = mass.frame();
	ds.plotOn(frame, RooFit::Binning(num_bins));
	sum_gauss.plotOn(frame);

	std::string cnvs_name = (boost::format("%s_signal_fit.png") % config::mass_branch.c_str()).str();
	TCanvas* cnvs = new TCanvas(cnvs_name.c_str(), cnvs_name.c_str(), 600, 800);
	cnvs->cd();
	frame->Draw();
	cnvs->Show();
	cnvs->SaveAs(cnvs_name.c_str());

	std::cout << "coeff[0]: " << coeff[0]->getValV() << std::endl;
	std::cout << "coeff[1]: " << coeff[1]->getValV() << std::endl;
	std::cout << "entries: " << ds.numEntries() << std::endl;
	std::cout << "bins: " << num_bins  << std::endl;

	RooWorkspace* w = new RooWorkspace("w", "w");
	w->import(sum_gauss);
	w->writeToFile("signal_fit.root");
}

#endif//ROOFIT_SIGNAL_C
