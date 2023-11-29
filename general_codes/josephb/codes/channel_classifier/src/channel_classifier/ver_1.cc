#include "ver_1.h"

channel_classifier_ver_1::channel_classifier_ver_1 (
) : m_hist{nullptr}, m_num_sig{5}, m_num_bins{100} {
	// do nothing
}

channel_classifier_ver_1::channel_classifier_ver_1 (
	int const& num_sig,
	int const& num_bins
) : m_hist{nullptr}, m_num_sig{num_sig}, m_num_bins{num_bins} {
	// do nothing
}

channel_classifier_ver_1::~channel_classifier_ver_1() {
	if(m_hist)delete m_hist;
	//   I don't understand why my program segfaults when the destructors tries to call this
	//   I wish I knew how to check that ROOT deletes it or something
	//   But there will be only one instance of this class in the main program, so...
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

	std::cout << "adjusted hirate average: " << avg_hitrate << std::endl;
	std::cout << "adjusted hirate deviation: " << sig_hitrate << std::endl;

	if(min_hitrate < avg_hitrate - m_num_sig * sig_hitrate)min_hitrate = avg_hitrate - m_num_sig * sig_hitrate;
	if(max_hitrate > avg_hitrate + m_num_sig * sig_hitrate)max_hitrate = avg_hitrate + m_num_sig * sig_hitrate;

	//m_hist = std::make_unique<TH1D> (
	if(m_hist)delete m_hist;
	m_hist = new TH1D (
		"hitrate_distribution",
		"hitrate_distribution",
		m_num_bins,
		min_hitrate,
		max_hitrate
	);
	m_hist->SetDirectory(nullptr);

	std::cout << "channel_classifier_ver_1::fill" << std::endl;
	std::cout << "\t" << std::hex << m_hist << std::endl;
	std::cout << "\t" << m_hist->GetName() << std::endl;
	std::cout << "\t" << m_hist->GetTitle() << std::endl;

	for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n) {
		tree->GetEntry(n);
		adjust_hitrate(hitrate, raw);

		if(hitrate < min_hitrate) hitrate = min_hitrate + 0.5 * (max_hitrate - min_hitrate) / m_num_bins;
		if(hitrate > max_hitrate) hitrate = max_hitrate - 0.5 * (max_hitrate - min_hitrate) / m_num_bins;
		m_hist->Fill(hitrate);
	}

	file->Close();

	return 0;
}

int
channel_classifier_ver_1::fit (
) {
	// do nothing

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

	std::cout << "channel_classifier_ver_1::write_hist" << std::endl;
	std::cout << "\t" << std::hex << m_hist << std::endl;
	std::cout << "\t" << m_hist->GetName() << std::endl;
	std::cout << "\t" << m_hist->GetTitle() << std::endl;

	file->cd();
	m_hist->Write();
	file->Write();
	file->Close();

	return 0;
}
