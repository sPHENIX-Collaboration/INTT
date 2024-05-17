#include "InttCloneRawHitQA.h"

using namespace std;

//____________________________________________________________________________..
InttCloneRawHitQA::InttCloneRawHitQA(const std::string &name, bool is_official ):
  InttRawHitQA(name, is_official)
{
  output_basename_ = "InttCloneRawHitQA_run";
}

//____________________________________________________________________________..
InttCloneRawHitQA::~InttCloneRawHitQA()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////

vector < InttRawHit* > InttCloneRawHitQA::GetHits()
{
  
  vector < InttRawHit* > hits;
  auto raw_hit_num = node_inttrawhit_map_->get_nhits();
    
  for (unsigned int i = 0; i < raw_hit_num; i++)
  {
    auto hit = node_inttrawhit_map_->get_hit(i);
    int felix = hit->get_packetid() - kFirst_pid_;
    int felix_ch = hit->get_fee();
    int chip = hit->get_chip_id();
    int chan = hit->get_channel_id();
    auto adc = hit->get_adc();
    auto bco = hit->get_FPHX_BCO();
    
    if (clone_hit_flag_[felix][felix_ch][chip-1][chan]) // case of clone hit
      {
	// loop for clone hit info.
	hits.push_back( hit );
      }
    else // case of non-clone hit
      {
	// loop for something related to clone hit QA
	// In this loop, hit without clone hit will be used.
	clone_hit_flag_[felix][felix_ch][chip-1][chan] = true;
      }
  }
  
return hits;
}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// Fun4All stuff
/////////////////////////////////////////////////////////////////////////

int InttCloneRawHitQA::Init(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}


int InttCloneRawHitQA::process_event(PHCompositeNode *topNode)
{

  // Get nodes to access data
  auto status = this->GetNodes(topNode);

  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  vector < InttRawHit* > hits = this->GetHits();
  
  auto hit_num = hits.size();
  hist_nhit_->Fill( hit_num );
  
  auto bco_full = node_intteventheader_map_->get_bco_full();
  hist_bco_full_->Fill( bco_full );

  // initial loop to get some values    
  uint32_t event_counter_ref = 100000000; // 100 M
  for (unsigned int i = 0; i < hit_num; i++)
  {
    auto hit = node_inttrawhit_map_->get_hit(i);

    // Because *min_element crashed the Fun4All process, I have to do it...
    if( hit->get_event_counter() < event_counter_ref )
      {
	event_counter_ref = hit->get_event_counter();
      }
      
  }

  // loop over all raw hits
  bool found = false;
  for (unsigned int i = 0; i < hits.size(); i++)
  {
    auto hit = hits[i];

    int felix = hit->get_packetid() - kFirst_pid_;
    int felix_ch = hit->get_fee();
    int chip = hit->get_chip_id();
    int chan = hit->get_channel_id();
    auto adc = hit->get_adc();
    auto bco = hit->get_FPHX_BCO();
    
    int bco_diff = (bco_full & 0x7f ) - bco;
    if( bco_diff < 0 )
      bco_diff += 128;
    
    // The raw Chip ID can be more than 26
    if (chip > kChip_num_)
    {
      chip = chip - kChip_num_;
    }
    
    hist_fee_chip_chan_[felix]->Fill(felix_ch, chip, chan);
    hist_adc_->Fill(hit->get_adc());
    hist_bco_->Fill(hit->get_FPHX_BCO());

    hist_fee_bco_full_event_counter_[felix]
        ->Fill(felix_ch, // chip, chan );
               hit->get_bco(),
               hit->get_event_counter());

    hist_fee_bco_full_event_counter_diff_[felix]
        ->Fill(felix_ch,
               hit->get_bco() - bco_full,
               hit->get_event_counter() - event_counter_ref);
    if (felix < 4)
      hist_nhit_south_->Fill(hit->get_event_counter());
    else
      hist_nhit_north_->Fill(hit->get_event_counter());
    
    if( false )
    //if (hit->get_event_counter() - event_counter_ref != 0)
    //if( !found )
    {
      found = true;
      cout << setw(6) << i << " "
           << setw(13) << event_counter_ref
           << setw(10) << hit->get_event_counter() << " "
           << setw(4) << "felix " << hit->get_packetid() << " " // Packet ID
           << setw(3) << hit->get_fee() << " "                  // FELIX CH (module)
           << setw(4) << chip << " "                            // chip_id
           << setw(4) << hit->get_channel_id() << " "           // chan_id
           << setw(3) << hit->get_adc() << " "                  // adc
           << setw(3) << hit->get_FPHX_BCO() << " "             // bco
           << setw(12) << hit->get_bco() << " "                 // bco_full
           << setw(3) << (hit->get_bco() & 0x7F) - hit->get_FPHX_BCO() << " "
           << endl;
    }
  }

  if( found )
    {
	cout << setw(6) << "index" << " "
	     << setw(30) << "Eve Cnter Ref" << " "
	     << setw(10) << "Eve Cnter" << " "
	     << setw(4) << "felix pid "
	     << setw(3) << "fee" << " "        // FELIX CH (module)
	     << setw(4) << "chip" << " "
	     << setw(4) << "chan" << " "
	     << setw(3) << "ADC" << " " 
	     << setw(3) << "BCO" << " " 
	     << setw(12) << "BCO full" << " "
	     << setw(3) << "BCO diff" << " "
	     << endl;

    }

  return Fun4AllReturnCodes::EVENT_OK;
}


int InttCloneRawHitQA::ResetEvent(PHCompositeNode *topNode)
{

  // Intitialize for Clone hit counter
  // cout << "before memset" << endl;
  memset(clone_hit_flag_, false, sizeof(clone_hit_flag_));
  
  return Fun4AllReturnCodes::EVENT_OK;
}

/*
int InttCloneRawHitQA::EndRun(const int runnumber)
{

  
  this->ProcessHists();
  
  // // Draw hits and save them into a PDF file.
  this->DrawHists();
  
  // write 3D hists into a ROOT file
  for( auto& hist : hist_fee_chip_chan_ )
    {
      tf_output_->WriteTObject( hist, hist->GetName() );
    }

    for (auto &hist : hist_fee_chip_chan_woclonehit_)
    {
      tf_output_->WriteTObject(hist, hist->GetName());
    }

  // write 1D hists into a ROOT file
  tf_output_->WriteTObject( hist_nhit_, hist_nhit_->GetName() );
  tf_output_->WriteTObject( hist_nhit_south_, hist_nhit_south_->GetName() );
  tf_output_->WriteTObject( hist_nhit_north_, hist_nhit_north_->GetName() );
  tf_output_->WriteTObject( hist_pid_, hist_pid_->GetName() );
  tf_output_->WriteTObject( hist_adc_, hist_adc_->GetName() );
  tf_output_->WriteTObject( hist_bco_, hist_bco_->GetName() );
  tf_output_->WriteTObject( hist_bco_full_, hist_bco_full_->GetName() );

  // for( auto hist : hist_event_counter_ )
  //   tf_output_->WriteTObject( hist, hist->GetName() );

  // for( int felix=0; felix<kFelix_num_; felix++ )
  //   tf_output_->WriteTObject( hist_event_counter_[felix], hist_event_counter_[felix]->GetName() );

  // Close the ROOT file

  tf_output_->WriteTObject( g_event_hit_num_, g_event_hit_num_->GetName() );
  tf_output_->WriteTObject( g_event_hit_num_strange_peak_, g_event_hit_num_strange_peak_->GetName() );

  tf_output_->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}


int InttCloneRawHitQA::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttCloneRawHitQA::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}
*/

