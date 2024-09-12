#include "bco_ana_v2.h"

#include <Event/Event.h>
#include <phool/phool.h>

#include <stdlib.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>

bco_ana_v2::bco_ana_v2 (
) {
}

bco_ana_v2::~bco_ana_v2 (
) {
}

int
bco_ana_v2::set_output_file (
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

	return EXIT_SUCCESS;
}

int
bco_ana_v2::write_output_file (
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
		int      fee;      m_tree->Branch("fee",      &fee);
		Long64_t head;     m_tree->Branch("head",     &head);
		Long64_t min;      m_tree->Branch("min",      &min);
		Long64_t max;      m_tree->Branch("max",      &max);
		int      min_evt;  m_tree->Branch("min_evt",  &min_evt);
		int      max_evt;  m_tree->Branch("max_evt",  &max_evt);
		Long64_t num;      m_tree->Branch("num",      &num);

		for(fee = 0; fee < 14; ++fee) {
			for(auto const& [bco_head, bco_bin_s] : m_bco_map[fee]) {
				head =    (Long64_t) bco_head;
				min =     (Long64_t) bco_bin_s.min;
				max =     (Long64_t) bco_bin_s.max;
				min_evt = (int)      bco_bin_s.min_evt;
				max_evt = (int)      bco_bin_s.max_evt;
				num =     (Long64_t)(bco_bin_s.count + bco_bin_s.bco_set.size());

				m_tree->Fill();
			}
		}

		m_tree->Write();
	}

	m_file->Write();
	m_file->Close();

	return EXIT_SUCCESS;
}

int
bco_ana_v2::analyze (
	Packet* pkt
) {
	if(print_event()) {
		std::cout << "num bcos: " << std::dec << pkt->iValue(0, "NR_BCOS") << std::endl;
		std::cout << "num hits: " << std::dec << pkt->iValue(0, "NR_HITS") << std::endl;
	}

	for(int n = 0, N = pkt->iValue(0, "NR_HITS"); n < N; ++n) {
		int   fee      =  pkt->iValue(n, "FEE");
		bco_t bco      =  pkt->lValue(n, "BCO");

		bco_bin_s& el = m_bco_map[fee][(bco & m_BCO_PROJ)];

		if(bco < el.min) {
			el.min = bco;
		} else if (el.max < bco) {
			el.max = bco;
		}

		if(num_evts() < el.min_evt) {
			el.min_evt = num_evts();
		} else if (el.max_evt < num_evts()) {
			el.max_evt = num_evts();
		}

		el.bco_set.insert(bco);
		if(el.bco_set.size() < 100) {
			continue;
		}

		std::set<bco_t>::const_iterator bco_itr = el.bco_set.begin();
		for(int i = 0; i < 10; ++i) {
			++el.count;
			++bco_itr;
		}
		el.bco_set.erase(el.bco_set.begin(), bco_itr);
	}

	return EXIT_SUCCESS;
}

