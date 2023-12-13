#include "ver_1.h"

void
channel_classifier_ver_1::init_terms(
) {
	m_terms["good_inner_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::good_inner_pdf,
		.init = &channel_classifier_ver_1::init_gauss,
		.min = 0.008,
		.max = 0.016,
		.begin = GOOD_INNER_NORM,
		.end = (char)GOOD_INNER_SIG + 1,
		.state = status::good
	};

	m_terms["half_inner_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::half_inner_pdf,
		.init = &channel_classifier_ver_1::init_gauss,
		.min = 0.004,
		.max = 0.008,
		.begin = HALF_INNER_NORM,
		.end = (char)HALF_INNER_SIG + 1,
		.state = status::half
	};

	m_terms["cold_inner_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::cold_inner_pdf,
		.init = &channel_classifier_ver_1::init_left_exp,
		.min = 0.0,
		.max = 0.004,
		.begin = COLD_INNER_NORM,
		.end = (char)COLD_INNER_WIDTH + 1,
		.state = status::cold
	};

	m_terms["hot_inner_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::hot_inner_pdf,
		.init = &channel_classifier_ver_1::init_right_exp,
		.min = 0.016,
		.max = FLT_MAX,
		.begin = HOT_INNER_NORM,
		.end = (char)HOT_INNER_WIDTH + 1,
		.state = status::hot
	};

	m_terms["good_outer_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::good_outer_pdf,
		.init = &channel_classifier_ver_1::init_gauss,
		.min = 0.005,
		.max = 0.009,
		.begin = GOOD_OUTER_NORM,
		.end = (char)GOOD_OUTER_SIG + 1,
		.state = status::good
	};

	m_terms["half_outer_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::half_outer_pdf,
		.init = &channel_classifier_ver_1::init_gauss,
		.min = 0.003,
		.max = 0.005,
		.begin = HALF_OUTER_NORM,
		.end = (char)HALF_OUTER_SIG + 1,
		.state = status::half
	};

	m_terms["cold_outer_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::cold_outer_pdf,
		.init = &channel_classifier_ver_1::init_left_exp,
		.min = 0.0,
		.max = 0.003,
		.begin = COLD_OUTER_NORM,
		.end = (char)COLD_OUTER_WIDTH + 1,
		.state = status::cold
	};

	m_terms["hot_outer_fit"] = (struct term_s) {
		.tf1 = nullptr,
		.func = &channel_classifier_ver_1::hot_outer_pdf,
		.init = &channel_classifier_ver_1::init_right_exp,
		.min = 0.010,
		.max = FLT_MAX,
		.begin = HOT_OUTER_NORM,
		.end = (char)HOT_OUTER_WIDTH + 1,
		.state = status::hot
	};
}

channel_classifier_ver_1::channel_classifier_ver_1 (
) : m_hist{nullptr}, m_num_sig{5}, m_num_bins{100} {
	init_terms();
}

channel_classifier_ver_1::channel_classifier_ver_1 (
	int const& num_sig,
	int const& num_bins
) : m_hist{nullptr}, m_num_sig{num_sig}, m_num_bins{num_bins} {
	init_terms();
}

channel_classifier_ver_1::~channel_classifier_ver_1() {
	delete m_hist;
	delete m_fit;

	for(auto& pair : m_terms) {
		delete pair.second.tf1;
	}
}

void
channel_classifier_ver_1::adjust_hitrate (
	Double_t& hitrate,
	InttNameSpace::RawData_s const& raw
) const {
	hitrate /= raw.chip % 13 < 5 ? 2.0 : 1.6;
}

int
channel_classifier_ver_1::fill (
	std::string const& filename,
	std::string const& treename
) {
	if(filename.empty()) {
		std::cout << "channel_classifier_ver_1::fill" << std::endl;
		std::cout << "Argument \"filename\" is empty string" << std::endl;
		return 1;
	}
	TFile* file = TFile::Open(filename.c_str(), "READ");
	if(!file) {
		std::cout << "channel_classifier_ver_1::fill" << std::endl;
		std::cout << "Could not get file:" << std::endl;
		std::cout << "\t" << filename << std::endl;
		return 1;
	}

	if(treename.empty()) {
		std::cout << "channel_classifier_ver_1::fill" << std::endl;
		std::cout << "Argument \"treename\" is empty string" << std::endl;
		return 1;
	}
	TTree* tree = (TTree*)file->Get(treename.c_str());
	if(!tree) {
		std::cout << "channel_classifier_ver_1::fill" << std::endl;
		std::cout << "Could not get tree:" << std::endl;
		std::cout << "\t" << treename << std::endl;
		return 1;
	}

	while(true) {
		bool b = true; 
		std::map<std::string, bool> branches = {
			{"hits",	true},
			{"flx_svr",	true},
			{"flx_chn",	true},
			{"chp",		true},
			{"chn",		true},
		};

		for(auto& itr : branches) {
			if(tree->GetBranch(itr.first.c_str())) continue;
			b = false;
			itr.second = false;
		}
		if(b) break;

		std::cout << "channel_classifier_ver_1::fill" << std::endl;
		std::cout << "Could not get branch:" << std::endl;
		for(auto& itr : branches) {
			if(itr.second) continue;
			std::cout << "\t" << itr.first << std::endl;
		}
		return 1;	
	}

	Double_t hitrate;
	InttNameSpace::RawData_s raw;
	tree->SetBranchAddress("hits",		&hitrate);
	tree->SetBranchAddress("flx_svr",	&raw.felix_server);
	tree->SetBranchAddress("flx_chn",	&raw.felix_channel);
	tree->SetBranchAddress("chp",		&raw.chip);
	tree->SetBranchAddress("chn",		&raw.channel);

	Double_t min_hitrate = FLT_MAX;
	Double_t max_hitrate = 0;
	Double_t avg_hitrate = 0;
	Double_t sig_hitrate = 0;

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		adjust_hitrate(hitrate, raw);

		avg_hitrate += hitrate;
		if(hitrate < min_hitrate)min_hitrate = hitrate;
		if(hitrate > max_hitrate)max_hitrate = hitrate;
	}
	avg_hitrate /= tree->GetEntriesFast();

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		adjust_hitrate(hitrate, raw);

		sig_hitrate += (hitrate - avg_hitrate) * (hitrate - avg_hitrate);
	}
	sig_hitrate /= tree->GetEntriesFast();
	sig_hitrate = sqrt(sig_hitrate);

	std::cout << "channel_classifier_ver_1::fill" << std::endl;
	std::cout << "\tadjusted hitrate average: " << avg_hitrate << std::endl;
	std::cout << "\tadjusted hitrate deviation: " << sig_hitrate << std::endl;

	if(min_hitrate < avg_hitrate - m_num_sig * sig_hitrate)min_hitrate = avg_hitrate - m_num_sig * sig_hitrate;
	if(max_hitrate > avg_hitrate + m_num_sig * sig_hitrate)max_hitrate = avg_hitrate + m_num_sig * sig_hitrate;

	std::cout << "\tmin: " << min_hitrate << std::endl;
	std::cout << "\tmax: " << max_hitrate << std::endl;

	delete m_hist;
	m_hist = new TH1D (
		m_hist_name.c_str(),
		m_hist_name.c_str(),
		m_num_bins,
		min_hitrate,
		max_hitrate
	);
	m_hist->SetDirectory(nullptr);
	m_hist->SetLineColor(kBlack);

	delete m_inner_hist;
	m_inner_hist = new TH1D (
		(m_hist_name + "inner").c_str(),
		(m_hist_name + "inner").c_str(),
		m_num_bins,
		min_hitrate,
		max_hitrate
	);
	m_inner_hist->SetDirectory(nullptr);
	m_inner_hist->SetLineColor(kBlue);

	delete m_outer_hist;
	m_outer_hist = new TH1D (
		(m_hist_name + "outer").c_str(),
		(m_hist_name + "outer").c_str(),
		m_num_bins,
		min_hitrate,
		max_hitrate
	);
	m_outer_hist->SetDirectory(nullptr);
	m_outer_hist->SetLineColor(kRed);

	InttNameSpace::Online_s onl;
	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		adjust_hitrate(hitrate, raw);

		if(hitrate < min_hitrate) hitrate = min_hitrate + 0.5 * (max_hitrate - min_hitrate) / m_num_bins;
		if(hitrate > max_hitrate) hitrate = max_hitrate - 0.5 * (max_hitrate - min_hitrate) / m_num_bins;

		m_hist->Fill(hitrate);
		onl = InttNameSpace::ToOnline(raw);
		if(onl.lyr < 2) {
			m_inner_hist->Fill(hitrate);
		} else {
			m_outer_hist->Fill(hitrate);
		}
	}

	file->Close();

	return 0;
}

int
channel_classifier_ver_1::fit (
) {
	if(!m_hist || !m_inner_hist || !m_outer_hist) {
		std::cout << "channel_classifier_ver_1::fit" << std::endl;
		std::cout << "Member histogram is null at call" << std::endl;
		return 1;
	}

	for(auto& pair : m_terms) {
		if(pair.second.min < m_hist->GetXaxis()->GetXmin())pair.second.min = m_hist->GetXaxis()->GetXmin();
		if(pair.second.max > m_hist->GetXaxis()->GetXmax())pair.second.max = m_hist->GetXaxis()->GetXmax();
		delete pair.second.tf1;
		pair.second.tf1 = new TF1 (
			pair.first.c_str(),
			pair.second.func,
			pair.second.min,
			pair.second.max,
			NUM_PAR
		);
		for(int i = 0; i < NUM_PAR; ++i) {
			pair.second.tf1->FixParameter(i, 0);
		}

		(this->*(pair.second.init))(pair.second.tf1, pair.second.begin);

		if(pair.first.find("inner") != std::string::npos) {
			for(int i = 0; i < 10; ++i) {
				m_inner_hist->Fit(pair.second.tf1, "RLQ");
			}
			m_inner_hist->Fit(pair.second.tf1, "RL");
		} else {
			for(int i = 0; i < 10; ++i) {
				m_outer_hist->Fit(pair.second.tf1, "RLQ");
			}
			m_outer_hist->Fit(pair.second.tf1, "RL");
		}
	}

	return 0;
}

int
channel_classifier_ver_1::classify (
	std::string const& i_filename,
	std::string const& o_filename,
	std::string const& i_treename,
	std::string const& o_treename
) {
	for(auto& pair : m_terms) {
		if(pair.second.tf1)continue;
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Member fit functions unititalized" << std::endl;
		std::cout << "A call to classify must be preceeded by (successful) calls to fill() and fit()" << std::endl;
		return 1;	
	}

	if(i_filename.empty()) {
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Argument \"i_filename\" is empty string" << std::endl;
		return 1;
	}
	TFile* i_file = TFile::Open(i_filename.c_str(), "READ");
	if(!i_file) {
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Could not get file:" << std::endl;
		std::cout << "\t" << i_filename << std::endl;
		return 1;
	}

	if(i_treename.empty()) {
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Argument \"i_treename\" is empty string" << std::endl;
		return 1;
	}
	TTree* i_tree = (TTree*)i_file->Get(i_treename.c_str());
	if(!i_tree) {
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Could not get tree:" << std::endl;
		std::cout << "\t" << i_treename << std::endl;
		return 1;
	}

	while(true) {
		bool b = true; 
		std::map<std::string, bool> branches = {
			{"hits",	true},
			{"flx_svr",	true},
			{"flx_chn",	true},
			{"chp",		true},
			{"chn",		true},
		};

		for(auto& itr : branches) {
			if(i_tree->GetBranch(itr.first.c_str())) continue;
			b = false;
			itr.second = false;
		}
		if(b) break;

		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Could not get branch:" << std::endl;
		for(auto& itr : branches) {
			if(itr.second) continue;
			std::cout << "\t" << itr.first << std::endl;
		}
		return 1;	
	}

	if(o_filename.empty()) {
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Argument \"o_filename\" is empty string" << std::endl;
		return 1;
	}
	TFile* o_file = TFile::Open(o_filename.c_str(), "RECREATE");
	if(!o_file) {
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Could not (re)create file:" << std::endl;
		std::cout << "\t" << o_filename << std::endl;
		return 1;
	}

	if(o_treename.empty()) {
		std::cout << "channel_classifier_ver_1::classify" << std::endl;
		std::cout << "Argument \"o_treename\" is empty string" << std::endl;
		return 1;
	}

	TTree* o_tree = new TTree(o_treename.c_str(), o_treename.c_str());
	o_tree->SetDirectory(o_file);

	Double_t hitrate;
	InttNameSpace::RawData_s raw;
	i_tree->SetBranchAddress("hits",	&hitrate);
	i_tree->SetBranchAddress("flx_svr",	&raw.felix_server);
	i_tree->SetBranchAddress("flx_chn",	&raw.felix_channel);
	i_tree->SetBranchAddress("chp",		&raw.chip);
	i_tree->SetBranchAddress("chn",		&raw.channel);

	char state;
	o_tree->Branch("hits",		&hitrate);
	o_tree->Branch("flx_svr",	&raw.felix_server);
	o_tree->Branch("flx_chn",	&raw.felix_channel);
	o_tree->Branch("chp",		&raw.chip);
	o_tree->Branch("chn",		&raw.channel);
	o_tree->Branch("state",		&state);

	Double_t max = 0.0;
	Double_t tmp = 0.0;
	std::string barrel = "";
	InttNameSpace::Online_s onl;
	for(Long64_t n = 0; n < i_tree->GetEntriesFast(); ++n) {
		i_tree->GetEntry(n);
		adjust_hitrate(hitrate, raw);

		if(hitrate < m_hist->GetXaxis()->GetXmin())hitrate = m_hist->GetXaxis()->GetXmin();
		if(hitrate > m_hist->GetXaxis()->GetXmax())hitrate = m_hist->GetXaxis()->GetXmax();

		if(hitrate == 0.0) {
			state = (char)status::dead;
			o_tree->Fill();
			continue;
		}

		max = 0.0;
		barrel = "";
		onl = InttNameSpace::ToOnline(raw);
		if(onl.lyr < 2) {
			barrel = "inner";
		} else {
			barrel = "outer";
		}

		for(auto const& pair : m_terms) {
			tmp = pair.second.tf1->Eval(hitrate);
			if(tmp < max)continue;

			max = tmp;
			state = (char)pair.second.state;
		}

		o_tree->Fill();
	}

	i_file->Close();

	o_tree->Write();
	o_file->Write();
	o_file->Close();

	return 0;
}
int
channel_classifier_ver_1::write_hist (
	std::string const& filename
) {
	if(!m_hist) {
		std::cout << "channel_classifier_ver_1::write_hist" << std::endl;
		std::cout << "\tMember \"m_hist\" is null at call" << std::endl;
		return 1;
	}

	if(filename.empty()) {
		std::cout << "channel_classifier_ver_1::write_hist" << std::endl;
		std::cout << "\tArgument \"filename\" is empty string" << std::endl;
		return 1;
	}

	TFile* file = TFile::Open(filename.c_str(), "RECREATE");
	if(!file) {
		std::cout << "channel_classifier_ver_1::write_hist" << std::endl;
		std::cout << "\tCould not recreate file" << std::endl;
		std::cout << "\t" << filename << std::endl;
		return 1;
	}

	file->cd();

	m_hist->Write();
	m_inner_hist->Write();
	m_outer_hist->Write();

	file->Write();
	file->Close();

	return 0;
}

int
channel_classifier_ver_1::draw (
	std::string const& filename
) {
	if(!m_hist) {
		std::cout << "channel_classifier_ver_1::write_hist" << std::endl;
		std::cout << "\tMember \"m_hist\" is null at call" << std::endl;
		return 1;
	}

	if(filename.empty()) {
		std::cout << "channel_classifier_ver_1::write_hist" << std::endl;
		std::cout << "\tArgument \"filename\" is empty string" << std::endl;
		return 1;
	}

	TCanvas* cnvs = new TCanvas (
		"cnvs",
		"cnvs"
	);
	cnvs->cd();
	cnvs->SetLogy();
	m_hist->Draw();
	m_inner_hist->Draw("same");
	m_outer_hist->Draw("same");
	if(m_fit)m_fit->Draw("same");
	for(auto& pair : m_terms) {
		if(!pair.second.tf1)continue;
		pair.second.tf1->Draw("same");
	}
	cnvs->SaveAs(filename.c_str());
	delete cnvs;

	cnvs = new TCanvas (
		"cnvs",
		"cnvs"
	);
	cnvs->cd();
	cnvs->SetLogy();
	m_inner_hist->Draw();
	for(auto& pair : m_terms) {
		if(pair.first.find("inner") == std::string::npos)continue;
		if(!pair.second.tf1)continue;
		pair.second.tf1->Draw("same");
	}
	std::string inner_name = filename.substr(0, filename.find("."));
	inner_name += "_inner.png";
	cnvs->SaveAs(inner_name.c_str());
	delete cnvs;

	cnvs = new TCanvas (
		"cnvs",
		"cnvs"
	);
	cnvs->cd();
	cnvs->SetLogy();
	m_outer_hist->Draw();
	for(auto& pair : m_terms) {
		if(pair.first.find("outer") == std::string::npos)continue;
		if(!pair.second.tf1)continue;
		pair.second.tf1->Draw("same");
	}
	std::string outer_name = filename.substr(0, filename.find("."));
	outer_name += "_outer.png";
	cnvs->SaveAs(outer_name.c_str());
	delete cnvs;

	return 0;
}

void
channel_classifier_ver_1::init_gauss (
	TF1* tf1,
	char begin
) {
	Double_t tmp_min;
	Double_t tmp_max;
	tf1->GetRange(tmp_min, tmp_max);
	// Double_t min_hitrate = m_hist->GetXaxis()->GetXmin();
	// Double_t max_hitrate = m_hist->GetXaxis()->GetXmax();
	Double_t max_entry = m_hist->GetBinContent(m_hist->GetMaximumBin());
	Double_t bin_width = m_hist->GetXaxis()->GetBinWidth(0);

	tf1->ReleaseParameter(begin + 0);
	tf1->ReleaseParameter(begin + 1);
	tf1->ReleaseParameter(begin + 2);

	tf1->SetParLimits(begin + 0, 0.0, max_entry);
	tf1->SetParameter(begin + 0, 0.0);
	tf1->SetParLimits(begin + 1, tmp_min, tmp_max);
	tf1->SetParameter(begin + 1, (tmp_max + tmp_min) / 2.0);
	tf1->SetParLimits(begin + 2, 0.0, tmp_max - tmp_min);
	tf1->SetParameter(begin + 2, (tmp_max - tmp_min) / 2.0);
}

void
channel_classifier_ver_1::init_left_exp (
	TF1* tf1,
	char begin
) {
	// Double_t tmp_min;
	// Double_t tmp_max;
	// tf1->GetRange(tmp_min, tmp_max);
	Double_t min_hitrate = m_hist->GetXaxis()->GetXmin();
	// Double_t max_hitrate = m_hist->GetXaxis()->GetXmax();
	Double_t max_entry = m_hist->GetBinContent(m_hist->GetMaximumBin());
	Double_t bin_width = m_hist->GetXaxis()->GetBinWidth(0);

	tf1->ReleaseParameter(begin + 0);
	tf1->ReleaseParameter(begin + 1);
	tf1->ReleaseParameter(begin + 2);

	tf1->SetParLimits(begin + 0, 0.0, max_entry);
	tf1->SetParameter(begin + 0, 0.0);
	// tf1->SetParLimits(begin + 1, min_hitrate, min_hitrate + bin_width);
	// tf1->SetParameter(begin + 1, min_hitrate + bin_width * 0.5);
	tf1->FixParameter(begin + 1, min_hitrate);
	tf1->SetParLimits(begin + 2, 0.0, m_num_sig * bin_width);
	tf1->SetParameter(begin + 2, bin_width);
}

void
channel_classifier_ver_1::init_right_exp (
	TF1* tf1,
	char begin
) {
	// Double_t tmp_min;
	// Double_t tmp_max;
	// tf1->GetRange(tmp_min, tmp_max);
	// Double_t min_hitrate = m_hist->GetXaxis()->GetXmin();
	Double_t max_hitrate = m_hist->GetXaxis()->GetXmax();
	Double_t max_entry = m_hist->GetBinContent(m_hist->GetMaximumBin());
	Double_t bin_width = m_hist->GetXaxis()->GetBinWidth(0);

	tf1->ReleaseParameter(begin + 0);
	tf1->ReleaseParameter(begin + 1);
	tf1->ReleaseParameter(begin + 2);

	tf1->SetParLimits(begin + 0, 0.0, max_entry);
	tf1->SetParameter(begin + 0, 0.0);
	// tf1->SetParLimits(begin + 1, max_hitrate - bin_width, max_hitrate);
	// tf1->SetParameter(begin + 1, max_hitrate - bin_width * 0.5);
	tf1->FixParameter(begin + 1, max_hitrate);
	tf1->SetParLimits(begin + 2, 0.0, m_num_sig * bin_width);
	tf1->SetParameter(begin + 2, bin_width);
}

Double_t
channel_classifier_ver_1::good_inner_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[GOOD_INNER_NORM] * TMath::Gaus(x[0], p[GOOD_INNER_AVG], p[GOOD_INNER_SIG]);
}


Double_t
channel_classifier_ver_1::half_inner_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[HALF_INNER_NORM] * TMath::Gaus(x[0], p[HALF_INNER_AVG], p[HALF_INNER_SIG]);
}

Double_t
channel_classifier_ver_1::cold_inner_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[COLD_INNER_NORM] * TMath::Exp((p[COLD_INNER_SHIFT] - x[0]) / p[COLD_INNER_WIDTH]);
}

Double_t
channel_classifier_ver_1::hot_inner_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[HOT_INNER_NORM] * TMath::Exp((x[0] - p[HOT_INNER_SHIFT]) / p[HOT_INNER_WIDTH]);
}


Double_t
channel_classifier_ver_1::good_outer_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[GOOD_OUTER_NORM] * TMath::Gaus(x[0], p[GOOD_OUTER_AVG], p[GOOD_OUTER_SIG]);
}

Double_t
channel_classifier_ver_1::half_outer_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[HALF_OUTER_NORM] * TMath::Gaus(x[0], p[HALF_OUTER_AVG], p[HALF_OUTER_SIG]);
}

Double_t
channel_classifier_ver_1::cold_outer_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[COLD_OUTER_NORM] * TMath::Exp((p[COLD_OUTER_SHIFT] - x[0]) / p[COLD_OUTER_WIDTH]);
}

Double_t
channel_classifier_ver_1::hot_outer_pdf(
	Double_t* x,
	Double_t* p
) {
	return p[HOT_OUTER_NORM] * TMath::Exp((x[0] - p[HOT_OUTER_SHIFT]) / p[HOT_OUTER_WIDTH]);
}
