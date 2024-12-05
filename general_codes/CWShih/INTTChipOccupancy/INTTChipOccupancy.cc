//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in INTTChipOccupancy.h.
//
// INTTChipOccupancy(const std::string &name = "INTTChipOccupancy")
// everything is keyed to INTTChipOccupancy, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// INTTChipOccupancy::~INTTChipOccupancy()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int INTTChipOccupancy::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int INTTChipOccupancy::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int INTTChipOccupancy::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int INTTChipOccupancy::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int INTTChipOccupancy::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int INTTChipOccupancy::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int INTTChipOccupancy::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void INTTChipOccupancy::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "INTTChipOccupancy.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
INTTChipOccupancy::INTTChipOccupancy(
  const std::string &name,
  const int process_id_in,
  const int runnumber_in,
  const std::string output_directory_in,
  const bool ApplyHitQA_in,
  const bool clone_hit_remove_BCO_tag_in
):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in),
  ApplyHitQA(ApplyHitQA_in),
  clone_hit_remove_BCO_tag(clone_hit_remove_BCO_tag_in)

{
  std::cout << "INTTChipOccupancy::INTTChipOccupancy(const std::string &name) Calling ctor" << std::endl;

  std::string job_index = std::to_string( process_id );
  int job_index_len = 5;
  job_index.insert(0, job_index_len - job_index.size(), '0');

  std::string runnumber_str = std::to_string( runnumber );
  int runnumber_str_len = 8;
  runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');


  output_filename = "ChipOccupancy";
  output_filename += (ApplyHitQA) ? "_HitQA" : "";
  output_filename += (clone_hit_remove_BCO_tag) ? "_CloneHitRemovedBCO" : "";
  output_filename += Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str());

  file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()),"RECREATE");

  h1_nChipHit_map.clear();

  for (int felix_i = 0; felix_i < nFelix; felix_i++){
    for (int fch_i = 0; fch_i < nFelix_channel; fch_i++){
      for (int chip_i = 0; chip_i < nChip; chip_i++){
        h1_nChipHit_map.insert(std::make_pair(Form("nChipHit_F%d_Fch%d_Chip%d",felix_i,fch_i,chip_i), new TH1D(Form("nChipHit_F%d_Fch%d_Chip%d",felix_i,fch_i,chip_i), Form("nChipHit_F%d_Fch%d_Chip%d;nHits (per chip and HitBco);Entries",felix_i,fch_i,chip_i), 150, 0, 150)));
        // for (int hitbco_i = 0; hitbco_i < nHitBco; hitbco_i++){
        //   h1_nChipHit_map.insert(std::make_pair(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",felix_i,fch_i,chip_i,hitbco_i), new TH1D(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",felix_i,fch_i,chip_i,hitbco_i), Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d;nHits (per chip and HitBco);Entries",felix_i,fch_i,chip_i,hitbco_i), 150, 0, 150)));
        // }
      }
      
    }
  }

  eID_count = 0;

}

//____________________________________________________________________________..
INTTChipOccupancy::~INTTChipOccupancy()
{
  std::cout << "INTTChipOccupancy::~INTTChipOccupancy() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int INTTChipOccupancy::Init(PHCompositeNode *topNode)
{
  std::cout << "INTTChipOccupancy::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTChipOccupancy::InitRun(PHCompositeNode *topNode)
{
  std::cout << "INTTChipOccupancy::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTChipOccupancy::process_event(PHCompositeNode *topNode)
{
  // std::cout << "INTTChipOccupancy::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  evt_inttHits_map.clear();
  evt_ChipHit_count_map.clear();

  inttcont = nullptr;
  inttcont = findNode::getClass<InttRawHitContainer>(topNode, m_InttRawNodeName);
  if (!inttcont)
  {
    std::cout << PHWHERE << std::endl;
    std::cout << "InttCombinedRawDataDecoder::process_event(PHCompositeNode* topNode)" << std::endl;
    std::cout << "Could not get \"" << m_InttRawNodeName << "\" from Node Tree" << std::endl;
    std::cout << "Exiting" << std::endl;

    gSystem->Exit(1);
    exit(1);
  }

  if (inttcont->get_nhits() == 0) {
    std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - no INTT hit found" << std::endl;
    eID_count++;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  for (unsigned int i = 0; i < inttcont->get_nhits(); i++)
  {
    InttRawHit* intthit = inttcont->get_hit(i);

    int bco_full = intthit->get_bco();

    int server = intthit->get_packetid() - Felix_offset; // note : the felix server ID
    int felix_ch = intthit->get_fee(); // note : the felix channel ID 0 - 13
    int chip = (intthit->get_chip_id() - 1) % 26; // note : chip ID 0 - 25
    int channel = intthit->get_channel_id();
    int adc = intthit->get_adc();
    int bco = intthit->get_FPHX_BCO();
    int bco_diff = (bco - (bco_full & 0x7fU) + 128) % 128;

    // note : sample the hit properties
    if (eID_count % 1000 == 0 && i%25 == 0) {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - server: "<< server << " felix_ch: "<< felix_ch << " chip: "<< chip << " channel: "<< channel << " adc: "<< adc << " bco: "<< bco << " bco_diff: "<< bco_diff << std::endl;
    }

    if (ApplyHitQA && (server < 0 || server > 7)) // note : server ID 0 - 7
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - server out of range, this server is : "<< server << std::endl;
      continue;
    }

    if (ApplyHitQA && (felix_ch < 0 || felix_ch > 13)) // note : felix channel ID 0 - 13
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - felix channel out of range, this channel is : "<< felix_ch << std::endl;
      continue;
    }

    if (ApplyHitQA && (chip < 0 || chip > 25)) // note : chip ID 0 - 25
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - chip out of range, this chip is : "<< chip << std::endl;
      continue;
    }

    if (ApplyHitQA && (channel < 0 || channel > 127)) // note : channel ID 0 - 127
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - channel out of range, this channel is : "<< channel << std::endl;
      continue;
    }

    if (ApplyHitQA && (adc < 0 || adc > 7)) // note : adc ID 0 - 7
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - adc out of range, this adc is : "<< adc << std::endl;
      continue;
    }
    
    if (ApplyHitQA && (bco < 0 || bco > 127)) // note : bco ID 0 - 127
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - bco out of range, this bco is : "<< bco << std::endl;
      continue;
    }

    if (clone_hit_remove_BCO_tag) // note : can only be one of each
    {
      if (evt_inttHits_map.find(Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)) == evt_inttHits_map.end()) // note : if it's not found, we just add it
      {
        evt_inttHits_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc,bco, bco_diff};
      }
    }
    else // note : if we don't want to remove the clone hits
    {
      evt_inttHits_map[Form("%d",i)] = {server,felix_ch,chip,channel,adc,bco, bco_diff}; // note : only index i to make the key unique
    }

  } // note : end of INTT raw hit loop


  for (auto &pair : evt_inttHits_map)
  {
    inttHitstr hit = pair.second; 

    if (evt_ChipHit_count_map.find(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",hit.hit_server,hit.hit_felix_ch,hit.hit_chip,hit.hit_bco)) == evt_ChipHit_count_map.end()) // note : if it's not found, we just add it
    {
      evt_ChipHit_count_map[Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",hit.hit_server,hit.hit_felix_ch,hit.hit_chip,hit.hit_bco)] = 1;
    }
    else
    {
      evt_ChipHit_count_map[Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",hit.hit_server,hit.hit_felix_ch,hit.hit_chip,hit.hit_bco)] += 1;
    }
  }

  for (auto &pair : evt_ChipHit_count_map)
  {
    std::string evt_ChipHit_count_map_key = pair.first;
    std::string bco_removed_key = evt_ChipHit_count_map_key.substr(0,evt_ChipHit_count_map_key.find("_HitBco"));

    if (h1_nChipHit_map.find(bco_removed_key) == h1_nChipHit_map.end()) // note : if it's not found, we just add it
    {
      std::cout << "eID: "<< eID_count <<" INTTChipOccupancy::process_event - h1_nChipHit_map key not found, this key is : "<< bco_removed_key << std::endl;
      continue;
    }

    h1_nChipHit_map[bco_removed_key]->Fill(pair.second);
  }

  eID_count++;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTChipOccupancy::ResetEvent(PHCompositeNode *topNode)
{
  std::cout << "INTTChipOccupancy::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTChipOccupancy::EndRun(const int runnumber)
{

  file_out->cd();

  for (auto &pair : h1_nChipHit_map)
  {
    pair.second->Write();
  }

  file_out->Close();

  std::cout << "INTTChipOccupancy::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTChipOccupancy::End(PHCompositeNode *topNode)
{
  std::cout << "INTTChipOccupancy::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTChipOccupancy::Reset(PHCompositeNode *topNode)
{
 std::cout << "INTTChipOccupancy::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void INTTChipOccupancy::Print(const std::string &what) const
{
  std::cout << "INTTChipOccupancy::Print(const std::string &what) const Printing info for " << what << std::endl;
}
