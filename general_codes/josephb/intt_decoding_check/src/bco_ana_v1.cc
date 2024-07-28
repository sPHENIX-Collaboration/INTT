#include "bco_ana_v1.h"

#include <Event/Event.h>
#include <phool/phool.h>

#include <stdlib.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>

bool
bco_comparator::operator() (
	bco_t const& lhs,
	bco_t const& rhs
) {
	return ((rhs % BCO_MAX) - (lhs % BCO_MAX) + 2 * BCO_MAX) % BCO_MAX < ((lhs % BCO_MAX) - (rhs % BCO_MAX) + 2 * BCO_MAX) % BCO_MAX;
}

bco_ana_v1::bco_ana_v1 (
) {
}

bco_ana_v1::~bco_ana_v1 (
) {
}

int
bco_ana_v1::set_output_file (
	std::string const& file_name
) {
	delete m_file;
	m_file = TFile::Open(file_name.c_str(), "RECREATE");

	if(!m_file) {
		std::cerr << PHWHERE << "\n"
		          << "\tCould not (re)create file: " << file_name << std::endl;
		return EXIT_FAILURE;
	}

	m_file->cd();
	delete m_tree;
	m_tree = new TTree("tree", "tree");
	m_tree->SetDirectory(m_file);

	m_file->cd();
	delete m_hist;
	m_hist = new TH1I("hist", "hist", 120, -0.5, 119.5);
	m_hist->SetDirectory(m_file);

	return EXIT_SUCCESS;
}

int
bco_ana_v1::write_output_file (
) {
	if(!m_file) {
		std::cerr << PHWHERE << "\n"
		          << "\tMember 'm_file' is null at call\n"
		          << "\tWas this preceded by a successful call to 'set_output_file'?" << std::endl;
		return EXIT_FAILURE;
	}

	if(!m_tree) {
		std::cerr << PHWHERE << "\n"
		          << "\tMember 'm_tree' is null at call\n"
		          << "\t(This state should be unreachable)" << std::endl;
		return EXIT_FAILURE;
	} else {
		Long64_t head;     m_tree->Branch("head",     &head);
		Long64_t min;      m_tree->Branch("min",      &min);
		Long64_t max;      m_tree->Branch("max",      &max);
		int      min_evt;  m_tree->Branch("min_evt",  &min_evt);
		int      max_evt;  m_tree->Branch("max_evt",  &max_evt);
		Long64_t num;      m_tree->Branch("num",      &num);

		for(auto const& [bco_head, bco_bin_struct] : m_bco_map) {
			head =    (Long64_t)bco_head;
			min =     (Long64_t)bco_bin_struct.min;
			max =     (Long64_t)bco_bin_struct.max;
			min_evt = (int)     bco_bin_struct.min_evt;
			max_evt = (int)     bco_bin_struct.max_evt;
			num =     (Long64_t)bco_bin_struct.count;

			m_tree->Fill();
		}

		m_tree->Write();
	}

	if(!m_hist) {
		std::cerr << PHWHERE << "\n"
		          << "\tMember 'm_hist' is null at call\n"
		          << "\t(This state should be unreachable)" << std::endl;
		return EXIT_FAILURE;
	} else {
		m_hist->Write();
	}

	m_file->Write();
	m_file->Close();

	return EXIT_SUCCESS;
}

int
bco_ana_v1::analyze (
	Packet* pkt
) {
	if(print_event()) {
		std::cout << "num bcos: " << std::dec << pkt->iValue(0, "NR_BCOS") << std::endl;
		std::cout << "num hits: " << std::dec << pkt->iValue(0, "NR_HITS") << std::endl;
	}

	// for(int n = 0, N = pkt->iValue(0, "NR_HITS"); n < N; ++n) {
	// 	int   fee      =  pkt->iValue(n, "FEE");
	// 	int   chip     = (pkt->iValue(n, "CHIP_ID") + 25) % 26;
	// 	int   channel  =  pkt->iValue(n, "CHANNEL_ID");
	// 	int   fphx_bco =  pkt->iValue(n, "FPHX_BCO");
	// 	int   adc      =  pkt->iValue(n, "ADC");
	// 	bco_t bco      =  pkt->lValue(n, "BCO");
	// 	// ...
	// }

	std::set<bco_t> bco_set;
	for(int n = 0, N = pkt->iValue(0, "NR_BCOS"); n < N; ++n) {
	 	bco_t bco = pkt->lValue(n, "BCOLIST");
		bco_set.insert(bco);

		bco_t bco_key = bco & m_BCO_PROJ;

		if(bco < m_bco_map[bco_key].min) {
			m_bco_map[bco_key].min = bco;
		}

		if(m_bco_map[bco_key].max < bco) {
			m_bco_map[bco_key].max = bco;
		}

		if(num_evts() < m_bco_map[bco_key].min_evt) {
			m_bco_map[bco_key].min_evt = num_evts();
		}

		if(m_bco_map[bco_key].max_evt < num_evts()) {
			m_bco_map[bco_key].max_evt = num_evts();
		}

		++m_bco_map[bco_key].count;
	}

	for(auto const& bco : bco_set) {
		if(m_prev_bco == std::numeric_limits<bco_t>::max()) {
			m_prev_bco = bco;
			continue;
		}

		if(m_prev_bco == bco) {
			continue;
		}

		signed long long diff = (signed long long)bco - (signed long long)m_prev_bco;
		while(diff < 0) {
			diff += 120;
		}
		diff = diff % 120;
		m_hist->Fill(diff);

		m_prev_bco = bco;
	}

	if(print_event()) {
		std::cout << "prev bco: 0x" << std::hex << m_prev_bco << std::endl;
	}

	return EXIT_SUCCESS;
}

