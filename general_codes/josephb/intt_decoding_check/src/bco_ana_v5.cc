#include "bco_ana_v5.h"

#include <Event/Event.h>
#include <phool/phool.h>

#include <TFile.h>
#include <TH2D.h>

#include <boost/format.hpp>

#include <stdlib.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>

bco_ana_v5::bco_ana_v5 (
) {
}

bco_ana_v5::~bco_ana_v5 (
) {
}

int
bco_ana_v5::set_output_file (
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

	for(int fch = 0; fch < 14; ++fch) {
		m_hist[fch] = new TH2D (
			(boost::format("hitmap_felix_channel_%02d") % fch).str().c_str(),
			(boost::format("hitmap_felix_channel_%02d") % fch).str().c_str(),
			 26, -0.5,  25.5,
			128, -0.5, 127.5
		);
		m_hist[fch]->SetDirectory(m_file);
	}

	return EXIT_SUCCESS;
}

int
bco_ana_v5::write_output_file (
) {
	if(!m_file) {
		std::cerr << PHWHERE << "\n"
		          << "\tMember 'm_file' is null at call\n"
		          << "\tWas this preceded by a successful call to 'set_output_file'?" << std::endl;
		return EXIT_FAILURE;
	}

	for(int fch = 0; fch < 14; ++fch) {
		if(!m_hist[fch]) {
			std::cerr << PHWHERE << "\n"
			          << "\tMember 'm_hist[" << fch << "]' is null at call\n"
			          << "\tWas this preceded by a successful call to 'set_output_file'?" << std::endl;
			return EXIT_FAILURE;
		}
		m_hist[fch]->Write();
	}

	m_file->Write();
	m_file->Close();

	return EXIT_SUCCESS;
}

int
bco_ana_v5::analyze (
	Packet* pkt
) {
	if(!pkt) {
		return EXIT_FAILURE;
	}

	for(int n = 0, nr_hits = pkt->iValue(0, "NR_HITS"); n < nr_hits; ++n) {
		int fee =  pkt->iValue(n, "FEE") % 14;
		int chp = (pkt->iValue(n, "CHIP_ID") + 25) % 26;
		int chn =  pkt->iValue(n, "CHANNEL_ID");
		// int adc =  pkt->iValue(n, "ADC");

		m_hist[fee]->Fill(chp, chn);
	}

	return EXIT_SUCCESS;
}

