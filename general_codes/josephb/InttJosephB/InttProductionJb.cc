#include "InttProductionJb.h"

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>
#include <phool/phool.h> 

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <map>

InttProductionJb::InttProductionJb(std::string const& name) : SubsysReco(name)
{
	// Do nothing
}

InttProductionJb::~InttProductionJb()
{
	// Do nothing
}

int 
InttProductionJb::Init(PHCompositeNode*)// top_node)
{
	std::stringstream err_stream;
	err_stream << "InttProductionJb::Init(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttProductionJb::InitRun(PHCompositeNode*)// top_node)
{
	std::stringstream err_stream;
	err_stream << "InttProductionJb::InitRun(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttProductionJb::process_event(PHCompositeNode* top_node)
{
	std::stringstream err_stream;
	err_stream << "InttProductionJb::process_event(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	InttRawHitContainer* intt_raw_hit_container = nullptr;
	InttRawHit* intt_raw_hit = nullptr;
	InttHitJb hit;

	MapType_t::iterator itr = m_map.end();
	unsigned int i = 0;

	intt_raw_hit_container = findNode::getClass<InttRawHitContainer>(top_node, m_InttRawNodeName);
	if(!intt_raw_hit_container)
	{
		err_stream << PHWHERE << std::endl;
		err_stream << "\tCould not get node" << std::endl;
		err_stream << "\t\t\"" << m_InttRawNodeName << "\"" << std::endl;
		err_stream << "\" from top_node" << std::endl;
		return_code = Fun4AllReturnCodes::ABORTRUN;

		goto LABEL;
	}

	m_hits->clear();
	for(itr = m_map.begin(); itr != m_map.end(); ++itr)itr->second.hit = false;

	for(i = 0; i < intt_raw_hit_container->get_nhits(); ++i)
	{
		intt_raw_hit = intt_raw_hit_container->get_hit(i);
		if(!intt_raw_hit)continue;

		hit.set_gtm_bco	( intt_raw_hit->get_bco() );

		hit.set_flx_bco	( intt_raw_hit->get_FPHX_BCO() );
		hit.set_flx_svr	( intt_raw_hit->get_packetid() - 3001 );
		hit.set_flx_chn	( intt_raw_hit->get_fee() );

		hit.set_chp	( (intt_raw_hit->get_chip_id() + 25) % 26 );
		hit.set_chn	( intt_raw_hit->get_channel_id() );
		hit.set_adc	( intt_raw_hit->get_adc() );

		itr = m_map.find(hit);
		if(itr == m_map.end())
		{
			err_stream << PHWHERE << std::endl;
			err_stream << "\tMember \"m_map\" (map of channel hitrates) not properly initialized" << std::endl;
			return_code = Fun4AllReturnCodes::ABORTRUN;

			goto LABEL;
		}

		if(itr->second.hit)continue; // Clone hit removal

		itr->second.hit = true;
		++(itr->second.hits);
		m_hits->push_back(hit);
	}

	m_tree->Fill();

	LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int InttProductionJb::ResetEvent(PHCompositeNode*)// top_node)
{
	std::stringstream err_stream;
	err_stream << "InttProductionJb::ResetEvent(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	std::cout << "InttProductionJb::ResetEvent(PHCompositeNode* top_node)" << std::endl;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int InttProductionJb::EndRun(int const)//runnumber)
{
	std::stringstream err_stream;
	err_stream << "InttProductionJb::EndRun(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int InttProductionJb::End(PHCompositeNode*)// *top_node)
{
	std::stringstream err_stream;
	err_stream << "InttProductionJb::End(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int InttProductionJb::Reset(PHCompositeNode*)// *top_node)
{
	std::stringstream err_stream;
	err_stream << "InttProductionJb::Reset(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

void InttProductionJb::Print(std::string const& what) const
{
	std::cout << "InttProductionJb::Print(const std::string &what)" << what << std::endl;
}
