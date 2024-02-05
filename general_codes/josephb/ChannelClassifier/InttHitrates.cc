#include "InttHitrates.h"

#include <cstdio>
#include <iostream>
#include <map>

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/phool.h>

#include <TSystem.h>

InttHitrates::InttHitrates (
	const std::string &name
) : SubsysReco(name) {
	// do nothing
}

InttHitrates::~InttHitrates (
) {
	// do nothing
}

int
InttHitrates::Init (
	PHCompositeNode*
) {
	m_num_events = 0;
	m_hitrates.clear();
	InttNameSpace::RawData_s raw;
	while (true) {
		m_hitrates.insert({raw, 0});

		if(++raw.channel < 128)continue;
		raw.channel = 0;

		if(++raw.chip < 26)continue;
		raw.chip = 0;

		if(++raw.felix_channel < 14)continue;
		raw.felix_channel = 0;

		if(++raw.felix_server < 8)continue;
		raw.felix_server = 0;

		break;
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

int
InttHitrates::InitRun (
	PHCompositeNode*
) {
	if ( !m_file ) {
		std::cout << "InttHitrates::InitRun" << std::endl;
		std::cout << "\tMember 'm_file' not set" << std::endl;
		std::cout << "\tNo file will be written" << std::endl;	
		std::cout << "\tEnusure function 'set_output_file' is called first in enclosing macro" << std::endl;

		return Fun4AllReturnCodes::ABORTRUN;
	}

	m_num_events = 0;
	for( auto& pair : m_hitrates ) {
		pair.second = 0;
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

void
InttHitrates::set_output_file (
	std::string const& file_name
) {
	if ( file_name.empty() ) {
		std::cout << "InttHitrates::set_output_file" << std::endl;
		std::cout << "\tArgument 'file_name' is empty string" << std::endl;
		std::cout << "\tNo file will be written" << std::endl;
	}

	if ( m_file ) m_file->Close();
	m_file = TFile::Open(file_name.c_str(), "RECREATE");
	if( !m_file ) {
		std::cout << "InttHitrates::set_output_file" << std::endl;
		std::cout << "\tCould not create TFile '" << file_name << "'" << std::endl;
		std::cout << "\tNo file will be written" << std::endl;
	}
}

int
InttHitrates::process_event (
	PHCompositeNode* top_node
) {
	InttRawHitContainer* intt_cont = findNode::getClass<InttRawHitContainer>(top_node, m_InttRawNodeName);
	if ( !intt_cont ) {
		gSystem->Exit(1);
		exit(1);
	}

	InttNameSpace::RawData_s raw;
	set_t hits;
	set_t::iterator set_itr;
	map_t::iterator map_itr;

	if(intt_cont->get_nhits())++m_num_events;
	for ( unsigned int i = 0; i < intt_cont->get_nhits(); ++i ) {
		InttRawHit* intt_hit = intt_cont->get_hit(i);
		InttNameSpace::RawFromHit(raw, intt_hit);

		set_itr = hits.find(raw);
		if ( set_itr != hits.end() ) {
			continue;
		}
		hits.insert(raw);

		map_itr = m_hitrates.find(raw);
		if( map_itr == m_hitrates.end() ) {
			printf("InttNameSpace::RawData_s key not existing in member map at runtime\n");
			printf("\t%16s%4d\n", "felix_server:",	raw.felix_server);
			printf("\t%16s%4d\n", "felix_channel:",	raw.felix_channel);
			printf("\t%16s%4d\n", "chip:",		raw.chip);
			printf("\t%16s%4d\n", "channel:",	raw.channel);
			printf("\n");
			return Fun4AllReturnCodes::ABORTRUN;
		}
		++(map_itr->second);
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

int
InttHitrates::ResetEvent (
	PHCompositeNode*
) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int
InttHitrates::EndRun (
	const int
) {
	if ( !m_file ) {
		std::cout << "InttHitrates::EndRun" << std::endl;
		std::cout << "\tMember 'm_file' not set" << std::endl;
		std::cout << "\tNo file will be written" << std::endl;	

		return Fun4AllReturnCodes::ABORTRUN;
	}

	Float_t hitrate;
	InttNameSpace::RawData_s raw;
	if ( m_tree ) delete m_tree;
	m_tree = new TTree("hitrates", "hitrates");
	m_tree->SetDirectory(m_file);
	m_tree->Branch("felix_server",	&(raw.felix_server));
	m_tree->Branch("felix_channel",	&(raw.felix_channel));
	m_tree->Branch("chip",		&(raw.chip));
	m_tree->Branch("channel",	&(raw.channel));
	m_tree->Branch("hitrate",	&hitrate);

	for(auto const& pair : m_hitrates) {
		raw = pair.first;
		hitrate = pair.second;
		hitrate /= m_num_events ? m_num_events : 1.0;

		m_tree->Fill();
	}

	m_tree->Write();
	m_file->Write();
	m_file->Close();

	return Fun4AllReturnCodes::EVENT_OK;
}

int
InttHitrates::End (
	PHCompositeNode*
) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int
InttHitrates::Reset (
	PHCompositeNode*
) {
	return Fun4AllReturnCodes::EVENT_OK;
}

void
InttHitrates::Print (
	const std::string &what
) const {
	std::cout << "InttHitrates::Print(const std::string &what) const Printing info for " << what << std::endl;
}
