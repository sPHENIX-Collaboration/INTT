#include "bco_ana_v4.h"

#include <Event/Event.h>
#include <phool/phool.h>

#include <stdlib.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>

bco_ana_v4::bco_ana_v4 (
) {
}

bco_ana_v4::~bco_ana_v4 (
) {
}

int
bco_ana_v4::set_output_file (
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

	m_tree->Branch("data_event", &m_data_event);
	for(int fee = 0; fee < 14; ++fee) {
		char buff[256];
		snprintf(buff, sizeof(buff), "cade_count_%02d", fee);
		m_tree->Branch(buff, &(m_cade_count[fee]));
	}
	for(int fee = 0; fee < 14; ++fee) {
		char buff[256];
		snprintf(buff, sizeof(buff), "byte_count_%02d", fee);
		m_tree->Branch(buff, &(m_byte_count[fee]));
	}

	return EXIT_SUCCESS;
}

int
bco_ana_v4::write_output_file (
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
		          << "\tWas this preceded by a successful call to 'set_output_file'?" << std::endl;
		return EXIT_FAILURE;
	}

	m_tree->Write();
	m_file->Write();
	m_file->Close();

	return EXIT_SUCCESS;
}

int
bco_ana_v4::analyze (
	Packet* pkt
) {
	if(!pkt)return EXIT_SUCCESS;
	++m_data_event;

	for(int fee = 0; fee < 14; ++fee) {
		for(int i = 0; i < pkt->iValue(fee, "FEE_LENGTH"); ++i) {
			m_byte_count[fee] += sizeof(int);
			if((pkt->iValue(fee, i, "") & 0xff00ffff) != 0xad00cade)continue;
			++m_cade_count[fee];
		}
	}

	if(m_tree)m_tree->Fill();

	if(print_event()) {
		std::cout << "data event: " << m_data_event << std::endl;
		char buff[256];
		for(int fee = 0; fee < 14; ++fee) {
			snprintf(buff, sizeof(buff), "\tcade_count_%02d:", fee);
			std::cout << buff << m_cade_count[fee] << std::endl;
			snprintf(buff, sizeof(buff), "\tbyte_count_%02d:", fee);
			std::cout << buff << m_byte_count[fee] << std::endl;
		}
	}

	return EXIT_SUCCESS;
}

