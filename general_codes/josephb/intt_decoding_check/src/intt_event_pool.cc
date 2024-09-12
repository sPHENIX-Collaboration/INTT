#include "intt_event_pool.h"

#include <Event/Event.h>

#include <phool/phool.h>

#include <stdlib.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>

intt_event_pool::intt_event_pool (
) {
	// m_pool = new intt_pool(1000, 100);
}

intt_event_pool::~intt_event_pool (
) {
	// delete m_pool;
	delete m_file_evt_itr;
}

int
intt_event_pool::open_list (
	std::string const& list_file
) {
	if(list_file.empty()) {
		std::cerr << PHWHERE << "\n"
		          << "\tArgument 'list_file' is empty string" << std::endl;
		return EXIT_FAILURE;
	}

	if(!std::filesystem::exists(list_file)) {
		std::cerr << PHWHERE << "\n"
		          << "\tFile '" << list_file << "' not found" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream list(list_file);
	for(std::string line; std::getline(list, line);) {
		open(line);
	}

	return EXIT_SUCCESS;
}

int
intt_event_pool::open (
	std::string const& file_name
) {
	if(file_name.empty()) {
		std::cerr << PHWHERE << "\n"
		          << "\tArgument 'file_name' is empty string" << std::endl;
		return EXIT_FAILURE;
	}

	if(!std::filesystem::exists(file_name)) {
		std::cerr << PHWHERE << "\n"
		          << "\tFile '" << file_name << "' not found" << std::endl;
		return EXIT_FAILURE;
	}

	m_files_to_read.push_back(file_name.c_str());
	if(m_verbosity) {
		std::cout << PHWHERE << "\n"
		          << "\tAppended file for reading: " << file_name << std::endl;
	}

	if(!m_file_evt_itr) {
		open_next();
	}

	return EXIT_SUCCESS;
}

int
intt_event_pool::open_next (
) {
	if(!m_files_to_read.size()) {
		if(m_verbosity) {
			std::cout << PHWHERE << "\n"
			          << "\tFinished reading" << std::endl;
		}
		return EXIT_FAILURE;
	}

	if(m_verbosity) {
		std::cout << PHWHERE << "\n"
		          << "\tOpening next file: " << *m_files_to_read.begin() << std::endl;
	}

	m_current_file = *m_files_to_read.begin();
	m_files_to_read.erase(m_files_to_read.begin(), std::next(m_files_to_read.begin()));

	delete m_file_evt_itr;
	m_file_evt_itr = new fileEventiterator(m_current_file.c_str());

	return EXIT_SUCCESS;
}

int
intt_event_pool::next (
) {
	if(!m_file_evt_itr) {
		std::cerr << PHWHERE << "\n"
		          << "\tMember 'm_file_evt_itr' is null at call\n"
		          << "\tWas this preceded by a successful call to 'open' or 'open_next'?" << std::endl;
		return EXIT_FAILURE;
	}

	if(print_event()) {
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "file: " << m_current_file << std::endl;
		std::cout << "event: " << std::dec << m_evts << std::endl;
	}

	Event* evt = m_file_evt_itr->getNextEvent();
	while(!evt) {
		if(open_next()) {
			return EXIT_FAILURE;
		}
		evt = m_file_evt_itr->getNextEvent();
	}

	for(int pid = 3001; pid < 3009; ++pid) {
		Packet* pkt = evt->getPacket(pid);
		if(!pkt)continue;
		analyze(pkt);
		// m_pool->addPacket(pkt);
		delete pkt;
	}
	delete evt;

	// if(m_pool->depth_ok()) {
	// 	analyze();
	// 	m_pool->next();
	// }
	++m_evts;

	return EXIT_SUCCESS;
}

