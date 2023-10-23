#include "ChannelClassifier.h"

ChannelClassifier::ChannelClassifier()
{
	hits = new Long64_t[NUM_CHANNELS];
	Reset();
}

ChannelClassifier::~ChannelClassifier()
{
	delete[] hits;
}

void ChannelClassifier::Reset()
{
	for(int i = 0; i < NUM_CHANNELS; ++i)
	{
		hits[i] = 0;
	}

	mode_map.clear();
	mode_map["cold"] = (struct ModeMap_s)
	{
		.s = 0,
		.avg_func = &ChannelClassifier::ColdAvg,
	};
	mode_map["half"] = (struct ModeMap_s)
	{
		.s = 1,
		.avg_func = &ChannelClassifier::HalfAvg,
	};
	mode_map["good"] = (struct ModeMap_s)
	{
		.s = 2,
		.avg_func = &ChannelClassifier::GoodAvg,
	};
	mode_map["hott"] = (struct ModeMap_s)
	{
		.s = 3,
		.avg_func = &ChannelClassifier::HottAvg,
	};
}

Intt::Online_s ChannelClassifier::struct_from_hash(Int_t h)
{
	Intt::Online_s s = (Intt::Online_s){.lyr = 0, .ldr = 0, .arm = 0, .chp = 0, .chn = 0};

	s.chn = h % 128;
	h /= 128;

	s.chp = h % 26;
	h /= 26;

	s.arm = h % 2;
	h /= 2;

	s.ldr = h;
	while(true)
	{
		if(s.ldr < 12)break;
		s.ldr -= 12;
		++s.lyr;

		if(s.ldr < 12)break;
		s.ldr -= 12;
		++s.lyr;

		if(s.ldr < 16)break;
		s.ldr -= 16;
		++s.lyr;

		break;
	}

	return s;
}

Int_t ChannelClassifier::hash_from_struct(Intt::Online_s const& s)
{
	Int_t h = 0;
	int b = 1;

	h += s.chn * b;
	b *= 128;

	h += s.chp * b;
	b *= 26;

	h += s.arm * b;
	b *= 2;

	h += s.ldr * b;
	while(true)
	{
		if(s.lyr < 1)break;
		h += 12 * b;

		if(s.lyr < 2)break;
		h += 12 * b;

		if(s.lyr < 3)break;
		h += 16 * b;

		break;
	}

	return h;
}

Int_t ChannelClassifier::hash_from_struct(Intt::Offline_s const& s)
{
	return hash_from_struct(Intt::ToOnline(s));
}

Int_t ChannelClassifier::hash_from_struct(Intt::RawData_s const& s)
{
	return hash_from_struct(Intt::ToOnline(s));
}

void ChannelClassifier::print(Intt::Online_s const& s)
{
	printf("lyr: %2d ldr: %2d arm: %1d chp: %2d chn: %3d\n", s.lyr, s.ldr, s.arm, s.chp, s.chn);
}

int ChannelClassifier::AppendFile(std::string const& i_filename)
{
	int return_val = 0;
	std::stringstream output;

	output << "int ChannelClassifier::AppendFile(std::string const& i_filename)" << std::endl;
	output << "\twith 'i_filename' = '" << i_filename << "'" << std::endl;

	TFile* file = nullptr;
	TTree* tree = nullptr;
	Intt::Online_s onl;

	std::map<std::string, std::vector<Int_t>*> branches =
	{
		{"lyr",	new std::vector<Int_t>()},
		{"ldr",	new std::vector<Int_t>()},
		{"arm",	new std::vector<Int_t>()},
		{"chp",	new std::vector<Int_t>()},
		{"chn",	new std::vector<Int_t>()},
	};

	if(!std::filesystem::exists(i_filename))
	{
		output << "\tpath 'i_filename' does not exist" << std::endl;
		output << "file not appended" << std::endl;
		output << std::endl;

		return_val = 1;
		goto LABEL;
	}

	file = TFile::Open(i_filename.c_str(), "READ");
	if(!file)
	{
		output << "\tfailed to open file 'i_filename' as ROOT file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In file:" << std::endl;
	output << "\t" << i_filename << std::endl;

	tree = (TTree*)file->Get(i_treename.c_str());
	if(!tree)
	{
		output << "\tfailed to get TTree '" << i_treename << "' in file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In tree:" << std::endl;
	output << "\t" << i_treename << std::endl;

	for(auto& itr: branches)
	{
		if(!tree->GetBranch(itr.first.c_str()))
		{
			output << "\tfailed to get branch '" << itr.first << "'" << std::endl;
			return_val = 1;
		}
	}
	if(return_val)goto LABEL;

	for(auto& itr: branches)
	{
		tree->SetBranchAddress(itr.first.c_str(), &(itr.second));
	}

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
	{
		tree->GetEntry(n);
		for(std::size_t s = 0; s < branches.begin()->second->size(); ++s)
		{
			onl.lyr = (*branches["lyr"])[s];
			onl.ldr = (*branches["ldr"])[s];
			onl.arm = (*branches["arm"])[s];
			onl.chp = (*branches["chp"])[s];
			onl.chn = (*branches["chn"])[s];

			//print(onl);
			//std::cout << hash_from_struct(onl) << std::endl;
			//std::cout << std::endl;

			++(hits[hash_from_struct(onl)]);
		}
	}

	LABEL:
	output << std::ends;
	if(file)file->Close();
	for(auto& itr: branches)
	{
		if(itr.second)delete itr.second;
	}
	if(return_val)
	{
		std::cout << output.str();
	}

	return return_val;
}

int ChannelClassifier::OutputHits(std::string const& o_filename)
{
	Double_t _hits;
	struct Intt::Online_s onl;
	struct Intt::Offline_s ofl;

	TFile* file = new TFile(o_filename.c_str(), "RECREATE");
	TTree* tree = new TTree(o_treename.c_str(), o_treename.c_str());

	tree->Branch("lyr", &(onl.lyr));
	tree->Branch("ldr", &(onl.ldr));
	tree->Branch("arm", &(onl.arm));
	tree->Branch("chp", &(onl.chp));
	tree->Branch("chn", &(onl.chn));

	tree->Branch("hits", &_hits);

	for(int i = 0; i < NUM_CHANNELS; ++i)
	{
		onl = struct_from_hash(i);
		ofl = Intt::ToOffline(onl);

		_hits = hits[i] / (ofl.ladder_z % 2 ? 1.6 : 2.0);

		tree->Fill();
	}

	tree->Print();
	tree->Write();
	file->Write();
	file->Close();

	return 0;
}

int ChannelClassifier::PoissonFit(std::string const& i_filename)
{
	int return_val = 0;
	std::stringstream output;

	output << "int ChannelClassifier::PoissonFit(std::string const& i_filename)" << std::endl;
	output << "\twith 'i_filename' = '" << i_filename << "'" << std::endl;

	TFile* file = nullptr;
	TTree* tree = nullptr;
	TF1* model = nullptr;

	Double_t _hits;
	Intt::Online_s onl;
	std::vector<std::string> branches =
	{
		"lyr",
		"ldr",
		"arm",
		"chp",
		"chn",

		"hits",
	};

	if(!std::filesystem::exists(i_filename))
	{
		output << "\tpath 'i_filename' does not exist" << std::endl;
		output << "file not appended" << std::endl;
		output << std::endl;

		return_val = 1;
		goto LABEL;
	}

	file = TFile::Open(i_filename.c_str(), "READ");
	if(!file)
	{
		output << "\tfailed to open file 'i_filename' as ROOT file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In file:" << std::endl;
	output << "\t" << i_filename << std::endl;

	tree = (TTree*)file->Get(o_treename.c_str());
	if(!tree)
	{
		output << "\tfailed to get TTree '" << o_treename << "' in file" << std::endl;
		output << "file not appended" << std::endl;

		return_val = 1;
		goto LABEL;
	}
	output << "In tree:" << std::endl;
	output << "\t" << o_treename << std::endl;

	for(auto& itr: branches)
	{
		if(!tree->GetBranch(itr.c_str()))
		{
			output << "\tfailed to get branch '" << itr << "'" << std::endl;
			return_val = 1;
		}
	}
	if(return_val)goto LABEL;

	tree->SetBranchAddress("lyr", &(onl.lyr));
	tree->SetBranchAddress("ldr", &(onl.ldr));
	tree->SetBranchAddress("arm", &(onl.arm));
	tree->SetBranchAddress("chp", &(onl.chp));
	tree->SetBranchAddress("chn", &(onl.chn));

	tree->SetBranchAddress("hits", &_hits);

	{
		Long64_t avg_hits = 0;
		Double_t sig_hits = 0;

		Long64_t min_hits = INT_MAX;
		Long64_t max_hits = 0;

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			avg_hits += _hits;
			if(_hits < min_hits)min_hits = _hits;
			if(_hits > max_hits)max_hits = _hits;
		}
		avg_hits /= tree->GetEntriesFast();

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			sig_hits += (_hits - avg_hits) * (_hits - avg_hits);
		}
		sig_hits /= tree->GetEntriesFast();
		sig_hits = sqrt(sig_hits);

		if(max_hits > avg_hits + NUM_SIG * sig_hits)max_hits = avg_hits + NUM_SIG * sig_hits;

		TH1* hist = new TH1D("hist", "hist", NUM_BINS,  min_hits, max_hits);
		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			hist->Fill(_hits);
		}


		model = new TF1(Form("model"), ChannelClassifier::SumGauss, min_hits, max_hits,  3 * mode_map.size() + 1);

		model->FixParameter(0, mode_map.size());

		struct Params_s params = (struct Params_s)
		{
			.avg = avg_hits,
			.min = min_hits,
			.max = max_hits,
		};
		for(ModeMap_t::const_iterator itr = mode_map.begin(); itr != mode_map.end(); ++itr)
		{
			model->SetParameter(3 * itr->second.s + 1, itr->second.avg_func(params));
			model->SetParLimits(3 * itr->second.s + 1, min_hits, max_hits);

			model->SetParameter(3 * itr->second.s + 2, (Double_t)sig_hits / mode_map.size());
			model->SetParLimits(3 * itr->second.s + 2, 1, (Double_t)NUM_SIG * sig_hits);

			model->SetParameter(3 * itr->second.s + 3, (Double_t)tree->GetEntriesFast() * (max_hits - min_hits) / NUM_BINS / mode_map.size());
			model->SetParLimits(3 * itr->second.s + 3, 0, (Double_t)max_hits * (max_hits - min_hits) / NUM_BINS);
		}

		hist->Fit(model, "Q");

		TF1* tmp_gauss = new TF1(Form("tmp_gauss"), ChannelClassifier::SumGauss, min_hits, max_hits, 4);
		tmp_gauss->FixParameter(0, 1);

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			Double_t prob = 0;
			ModeMap_t::const_iterator mode;

			for(ModeMap_t::const_iterator itr = mode_map.begin(); itr != mode_map.end(); ++itr)
			{
				for(int i = 1; i < 4; ++i)tmp_gauss->SetParameter(i, model->GetParameter(3 * itr->second.s + i));
				Double_t tmp_prob = tmp_gauss->EvalPar(&_hits);

				if(tmp_prob < prob)continue;

				prob = tmp_prob;
				mode = itr;
			}

			//here
		}

		delete model;
	}

	LABEL:
	output << std::ends;
	if(file)file->Close();
	if(return_val)
	{
		std::cout << output.str();
	}

	return return_val;
}

//RooFit
//{
//		Long64_t avg_hits = 0;
//		Double_t sig_hits = 0;
//
//		Long64_t min_hits = INT_MAX;
//		Long64_t max_hits = 0;
//
//		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
//		{
//			tree->GetEntry(n);
//
//			avg_hits += _hits;
//			if(_hits < min_hits)min_hits = _hits;
//			if(_hits > max_hits)max_hits = _hits;
//		}
//		avg_hits /= tree->GetEntriesFast();
//
//		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
//		{
//			tree->GetEntry(n);
//
//			sig_hits += (_hits - avg_hits) * (_hits - avg_hits);
//		}
//		sig_hits /= tree->GetEntriesFast();
//		sig_hits = sqrt(sig_hits);
//
//		if(max_hits > avg_hits + NUM_SIG * sig_hits)max_hits = avg_hits + NUM_SIG * sig_hits;
//		RooRealVar x("hits", "hits", min_hits, max_hits);
//		RooArgList m;
//		RooArgList s;
//		RooArgList g;
//		RooArgList c;
//
//		for(int i = 0; i < NUM_FUNC; ++i)
//		{
//			m.addOwned(*(new RooRealVar(Form("m_%d", i), Form("m_%d", i), 0, (Double_t)max_hits)));
//			s.addOwned(*(new RooRealVar(Form("s_%d", i), Form("s_%d", i), 0, (Double_t)max_hits)));
//			g.addOwned(*(new RooGaussian(Form("g_%d", i), Form("g_%d", i), x, (RooRealVar&)m[i], (RooRealVar&)s[i])));
//			c.addOwned(*(new RooRealVar(Form("c_%d", i), Form("c_%d", i), 0, (Double_t)NUM_CHANNELS)));
//		}
//
//		((RooRealVar&)m[0]).setVal(avg_hits + 23);
//		((RooRealVar&)s[0]).setVal(sig_hits + 45);
//		((RooRealVar&)c[0]).setVal(tree->GetEntriesFast());
//
//		((RooRealVar&)m[1]).setVal(avg_hits - 23);
//		((RooRealVar&)s[1]).setVal(sig_hits - 45);
//		((RooRealVar&)c[1]).setVal(tree->GetEntriesFast());
//
//		//((RooRealVar&)lmbd[1]).setVal(1.0);
//		//((RooRealVar&)cfnt[1]).setVal(0.0);
//
//		//((RooRealVar&)lmbd[2]).setVal(max_hits);
//		//((RooRealVar&)cfnt[2]).setVal(0.0);
//
//		RooAddPdf* model = new RooAddPdf("model", "model", g, c);
//		RooDataSet* data_set = new RooDataSet("data_set", "data_set", RooArgSet(x), RooFit::Import(*tree));
//
//		//break;
//
//		RooFitResult* result = model->fitTo(*data_set);
//
//		TCanvas* cnvs = new TCanvas("cnvs", "Distribution of Hitrates");
//		cnvs->cd();
//		RooPlot* plot = x.frame(RooFit::Title("Distribution of Hitrates"));
//
//		data_set->plotOn
//		(
//			plot,
//			RooFit::MarkerColor(kBlack),
//			RooFit::MarkerStyle(8),
//			RooFit::Binning(NUM_BINS, min_hits, max_hits)//,
//			//RooFit::RefreshNorm(),
//		);
//		model->plotOn
//		(
//			plot,
//			RooFit::LineColor(kRed),
//			RooFit::LineStyle(1),
//			RooFit::LineWidth(3)//,
//			//RooFit::Normalization(data_set->sumEntries())
//		);
//		plot->Draw();
//
//		std::string o_filename = i_filename;
//		o_filename = o_filename.substr(0, o_filename.find("."));
//		o_filename += ".png";
//		cnvs->SaveAs(o_filename.c_str());
//		delete cnvs;
//
//
//		std::cout << "avg_hits:\t" << avg_hits << "\tfitted:\t" << ((RooRealVar&)m[0]).getValV() << std::endl;
//		std::cout << "sig_hits:\t" << sig_hits << "\tfitted:\t" << ((RooRealVar&)s[0]).getValV() << std::endl;
//		//std::cout << "measured:\t" << model->GetParameter(3) << std::endl;
//		//std::cout << "computed:\t" << tree->GetEntriesFast() * (max_hits - min_hits) / NUM_BINS << std::endl;
//}




//	//std::string o_filename = i_filename;
//	//o_filename = o_filename.substr(0, o_filename.find("."));
//	//o_filename += ".png";
//
//	TCanvas* cnvs = new TCanvas("cnvs", "Distribution of Hitrates");
//	cnvs->cd();
//	plot->Draw();
//	//cnvs->SaveAs(o_filename.c_str());
//}


//		if(max_hits > avg_hits + NUM_SIG * sig_hits)max_hits = avg_hits + NUM_SIG * sig_hits;
//
//		TH1* hist = new TH1D("hist", "hist", NUM_BINS,  min_hits, max_hits);
//		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
//		{
//			tree->GetEntry(n);
//
//			hist->Fill(_hits);
//		}
//
//		std::size_t s = 0;
//		while(true)
//		{
//			if(model)delete model;
//
//			model = new TF1(Form("model_%d", s), ChannelClassifier::SumGauss, min_hits, max_hits,  3 * s + 4);
//			model->FixParameter(0, s + 1);
//
//			model->SetParameter(3 * s + 1, avg_hits);
//			model->SetParLimits(3 * s + 1, min_hits, max_hits);
//
//			model->SetParameter(3 * s + 2, sig_hits);
//			model->SetParLimits(3 * s + 2, 1, NUM_SIG * sig_hits);
//
//			model->SetParameter(3 * s + 3, tree->GetEntriesFast() * (max_hits - min_hits) / NUM_BINS);
//
//			hist->Fit(model, "Q");
//
//			break;
//		}
