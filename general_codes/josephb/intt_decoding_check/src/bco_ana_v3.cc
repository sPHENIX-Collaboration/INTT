#include "bco_ana_v3.h"

#include <Event/Event.h>
#include <phool/phool.h>

#include <stdlib.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>

bco_ana_v3::bco_ana_v3 (
) {
}

bco_ana_v3::~bco_ana_v3 (
) {
}

int
bco_ana_v3::analyze (
	Packet* pkt
) {
	if(print_event()) {
		std::cout << PHWHERE << std::endl;
		std::cout << "num bcos: " << std::dec << pkt->iValue(0, "NR_BCOS") << std::endl;
		std::cout << "num hits: " << std::dec << pkt->iValue(0, "NR_HITS") << std::endl;
	}

	return EXIT_SUCCESS;
}

int
bco_ana_v3::analyze (
) {
	if(print_event()) {
		std::cout << PHWHERE << std::endl;
		std::cout << "num bcos: " << std::dec << iValue(0, "NR_BCOS") << std::endl;
		std::cout << "num hits: " << std::dec << iValue(0, "NR_HITS") << std::endl;
	}

	return EXIT_SUCCESS;
}

