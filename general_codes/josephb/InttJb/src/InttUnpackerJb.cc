#include "InttUnpackerJb.h"

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

InttUnpackerJb::InttUnpackerJb(std::string const& name) : SubsysReco(name)
{
	// Do nothing
}

InttUnpackerJb::~InttUnpackerJb()
{
	if(m_file)m_file->Close();

	Clear();
	delete m_evt;
	delete m_tree;
	delete m_file;
}

int 
InttUnpackerJb::Init(PHCompositeNode*)// top_node)
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::Init(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	if(Verbosity())
	{
		std::cout << std::endl;
		std::cout << "int InttUnpackerJb::Init(PHCompositeNode* top_node)" << std::endl;
		std::cout << "\t(called)" << std::endl;
		std::cout << std::endl;
	}

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttUnpackerJb::InitRun(PHCompositeNode*)// top_node)
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::InitRun(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	if(Verbosity())
	{
		std::cout << std::endl;
		std::cout << "int InttUnpackerJb::InitRun(PHCompositeNode* top_node)" << std::endl;
		std::cout << "\t(called)" << std::endl;
		std::cout << std::endl;
	}

	MapStruct_s tmp;
	HitVer_t* hit = nullptr;

	if(!m_file)
	{
		err_stream << "\tMember function" << std::endl;
		err_stream << "\t\tint InttUnpackerJb::SetOutputFile(std::string const& filename)" << std::endl;
		err_stream << "\tmust be (successfully) called before starting the run" << std::endl;
		return_code = Fun4AllReturnCodes::ABORTRUN;

		goto LABEL;
	}

	m_file->cd();

	delete m_evt;
	m_evt = new EvtVer_t();

	delete m_tree;
	m_tree = new TTree(m_hits_tree_name.c_str(), m_hits_tree_name.c_str());
	m_tree->Branch(m_hits_hits_branch_name.c_str(), &m_evt);
	m_tree->SetDirectory(m_file);

	Clear();
	for(uint8_t flx_svr = 0; flx_svr < 8; ++flx_svr) {
		for(uint8_t flx_chn = 0; flx_chn < 14; ++flx_chn) {
			for(uint8_t chp = 0; chp < 26; ++chp) {
				for(uint8_t chn = 0; chn < 128; ++chn) {
					hit = new HitVer_t();

					hit->set_flx_svr(flx_svr);
					hit->set_flx_chn(flx_chn);
					hit->set_chp(chp);
					hit->set_chn(chn);

					tmp.hit = false;
					tmp.hits = 0;
					m_map[(InttHitJb*)hit] = tmp;
				}
			}
		}
	}

	LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttUnpackerJb::process_event(PHCompositeNode* top_node)
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::process_event(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	if(Verbosity())
	{
		std::cout << std::endl;
		std::cout << "int InttUnpackerJb::process_event(PHCompositeNode* top_node)" << std::endl;
		std::cout << "\t(called)" << std::endl;
		std::cout << std::endl;
	}

	InttRawHitContainer* intt_raw_hit_container = nullptr;
	InttRawHit* intt_raw_hit = nullptr;
	HitVer_t hit;

	MapType_t::iterator itr = m_map.end();
	unsigned int i = 0;

	intt_raw_hit_container = findNode::getClass<InttRawHitContainer>(top_node, m_intt_raw_node_name);
	if(!intt_raw_hit_container)
	{
		err_stream << PHWHERE << std::endl;
		err_stream << "\tCould not get node" << std::endl;
		err_stream << "\t\t\"" << m_intt_raw_node_name << "\"" << std::endl;
		err_stream << "\tfrom top_node" << std::endl;
		return_code = Fun4AllReturnCodes::ABORTRUN;

		goto LABEL;
	}

	m_evt->clear();
	for(itr = m_map.begin(); itr != m_map.end(); ++itr)itr->second.hit = false;
	for(i = 0; i < intt_raw_hit_container->get_nhits(); ++i)
	{
		intt_raw_hit = intt_raw_hit_container->get_hit(i);
		if(!intt_raw_hit)continue;

		hit.set_gtm_bco	( intt_raw_hit->get_bco() & 0x7f);

		hit.set_flx_bco	( intt_raw_hit->get_FPHX_BCO() );
		hit.set_flx_svr	( intt_raw_hit->get_packetid() - 3001 );
		hit.set_flx_chn	( intt_raw_hit->get_fee() );

		hit.set_chp	( (intt_raw_hit->get_chip_id() + 25) % 26 );
		hit.set_chn	( intt_raw_hit->get_channel_id() );
		hit.set_adc	( intt_raw_hit->get_adc() );

		itr = m_map.find((InttHitJb*)&hit);
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
		++(itr->second.adc[hit.get_adc()]);

		m_evt->push_back(&hit);
	}

	m_tree->Fill();

	LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttUnpackerJb::ResetEvent(PHCompositeNode*)// top_node)
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::ResetEvent(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	if(Verbosity())
	{
		std::cout << std::endl;
		std::cout << "int InttUnpackerJb::ResetEvent(PHCompositeNode* top_node)" << std::endl;
		std::cout << "\t(called)" << std::endl;
		std::cout << std::endl;
	}

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttUnpackerJb::EndRun(int const)//runnumber)
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::EndRun(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttUnpackerJb::End(PHCompositeNode*)// *top_node)
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::End(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttUnpackerJb::Reset(PHCompositeNode*)// *top_node)
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::Reset(PHCompositeNode* top_node)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

void
InttUnpackerJb::Print(std::string const& what) const
{
	std::stringstream err_stream;
	err_stream << "void InttUnpackerJb::Print(std::string const& what)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	std::cout << "void InttUnpackerJb::Print(const std::string &what)" << what << std::endl;

	// LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
}

int
InttUnpackerJb::SetOutputFile(std::string const& filename)
{
	std::stringstream err_stream;
	err_stream << "void InttUnpackerJb::SetOutputFile(std::string const& filename)" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	if(filename.empty())
	{
		err_stream << "\tArgument \"filename\" is empty string" << std::endl;
		err_stream << "\tExiting" << std::endl;
		return_code = Fun4AllReturnCodes::ABORTRUN;

		goto LABEL;
	}

	if(m_file)m_file->Close();
	m_file = TFile::Open(filename.c_str(), "RECREATE");
	if(!m_file)
	{
		err_stream << "\tFailed to (re)create file:" << std::endl;
		err_stream << "\t\t" << filename << std::endl;
		err_stream << "\tExiting" << std::endl;
		return_code = Fun4AllReturnCodes::ABORTRUN;

		goto LABEL;
	}

	m_file->cd();
	if(m_tree)m_tree->SetDirectory(m_file);

	LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

int
InttUnpackerJb::WriteOutputFile()
{
	std::stringstream err_stream;
	err_stream << "int InttUnpackerJb::WriteOutputFile()" << std::endl;
	auto return_code = Fun4AllReturnCodes::EVENT_OK;

	m_file->cd();

	TTree* hitrates_tree = nullptr;
	HitVer_t* hit;
	Double_t hitrate;
	Double_t adc_dis[8];

	TTree* version_tree = nullptr;
	TString version;

	if(!m_file)
	{
		err_stream << "\tMember \"m_file\" uninitialized" << std::endl;
		return_code = Fun4AllReturnCodes::ABORTRUN;

		goto LABEL;
	}

	hit = new HitVer_t();
	hitrates_tree = new TTree(m_hitrates_tree_name.c_str(), m_hitrates_tree_name.c_str());
	hitrates_tree->Branch(m_hitrates_hit_branch_name.c_str(), &hit);
	hitrates_tree->Branch(m_hitrates_hitrate_branch_name.c_str(), &hitrate);
	hitrates_tree->Branch(m_hitrates_adc_dis_branch_name.c_str(), adc_dis);

	hitrates_tree->SetDirectory(m_file);
	for(MapType_t::const_iterator itr = m_map.begin(); itr != m_map.end(); ++itr)
	{
		hit->copy(itr->first);
		hitrate = (Double_t)(itr->second.hits) / (Double_t)(m_tree->GetEntriesFast());
		for(uint8_t i = 0; i < 8; ++i)
		{
			adc_dis[i] = (Double_t)(itr->second.adc[i]) / (Double_t)(m_tree->GetEntriesFast());
		}

		hitrates_tree->Fill();
	}
	delete hit;

	m_file->cd();
	version_tree = new TTree(m_version_tree_name.c_str(), m_version_tree_name.c_str());
	version_tree->Branch(m_version_version_branch_name.c_str(), &version);
	version = TString(m_evt->version().c_str());
	version_tree->Fill();

	m_file->cd();
	m_tree->Write();

	hitrates_tree->Write();
	version_tree->Write();

	m_file->Write();
	m_file->Close();

	LABEL:
	err_stream << std::ends;
	if(return_code != Fun4AllReturnCodes::EVENT_OK)std::cerr << err_stream.str();	
	return return_code;
}

void
InttUnpackerJb::Clear()
{
	for(auto itm : m_map) {
		delete itm.first;
	}
	m_map.clear();
}
