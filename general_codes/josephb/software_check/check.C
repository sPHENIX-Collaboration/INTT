#ifndef CHECK_C
#define CHECK_C

void
check (
) {
	TFile* file = TFile::Open("G4sPHENIX_g4svtx_eval.root");
	if (!file) {
		std::cout << "file" << std::endl;
		return;
	}

	TNtuple* ntp_cluster = dynamic_cast<TNtuple*>(file->Get("ntp_cluster"));
	if (!ntp_cluster) {
		std::cout << "ntuple" << std::endl;
		return;
	}

	std::map<std::string, float> branches {
		{"x", 0},
		{"y", 0},
		{"z", 0},
		{"gx", 0},
		{"gy", 0},
		{"gz", 0},
	};

	for (auto& [name, value] : branches) {
		ntp_cluster->SetBranchAddress(name.c_str(), &value);
	}

	for (Long64_t n = 0, N = ntp_cluster->GetEntriesFast(); n < N; ++n) {
		ntp_cluster->GetEntry(n);

		std::cout << n << std::endl;
		for (auto const& [name, value] : branches) {
			std::cout << "\t" << name << ": " << value << std::endl;
		}
	}
}


#endif//CHECK_C
