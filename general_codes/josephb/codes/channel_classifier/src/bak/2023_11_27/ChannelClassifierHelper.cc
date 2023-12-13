#include "ChannelClassifierHelper.h"

ChannelClassifierHelper::ChannelClassifierHelper()
{
	params.fit_flag = 1;
}

ChannelClassifierHelper::~ChannelClassifierHelper()
{
	if(params.fit_file)params.fit_file->Close();

	if(params.fit_func)delete params.fit_func;
	for(auto itr : params.terms)if(itr.second)delete itr.second;
}

const ChannelClassifierHelper::FitParams_e
ChannelClassifierHelper::FitParamsArr[] =
{
	FitParams_e::i_avg,
	FitParams_e::i_sig,
	FitParams_e::i_rho,
};

Double_t ChannelClassifierHelper::SumGauss(Double_t* var, Double_t* par)
{
	Double_t d = 0;

	for(uint8_t i_term = 0; i_term < par[0]; ++i_term)
	{
		d += par[ParIndex(i_term, FitParams_e::i_rho)] * TMath::Gaus(var[0], par[ParIndex(i_term, FitParams_e::i_avg)], par[ParIndex(i_term, FitParams_e::i_sig)], kTRUE);
	}

	return d;
}

uint8_t
ChannelClassifierHelper::ParIndex(uint8_t const& _i_trm, FitParams_e const& _i_par)
{
	return FitParams_e::NUM_PAR * _i_trm + _i_par + 1;
}
void
ChannelClassifierHelper::SetNumSig(uint8_t const& _n)
{
	num_sig = _n;
}

void
ChannelClassifierHelper::SetVerbose(bool const& _b)
{
	debug.verbose = _b;
}

void
ChannelClassifierHelper::SetNumAcceptionTerms(uint8_t const& _n)
{
	terms_to_accept = _n;
}

void
ChannelClassifierHelper::SetNumRejectionTerms(uint8_t const& _n)
{
	terms_to_reject = _n;
}

void
ChannelClassifierHelper::SetNumBins(Long64_t const& _n)
{
	num_bins = _n;
}

void
ChannelClassifierHelper::SetFitPlotName(std::string const& _name)
{
	debug.fit_plot_name = _name;
}

void
ChannelClassifierHelper::SetPreFitPlotName(std::string const& _name)
{
	debug.pre_fit_plot_name = _name;
}

void
ChannelClassifierHelper::SetPostFitPlotName(std::string const& _name)
{
	debug.post_fit_plot_name = _name;
}

void
ChannelClassifierHelper::SetFitMode(std::string const& _mode)
{
	debug.fit_mode = _mode;
}

int
ChannelClassifierHelper::FitFile(std::string const& _file_name, std::string const& _tree_name)
{
	int return_val = 0;
	std::stringstream err_stream;

	err_stream << "int ChannelClassifierHelper::LoadFile(std::string const& _file_name, std::string const& _tree_name)" << std::endl;

	// here
	// may need to change if format of tree changes
	std::vector<std::string> branch_names =
	{
		"hits",

		"lyr",
		"ldr",
		"arm",
		"chp",
		"chn",
	};

	if(_file_name.empty())
	{
		err_stream << "\tArgument \"_file_name\" is empty string" << std::endl;
		return_val = 1;

		goto LABEL;
	}
	params.fit_file = TFile::Open(_file_name.c_str(), "READ");
	if(!params.fit_file)
	{
		err_stream << "\tCould not open file:" << std::endl;
		err_stream << "\t\t\"" << _file_name << "\"" << std::endl;
		return_val = 1;

		goto LABEL;
	}
	err_stream << "\tIn file:" << std::endl;
	err_stream << "\t\t\"" << _file_name << "\"" << std::endl;

	if(_tree_name.empty())
	{
		err_stream << "\tArgument \"_tree_name\" is empty string" << std::endl;
		return_val = 1;

		goto LABEL;
	}
	params.fit_tree = (TTree*)params.fit_file->Get(_tree_name.c_str());
	if(!params.fit_tree)
	{
		err_stream << "\tCould not get tree:" << std::endl;
		err_stream << "\t\t\"" << _tree_name << "\"" << std::endl;
		return_val = 1;

		goto LABEL;
	}
	err_stream << "\tIn tree:" << std::endl;
	err_stream << "\t\t\"" << _tree_name << "\"" << std::endl;

	for(auto const& branch_name : branch_names)
	{
		if(params.fit_tree->GetBranch(branch_name.c_str()))continue;

		err_stream << "\tFailed to get branch \"" << branch_name << "\"" << std::endl;
		return_val = 1;
	}
	if(return_val)goto LABEL;

	params.fit_tree->SetBranchAddress("hits", &(params.hits));

	// here
	// may need to change if format of tree changes
	params.fit_tree->SetBranchAddress("lyr", &(params.onl.lyr));
	params.fit_tree->SetBranchAddress("ldr", &(params.onl.ldr));
	params.fit_tree->SetBranchAddress("arm", &(params.onl.arm));
	params.fit_tree->SetBranchAddress("chp", &(params.onl.chp));
	params.fit_tree->SetBranchAddress("chn", &(params.onl.chn));

	FillHist();
	FitEntire();

	LABEL:
	err_stream << std::ends;
	if(return_val)std::cout << err_stream.str();
	params.fit_flag = return_val;
	return return_val;
}

int
ChannelClassifierHelper::Classify(std::string const& _file_name, std::string const& _tree_name)
{
	int return_val = 0;
	std::stringstream err_stream;

	err_stream << "int ChannelClassifierHelper::Classify(std::string const& _file_name, std::string const& _tree_name)" << std::endl;
	if(params.fit_flag)
	{
		err_stream << "\tInternal flag set; previous call to:" << std::endl;
		err_stream << "\t\tint ChannelClassifierHelper::FitFile(std::string const& _file_name, std::string const& _tree_name)" << std::endl;
		err_stream << "\tEither:" << std::endl;
		err_stream << "\t\tUnsuccessful call (error description in same output stream), or" << std::endl;
		err_stream << "\t\tNot called on this instance of ChannelClassifierHelper" << std::endl;
		err_stream << "\tNo file written" << std::endl;
		return_val = 1;

		goto LABEL;
	}

	if(_file_name.empty())
	{
		err_stream << "\tArgument \"_file_name\" is empty string" << std::endl;
		return_val = 1;

		goto LABEL;
	}
	params.out_file = TFile::Open(_file_name.c_str(), "RECREATE");
	if(!params.out_file)
	{
		err_stream << "\tFailed to (re)create file:" << std::endl;
		err_stream << "\t\t\"" << _file_name << "\"" << std::endl;
		return_val = 1;

		goto LABEL;
	}
	err_stream << "\tIn file:" << std::endl;
	err_stream << "\t\t\"" << _file_name << "\"" << std::endl;

	if(_tree_name.empty())
	{
		err_stream << "\tArgument \"_tree_name\" is empty string" << std::endl;
		return_val = 1;

		goto LABEL;
	}
	params.out_tree = new TTree(_tree_name.c_str(), _tree_name.c_str());
	params.out_tree->SetDirectory(params.out_file);

	params.out_tree->Branch("hits", &(params.hits));
	params.out_tree->Branch("status", &(params.status));

	// here
	// may need to change if format of tree changes
	params.out_tree->Branch("lyr", &(params.onl.lyr));
	params.out_tree->Branch("ldr", &(params.onl.ldr));
	params.out_tree->Branch("arm", &(params.onl.arm));
	params.out_tree->Branch("chp", &(params.onl.chp));
	params.out_tree->Branch("chn", &(params.onl.chn));

	ClassifyEntire();

	LABEL:
	err_stream << std::ends;
	if(return_val)std::cout << err_stream.str();

	if(params.out_tree)params.out_tree->Write();
	if(params.out_file)params.out_file->Write();
	if(params.out_file)params.out_file->Close();

	return return_val;
}

void
ChannelClassifierHelper::GetOfflineFromHit()
{
	params.ofl.layer = TrkrDefs::getLayer(params.hit_set_key);
	params.ofl.ladder_phi = TrkrDefs::getPhiElement(params.hit_set_key);
	params.ofl.ladder_z = TrkrDefs::getZElement(params.hit_set_key);

	params.ofl.strip_y = InttDefs::getCol(params.hit_key);
	params.ofl.strip_x = InttDefs::getRow(params.hit_key);

	params.flx_bco = InttDefs::getTimeBucketId(params.hit_set_key);
}

void
ChannelClassifierHelper::GetHitFromOffline()
{
	params.hit_set_key = InttDefs::genHitSetKey(params.ofl.layer, params.ofl.ladder_z, params.ofl.ladder_phi, params.flx_bco);
	params.hit_key = InttDefs::genHitKey(params.ofl.strip_y, params.ofl.strip_x);
}

Double_t
ChannelClassifierHelper::GetAdjustedHit(Long64_t const& _n)
{
	params.fit_tree->GetEntry(_n);

	// here
	// may need to change if format of tree changes
	params.ofl = InttNameSpace::ToOffline(params.onl);

	//Noramlize different sensors by length (cm)
	params.hits /= params.ofl.ladder_z % 2 ? 2.0 : 1.6;

	return params.hits;
}

Double_t
ChannelClassifierHelper::GetAdjustedNll(Long64_t const& _n)
{
	GetAdjustedHit(_n);

	if(params.hits < params.min_hits)params.hits = params.min_hits;
	if(params.hits > params.max_hits)params.hits = params.max_hits;

	params.nll = params.fit_func->EvalPar(&(params.hits));
	params.nll = (params.nll > 0) ? -1.0 * log(params.nll / params.norm) : 1.0;

	return params.nll;
}

void
ChannelClassifierHelper::FillHist()
{
	Double_t avg_HIT = 0.0;
	Double_t sig_HIT = 0.0;

	params.min_hits = INT_MAX;
	params.max_hits = 0;

	for(Long64_t n = 0; n < params.fit_tree->GetEntriesFast(); ++n)
	{
		GetAdjustedHit(n);

		avg_HIT += params.hits;

		if(params.hits < params.min_hits)params.min_hits = params.hits;
		if(params.hits > params.max_hits)params.max_hits = params.hits;
	}
	avg_HIT /= params.fit_tree->GetEntriesFast();

	for(Long64_t n = 0; n < params.fit_tree->GetEntriesFast(); ++n)
	{
		GetAdjustedHit(n);

		sig_HIT += (params.hits - avg_HIT) * (params.hits - avg_HIT);
	}
	sig_HIT /= params.fit_tree->GetEntriesFast();
	sig_HIT = sqrt(sig_HIT);

	if(params.min_hits < avg_HIT - num_sig * sig_HIT)params.min_hits = avg_HIT - num_sig * sig_HIT;
	if(params.max_hits > avg_HIT + num_sig * sig_HIT)params.max_hits = avg_HIT + num_sig * sig_HIT;

	if(params.fit_hist)delete params.fit_hist;
	params.fit_hist = new TH1D
	{
		"hitrate_hist",
		"Distribution of Hitrates",
		num_bins,
		params.min_hits,
		params.max_hits
	};

	for(Long64_t n = 0; n < params.fit_tree->GetEntriesFast(); ++n)
	{
		GetAdjustedHit(n);

		if(params.hits < params.min_hits)params.hits = params.min_hits + (params.max_hits - params.min_hits) / num_bins / 2.0;
		if(params.hits > params.max_hits)params.hits = params.max_hits - (params.max_hits - params.min_hits) / num_bins / 2.0;

		params.fit_hist->Fill(params.hits);
	}
}

void
ChannelClassifierHelper::FitEntire()
{
	if(params.fit_func)delete params.fit_func;

	params.fit_func = new TF1
	(
		"hitrate_func",
		ChannelClassifierHelper::SumGauss,
		params.min_hits,
		params.max_hits,
		FitParams_e::NUM_PAR * (terms_to_accept + terms_to_reject) + 1
	);

	params.fit_func->FixParameter(0, terms_to_accept + terms_to_reject);
	for(uint8_t i_term = 0; i_term < params.fit_func->GetParameter(0); ++i_term)
	{
		params.fit_func->FixParameter(ParIndex(i_term, FitParams_e::i_avg), (params.max_hits + params.min_hits) / 2.0);
		params.fit_func->FixParameter(ParIndex(i_term, FitParams_e::i_sig), (params.max_hits - params.min_hits) / 2.0);
		params.fit_func->FixParameter(ParIndex(i_term, FitParams_e::i_rho), 0);
	}

	for(uint8_t i_term = 0; i_term < params.fit_func->GetParameter(0); ++i_term)
	{
		FitTerm(i_term);
	}

	if(!debug.fit_plot_name.empty())
	{
		std::cout << std::endl;
		std::cout << std::endl;
		DrawFit(debug.fit_plot_name.c_str());
	}
}

void
ChannelClassifierHelper::FitTerm(uint8_t const& _i_term)
{
	Double_t avg_NLL = 0;
	Double_t sig_NLL = 0;
	Double_t wgt_NLL = 0;

	params.norm = 0;
	for(uint8_t i_term = 0; i_term < _i_term; ++i_term)
	{
		params.norm += params.fit_func->GetParameter(ParIndex(i_term, FitParams_e::i_rho)) / params.fit_func->GetParameter(ParIndex(i_term, FitParams_e::i_sig));
	}
	params.norm /= sqrt(2 * TMath::Pi());
	// The argument of log() in NLL calculation should be less than 1
	//	We have a probability density, not a probability, so this is not gauranteed
	//	Naively dividing by this factor will ensure the NLLs are all positive
	//		(this is not the exact maximum fit_func can take, but an upper bound for the maximum)
	//	Choice of factor will give a constant offset in weight,
	//	but this will roughly achieve the desired effect

	for(Long64_t n = 0; n < params.fit_tree->GetEntriesFast(); ++n)
	{
		GetAdjustedNll(n);

		avg_NLL += params.hits * params.nll;
		wgt_NLL += params.nll;
	}
	avg_NLL /= wgt_NLL;

	for(Long64_t n = 0; n < params.fit_tree->GetEntriesFast(); ++n)
	{
		GetAdjustedNll(n);

		sig_NLL += (params.hits - avg_NLL) * (params.hits - avg_NLL) * params.nll;
	}
	sig_NLL /= wgt_NLL;
	sig_NLL = sqrt(sig_NLL);

	for(FitParams_e i_par : FitParamsArr)
	{
		params.fit_func->ReleaseParameter(ParIndex(_i_term, i_par));
	}

	params.fit_func->SetParLimits(ParIndex(_i_term, FitParams_e::i_avg), params.min_hits, params.max_hits);
	params.fit_func->SetParLimits(ParIndex(_i_term, FitParams_e::i_sig), 1, params.max_hits - params.min_hits);
	params.fit_func->SetParLimits(ParIndex(_i_term, FitParams_e::i_rho), 0, params.fit_tree->GetEntriesFast() * (params.max_hits + params.min_hits) / num_bins);

	params.fit_func->SetParameter(ParIndex(_i_term, FitParams_e::i_avg), avg_NLL);
	params.fit_func->SetParameter(ParIndex(_i_term, FitParams_e::i_sig), sig_NLL);
	params.fit_func->SetParameter(ParIndex(_i_term, FitParams_e::i_rho), params.fit_tree->GetEntriesFast() * (params.max_hits + params.min_hits) / num_bins / 2.0);

	if(debug.verbose)
	{
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "Iteration: " << (int)_i_term << std::endl;
	}

	if(debug.verbose)
	{
		std::cout << "\t(before fit; initial params)" << std::endl;
		for(uint8_t i_term = 0; i_term <= _i_term; ++i_term)
		{
			PrintParams(i_term);
		}
	}
	if(!debug.pre_fit_plot_name.empty())DrawFit(debug.pre_fit_plot_name, _i_term, false);

	if(!debug.fit_mode.empty())
	{
		params.fit_hist->Fit(params.fit_func, debug.fit_mode.c_str());
	}
	else
	{
		params.fit_hist->Fit(params.fit_func);
	}

	if(debug.verbose)
	{
		std::cout << "\t(after fit; fitted params)" << std::endl;
		for(uint8_t i_term = 0; i_term <= _i_term; ++i_term)
		{
			PrintParams(i_term);
		}
	}
	if(!debug.post_fit_plot_name.empty())DrawFit(debug.post_fit_plot_name, _i_term, true);
}

void
ChannelClassifierHelper::ClassifyEntire()
{
	for(auto itr : params.terms)if(itr.second)delete itr.second;
	params.terms.clear();

	for(uint8_t i_term = 0; i_term < terms_to_accept + terms_to_reject; ++i_term)
	{
		Double_t rho;
		char buff[256];

		rho = params.fit_func->GetParameter(ParIndex(i_term, FitParams_e::i_rho));
		snprintf(buff, sizeof(buff), "term_%02d", (int)i_term);

		params.terms[rho] = new TF1
		(
			buff,
			ChannelClassifierHelper::SumGauss,
			params.min_hits,
			params.max_hits,
			FitParams_e::NUM_PAR + 1
		);

		params.terms[rho]->FixParameter(0, 1);
		for(FitParams_e i_par : FitParamsArr)
		{
			params.terms[rho]->FixParameter(ParIndex(0, i_par), params.fit_func->GetParameter(ParIndex(i_term, i_par)));
		}
	}

	if(debug.verbose)
	{
		uint8_t i_term = 0;
		std::cout << std::endl;
		std::cout << std::endl;

		i_term = 0;
		std::cout << std::endl;
		std::cout << "---------------------------------------------------" << std::endl;
		std::cout << "Parameters of Gaussian terms that will be ACCEPTED:" << std::endl;
		for(auto itr = params.terms.rbegin(); itr != params.terms.rend(); ++itr)
		{
			if(terms_to_accept < ++i_term)break;

			std::cout << "\tavg: " << itr->second->GetParameter(ParIndex(0, FitParams_e::i_avg)) << std::endl;
			std::cout << "\tsig: " << itr->second->GetParameter(ParIndex(0, FitParams_e::i_sig)) << std::endl;
			std::cout << "\trho: " << itr->second->GetParameter(ParIndex(0, FitParams_e::i_rho)) << std::endl;
			std::cout << std::endl;
		}

		i_term = 0;
		std::cout << std::endl;
		std::cout << "---------------------------------------------------" << std::endl;
		std::cout << "Parameters of Gaussian terms that will be REJECTED:" << std::endl;
		for(auto itr = params.terms.begin(); itr != params.terms.end(); ++itr)
		{
			if(terms_to_reject < ++i_term)break;

			std::cout << "\tavg: " << itr->second->GetParameter(ParIndex(0, FitParams_e::i_avg)) << std::endl;
			std::cout << "\tsig: " << itr->second->GetParameter(ParIndex(0, FitParams_e::i_sig)) << std::endl;
			std::cout << "\trho: " << itr->second->GetParameter(ParIndex(0, FitParams_e::i_rho)) << std::endl;
			std::cout << std::endl;
		}
	}

	for(Long64_t n = 0; n < params.fit_tree->GetEntriesFast(); ++n)
	{
		ClassifyHit(n);
	}

	// uint8_t i_accept = 0;
	// uint8_t i_reject = 0;
	// auto itr_accept = params.terms.rbegin();
	// auto itr_reject = params.terms.begin();
	// goto INC;
	// while(true)
	// {
	// 	//...
           
	// 	++itr_reject;
	// 	if(++i_reject < terms_to_reject)goto REJECT_INC;
           
	// 	++itr_accept;
	// 	if(++i_accept < terms_to_accept)goto ACCEPT_INC;
           
	// 	break;
	// 	INC:
           
	// 	i_accept = 0;
	// 	itr_accept = params.rbegin();
	// 	ACCEPT_INC:
           
	// 	i_reject = 0;
	// 	itr_reject = params.begin();
	// 	REJECT_INC:
	// }
}

void
ChannelClassifierHelper::ClassifyHit(Long64_t const& _n)
{
	GetAdjustedHit(_n);

	Double_t i_prob = 0;
	Double_t max_prob = 0;

	uint8_t i_term = 0;
	uint8_t max_term = 0;
	for(auto const& itr : params.terms)
	{
		i_prob = itr.second->EvalPar(&(params.hits));
		if(i_prob > max_prob)
		{
			max_prob = i_prob;
			max_term = i_term;
		}

		++i_term;
	}

	params.status = max_term < terms_to_reject ? 1 : 0;
	params.out_tree->Fill();
}

void
ChannelClassifierHelper::PrintParams(uint8_t const& _i_term)
{
	char buff[256];

	snprintf(buff, sizeof(buff), "%-4s%3d", "term:", (int)_i_term);
	std::cout << "\t\t" << buff << std::endl;

	snprintf(buff, sizeof(buff), "%-8s%+.6e", "avg:", params.fit_func->GetParameter(ParIndex(_i_term, FitParams_e::i_avg)));
	std::cout << "\t\t\t" << buff << std::endl;

	snprintf(buff, sizeof(buff), "%-8s%+.6e", "sig:", params.fit_func->GetParameter(ParIndex(_i_term, FitParams_e::i_sig)));
	std::cout << "\t\t\t" << buff << std::endl;

	snprintf(buff, sizeof(buff), "%-8s%+.6e", "rho:", params.fit_func->GetParameter(ParIndex(_i_term, FitParams_e::i_rho)));
	std::cout << "\t\t\t" << buff << std::endl;
}

void
ChannelClassifierHelper::DrawFit(std::string const& _filename)
{
	TCanvas* cnvs = new TCanvas
	(
		"cnvs",
		"Distribution of Hitrates"
	);
	cnvs->cd();
	params.fit_hist->Draw();

	params.fit_func->SetLineColor(kBlack);
	params.fit_func->Draw("same");

	cnvs->SaveAs(_filename.c_str());
	delete cnvs;
}

void
ChannelClassifierHelper::DrawFit(std::string const& _filename, uint8_t const& _i_term, bool const& _inclusive)
{
	char buff[256];

	snprintf(buff, sizeof(buff), "cnvs_%02d_%0d", (int)_i_term, (int)_inclusive);
	TCanvas* cnvs = new TCanvas
	(
		buff,
		"Distribution of Hitrates"
	);
	params.fit_hist->Draw();

	// Draw just the entire function
	// Either up to or including _i_term
	// depending on the value of _inclusive
	snprintf(buff, sizeof(buff), "sum_hitrate_func_%02d_%0d", (int)_i_term, (int)_inclusive);
	TF1* sum_func = new TF1
	(
		buff,
		ChannelClassifierHelper::SumGauss,
		params.min_hits,
		params.max_hits,
		FitParams_e::NUM_PAR * (_i_term + (int)_inclusive) + 1
	);
	sum_func->SetLineColor(kBlack);
	sum_func->FixParameter(0, _i_term + (int)_inclusive);
	for(uint8_t i_term = 0; i_term < _i_term + (int)_inclusive; ++i_term)
	{
		for(FitParams_e i_par : FitParamsArr)
		{
			sum_func->FixParameter(ParIndex(0, i_par), params.fit_func->GetParameter(ParIndex(i_term, i_par)));
		}
	}
	sum_func->Draw("same");

	// Draw just the last Gaussian term
	snprintf(buff, sizeof(buff), "tmp_hitrate_func_%02d_%0d", (int)_i_term, (int)_inclusive);
	TF1* tmp_func = new TF1
	(
		buff,
		ChannelClassifierHelper::SumGauss,
		params.min_hits,
		params.max_hits,
		FitParams_e::NUM_PAR + 1
	);
	tmp_func->SetLineColor(kRed);
	tmp_func->FixParameter(0, 1);
	for(FitParams_e i_par : FitParamsArr)
	{
		tmp_func->FixParameter(ParIndex(0, i_par), params.fit_func->GetParameter(ParIndex(_i_term, i_par)));
	}
	tmp_func->Draw("same");

	snprintf(buff, sizeof(buff), _filename.c_str(), (int)_i_term);
	cnvs->SaveAs(buff);

	delete sum_func;
	delete tmp_func;
	delete cnvs;
}
