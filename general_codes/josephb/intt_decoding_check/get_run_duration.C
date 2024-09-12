#ifndef GET_RUN_DURATION_C
#define GET_RUN_DURATION_C

struct bco_comparator_s {
	static Long64_t const MAX = Long64_t{1}<<40;
	bool operator()(Long64_t const&, Long64_t const&) const;
};

Color_t get_fee_color(int const&);

void
get_run_duration (
	int run_num
) {
	std::string format = "run_%08d_intt%01d.root";
	char buff[256];

	std::set<Long64_t, bco_comparator_s> nonsense_set;
	std::map<Long64_t, Long64_t> nonsense_map;
	for(int i = 0; i < 8; ++i) {
		snprintf(buff, sizeof(buff), format.c_str(), run_num, i);
		std::cout << buff << std::endl;

		TFile* file = TFile::Open(buff, "READ");
		if(!file) {
			std::cerr << "file " << buff << std::endl;
			continue;
		}
		TTree* tree = (TTree*)file->Get("tree");
		if(!tree) {
			std::cerr << "tree " << buff << std::endl;
			continue;
		}

		int fee = 0, max_bcos = 0;
		tree->SetBranchAddress("fee",  &fee);
		int min_evt, max_evt;
		Long64_t head, min, max, num;
		tree->SetBranchAddress("head", &head);
		tree->SetBranchAddress("min",  &min);
		tree->SetBranchAddress("max",  &max);
		tree->SetBranchAddress("num",  &num);
		tree->SetBranchAddress("min_evt",  &min_evt);
		tree->SetBranchAddress("max_evt",  &max_evt);

		bool is_dead[14] = {};
		for(auto& b : is_dead) {
			b = false;
		}
		std::map<Long64_t, Long64_t, bco_comparator_s> bco_map[14];
		for(int n = 0, N = tree->GetEntriesFast(); n < N; ++n) {
			tree->GetEntry(n);

			if(num < 5) {
				nonsense_set.insert(min);
				nonsense_set.insert(max);
				snprintf(buff, sizeof(buff), "0x%010llx", min);
				std::cout << buff << " " << min_evt << std::endl;

				if(nonsense_map.find(min_evt) == nonsense_map.end()) {
					nonsense_map[min_evt] = min;
				}

				if(min < nonsense_map[min_evt]) {
					nonsense_map[min_evt] = min;
				}


				continue;
			}

			bco_map[fee][(max + min) / 2] = num;
			if(max_bcos < num) {
				max_bcos = num;
			}
		}

		char buff[256];
		bco_comparator_s bco_less;
		Long64_t begin = std::numeric_limits<Long64_t>::max();
		Long64_t end   = std::numeric_limits<Long64_t>::max();
		int max_size = 0;

		std::cout  << "intt" << i << std::endl;
		for(fee = 0; fee < 14; ++fee) {
			std::cout << "\tfee: " << fee << std::endl;
			if(!bco_map[fee].size()) {
				std::cout << "error" << std::endl;
				is_dead[fee] = true;
				continue;
			} else if (max_size < bco_map[fee].size()) {
				max_size = bco_map[fee].size();
			}

			if(begin == std::numeric_limits<Long64_t>::max() || bco_less(bco_map[fee].begin()->first, begin)) {
				begin = bco_map[fee].begin()->first;
			} else if (end == std::numeric_limits<Long64_t>::max() || bco_less(end, bco_map[fee].rbegin()->first)) {
				end = bco_map[fee].rbegin()->first;
			}
		}
		snprintf(buff, sizeof(buff), "0x%010llx", begin);
		std::cout << "\t\tbegin: " << buff << std::endl;
		snprintf(buff, sizeof(buff), "0x%010llx", end);
		std::cout << "\t\tend:   " << buff << std::endl;
		std::cout << "\t\tsize:  " << max_size << std::endl;

		if(false) {
			gStyle->SetOptStat(0);
			snprintf(buff, sizeof(buff), "Run %08d intt%01d Canvas", run_num, i);
			TCanvas* cnvs = new TCanvas(buff, buff);
	
			cnvs->cd();
			TPad* hist_pad = new TPad("hist_pad", "hist_pad", 0.0, 0.0, 0.9, 1.0);
			hist_pad->Range(0.0, 0.0, 1.0, 1.0);
			hist_pad->Draw();
			hist_pad->cd();
			TH1* hist[14] = {}; 
			for(fee = 0; fee < 14; ++fee) {
				snprintf(buff, sizeof(buff), "Run %08d intt%01d fch%02d", run_num, i, fee);
				hist[fee] = new TH1D(buff, buff, max_size / 2, 0, (end - begin + (Long64_t{1}<<40)) % (Long64_t{1}<<40));
				for(auto const& [bco_key, num_bcos] : bco_map[fee]) {
					Long64_t bco_val = (bco_key - begin + (Long64_t{1}<<40)) % (Long64_t{1}<<40);
				 	hist[fee]->SetBinContent(hist[fee]->FindBin(bco_val), num_bcos);
				}
				hist[fee]->SetLineColor(get_fee_color(fee));
				hist[fee]->GetYaxis()->SetRangeUser(-0.1 * max_bcos, 1.1 * max_bcos);
				hist[fee]->Draw("same");
			}
	
			cnvs->cd();
			TPad* legend_pad = new TPad("legend_pad", "legend_pad", 0.9, 0.0, 1.0, 1.0);
			legend_pad->Range(0.0, 0.0, 1.0, 1.0);
			legend_pad->Draw();
			legend_pad->cd();
			for(fee = 0; fee < 14; ++fee) {
				double x = 0.5, y = (2.0 * fee + 1.0) / (2.0 * 14);
				TText text;
				text.SetTextAlign(22);
				text.SetTextSize(0.15);
				text.SetTextColor(get_fee_color(fee));
				snprintf(buff, sizeof(buff), "%02d", fee);
				text.DrawText(x, y, buff);
	
				if(!is_dead[fee]) continue;
	
				double x_corners[5] = {-1.0, +1.0, +1.0, -1.0, -1.0};
				double y_corners[5] = {-1.0, -1.0, +1.0, +1.0, -1.0};
				for(int i = 0; i < 5; ++i) {
					x_corners[i] *= 0.18; // width
					x_corners[i] += x;
	
					y_corners[i] *= 0.02; // height
					y_corners[i] += y;
				}
	
				TPolyLine box;
				box.SetLineColor(kRed);
				box.SetLineWidth(2);
				box.DrawPolyLine(5, x_corners, y_corners);
			}
	
			snprintf(buff, sizeof(buff), "run%08d_intt%01d.png", run_num, i);
			cnvs->SaveAs(buff);
		}
	}

	// for(auto const& bco : nonsense_set) {
	// 	std::cout << "\tnonsense 0x" << std::hex << bco << std::endl;
	// }

	for(auto const& [evt, bco] : nonsense_map) {
		std::cout << "\tnonsense 0x" << std::hex << bco << " at " << std::dec << evt << std::endl;
	}
}

bool
bco_comparator_s::operator() (
	Long64_t const& lhs,
	Long64_t const& rhs
) const {
	return (rhs - lhs + MAX) % MAX < (lhs - rhs + MAX) % MAX;
}

Color_t
get_fee_color (
	int const& fee
) {
	 switch (fee % 7) {
	 case 0:
		 return (fee / 7) ? kOrange : kBlack;
	 case 1:
		 return kRed + 3 * (fee / 7);
	 case 2:
		 return kViolet + 3 + 7 * (fee / 7);
	 case 3:
		 return kGreen + 3 * (fee / 7);
	 case 4:
		 return kCyan + 3 * (fee / 7);
	 case 5:
		 return kBlue + 3 * (fee / 7);
	 case 6:
		 return kMagenta + 3 * (fee / 7);
	 }
	 return kBlack;
}

#endif//GET_RUN_DURATION_C
