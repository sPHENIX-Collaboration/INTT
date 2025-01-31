//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in INTTHitMapEvt.h.
//
// INTTHitMapEvt(const std::string &name = "INTTHitMapEvt")
// everything is keyed to INTTHitMapEvt, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// INTTHitMapEvt::~INTTHitMapEvt()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int INTTHitMapEvt::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int INTTHitMapEvt::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int INTTHitMapEvt::process_event(PHCompositeNode *topNode)
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
// int INTTHitMapEvt::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int INTTHitMapEvt::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int INTTHitMapEvt::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int INTTHitMapEvt::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void INTTHitMapEvt::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "INTTHitMapEvt.h"

//____________________________________________________________________________..
INTTHitMapEvt::INTTHitMapEvt(
  const std::string &name,
  const int process_id_in,
  const int runnumber_in,
  const std::string output_directory_in,
  const bool isBadChannelMask_in,
  const std::string hotchannel_full_directory_in,
  const bool ApplyBcoDiff_in,
  const int bco_diff_peak_in,
  const bool ApplyHitQA_in,
  const bool clone_hit_remove_BCO_tag_in,
  const bool setADCinZaxis_tag_in,
  const std::vector<int> adc_conversion_vec_in
):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in),
  isBadChannelMask(isBadChannelMask_in),
  hotchannel_full_directory(hotchannel_full_directory_in),
  ApplyBcoDiff(ApplyBcoDiff_in),
  bco_diff_peak(bco_diff_peak_in),
  ApplyHitQA(ApplyHitQA_in),
  clone_hit_remove_BCO_tag(clone_hit_remove_BCO_tag_in),
  setADCinZaxis_tag(setADCinZaxis_tag_in),
  adc_conversion_vec(adc_conversion_vec_in)

{
  std::cout << "INTTHitMapEvt::INTTHitMapEvt(const std::string &name) Calling ctor" << std::endl;


  std::string job_index = std::to_string( process_id );
  int job_index_len = 5;
  job_index.insert(0, job_index_len - job_index.size(), '0');

  std::string runnumber_str = std::to_string( runnumber );
  int runnumber_str_len = 8;
  runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');


  output_filename = "INTTHitMapEvt";
  output_filename += (isBadChannelMask) ? "_BadChannelMasked" : ""; 
  output_filename += (ApplyBcoDiff) ? "_BcoDiffApplied" : "";
  output_filename += (ApplyHitQA) ? "_HitQA" : "";
  output_filename += (clone_hit_remove_BCO_tag) ? "_CloneHitRemovedBCO" : "";
  output_filename += (setADCinZaxis_tag) ? "_ADCinZaxis" : "";
  output_filename += Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str());

  file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()),"RECREATE");

  h2_hitmap_map.clear();

  // for (int felix_i = 0; felix_i < nFelix; felix_i++){
  //   for (int fch_i = 0; fch_i < nFelix_channel; fch_i++){
  //     h2_hitmap_map.insert(std::make_pair(Form("hitmap_F%d_Fch%d",felix_i,fch_i), new TH2D(Form("hitmap_F%d_Fch%d",felix_i,fch_i), Form("hitmap_F%d_Fch%d;Chip ID (12 -> 0);Channel ID",felix_i,fch_i),13,0,13,265,-5,260)));
  //   }
  // }

  eID_count = 0;

  PrepareHotChannel();

  check_event_bcofull_map_map.clear();

}

int INTTHitMapEvt::SetCheckEventBCOFULLsHalfLadder(std::vector<std::tuple<long long, int, int>> input_bcofull_halfladder_vec)
{
  check_event_bcofull_map_map.clear();

  for (auto &bcofull_halfladder : input_bcofull_halfladder_vec)
  {
    long long bco_full = std::get<0>(bcofull_halfladder);
    int FELIX          = std::get<1>(bcofull_halfladder);
    int FELIX_ch       = std::get<2>(bcofull_halfladder);
    
    if (check_event_bcofull_map_map.find(bco_full) == check_event_bcofull_map_map.end())
    {
      check_event_bcofull_map_map.insert(
        std::make_pair(
          bco_full, 
          std::map<std::string, int>{ { Form("%d_%d",FELIX,FELIX_ch), 1 } }
        )
      );
    }
    else {
      check_event_bcofull_map_map[bco_full].insert(
        std::make_pair(
          Form("%d_%d",FELIX,FELIX_ch), 1
        )
      );
    }
    
  }

  halfladder_specified = true;

  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTHitMapEvt::SetCheckEventBCOFULLs(std::vector<long long> input_bcofull_vec)
{
  check_event_bcofull_map_map.clear();
  for (auto &bcofull : input_bcofull_vec)
  {
    check_event_bcofull_map_map.insert(std::make_pair(bcofull, std::map<std::string, int>{}));
  }

  halfladder_specified = false;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
INTTHitMapEvt::~INTTHitMapEvt()
{
  std::cout << "INTTHitMapEvt::~INTTHitMapEvt() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int INTTHitMapEvt::Init(PHCompositeNode *topNode)
{
  std::cout << "INTTHitMapEvt::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTHitMapEvt::InitRun(PHCompositeNode *topNode)
{
  std::cout << "INTTHitMapEvt::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTHitMapEvt::process_event(PHCompositeNode *topNode)
{
  // std::cout << "INTTHitMapEvt::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  evt_inttHits_map.clear();

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

  // intteventinfo = findNode::getClass<InttEventInfo>(topNode, "INTTEVENTHEADER");
  // if (!intteventinfo)
  // {
  //   std::cout << "The INTT event header is missing, you will have no BCO information fro syncing" << std::endl;

  //   gSystem->Exit(1);
  //   exit(1);
  // }

  // intt_bco = intteventinfo->get_bco_full();

  if (inttcont->get_nhits() == 0) {
    std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - no INTT hit found" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  InttRawHit* intthit = inttcont->get_hit(0);
  intt_bco = intthit->get_bco();

  if (check_event_bcofull_map_map.size() != 0 && check_event_bcofull_map_map.find(intt_bco) == check_event_bcofull_map_map.end())
  {
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

    if (isBadChannelMask) // note : if we want to remove the hot channels
    {
      if (hot_channel_map.find(Form("%d_%d_%d_%d",server,felix_ch,chip,channel)) != hot_channel_map.end())
      {
        continue;
      }
    }

    if (ApplyBcoDiff)
    {
      if (bco_diff != bco_diff_peak && bco_diff != (bco_diff_peak + 1) && bco_diff != (bco_diff_peak - 1)){
        continue;
      }
        
    }

    if (clone_hit_remove_BCO_tag) // note : can only be one of each
    {
      if (evt_inttHits_map.find(Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)) == evt_inttHits_map.end()) // note : if it's not found, we just add it
      {
        evt_inttHits_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco, bco_diff};
      }
    }
    else // note : if we don't want to remove the clone hits
    {
      evt_inttHits_map[Form("%d",i)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco, bco_diff}; // note : only index i to make the key unique
    }

  } // note : end of INTT raw hit loop

  for (int felix_i = 0; felix_i < nFelix; felix_i++){
    for (int fch_i = 0; fch_i < nFelix_channel; fch_i++){
      
      if (check_event_bcofull_map_map[intt_bco].size() != 0 && check_event_bcofull_map_map[intt_bco].find(Form("%d_%d",felix_i,fch_i)) == check_event_bcofull_map_map[intt_bco].end())
      {
        continue;
      }

      h2_hitmap_map.insert(std::make_pair(Form("bcofull%ld_F%d_Fch%d",intt_bco,felix_i,fch_i), new TH2D(Form("bcofull%ld_F%d_Fch%d",intt_bco,felix_i,fch_i), Form("bcofull%ld_F%d_Fch%d;Chip ID (12 -> 0);Channel ID",intt_bco,felix_i,fch_i),13,0,13,265,-5,260)));
    }
  }

  for (auto &pair : evt_inttHits_map)
  {
    inttHitstr hit = pair.second;
    
    if(h2_hitmap_map.find(Form("bcofull%ld_F%d_Fch%d",intt_bco,hit.hit_server,hit.hit_felix_ch)) == h2_hitmap_map.end())
    {
      if (!halfladder_specified) {std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - hitmap not found" << std::endl;}
      continue;
    }

    
    
    //todo : //////////////////////////////////////////////////////////////////////////////////////////////////
    //todo : //                                                                                              //
    //todo : //                  25  24  23  22  21  20  19  28  17  16  15  14  13   chip_id                //
    //todo : //              0 +---+---+---+---+---+---+---+---+---+---+---+---+---+                         //
    //todo : //          c   1 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //   255    h   2 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //   ...    a ... |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //   130    n 126 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //   128      127 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //          i --- +---+---+---+---+---+---+---+---+---+---+---+---+---+                         //
    //todo : //   127    d 127 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //   126      126 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //   ...      ... |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //     2        2 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //     1        1 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
    //todo : //     0        0 +---+---+---+---+---+---+---+---+---+---+---+---+---+                         //
    //todo : //   y-axis         12  11  10   9   8   7   6   5   4   3   2   1   0  chip_id                 //
    //todo : //                                                                                              //
    //todo : //           x-axis  1   2   3   4   5   6   7   8   9  10  11  12  13                          //
    //todo : //                                                                                              //
    //todo : //                       Schematic figure of what will be drawn                                 //
    //todo : //////////////////////////////////////////////////////////////////////////////////////////////////

    // todo: the coordinate is shown above
    // note : 0 - 12 is bottom row, 13 - 25 is top row
    int convert_chipID = -(hit.hit_chip + 1)+ (1 + int((hit.hit_chip + 1) / 14)) * 13;
    int convert_chanID = int((hit.hit_chip + 1) / 14) * 255+ pow(-1, int((hit.hit_chip + 1) / 14)) * hit.hit_channel;
    int hit_adc = (setADCinZaxis_tag) ? hit.hit_adc : 1;

    h2_hitmap_map[Form("bcofull%ld_F%d_Fch%d",intt_bco,hit.hit_server,hit.hit_felix_ch)] -> Fill(convert_chipID,convert_chanID,hit_adc);
  }

  eID_count++;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTHitMapEvt::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "INTTHitMapEvt::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTHitMapEvt::EndRun(const int runnumber)
{

  file_out->cd();

  for (auto &pair : h2_hitmap_map)
  {
    pair.second->Write();
  }

  file_out->Close();


  std::cout << "INTTHitMapEvt::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTHitMapEvt::End(PHCompositeNode *topNode)
{
  std::cout << "INTTHitMapEvt::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTHitMapEvt::Reset(PHCompositeNode *topNode)
{
 std::cout << "INTTHitMapEvt::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void INTTHitMapEvt::Print(const std::string &what) const
{
  std::cout << "INTTHitMapEvt::Print(const std::string &what) const Printing info for " << what << std::endl;
}

int INTTHitMapEvt::PrepareHotChannel()
{
  hot_file_in = TFile::Open(hotchannel_full_directory.c_str());

  if (!hot_file_in)
  {
    std::cout << "INTTBcoResolution::PrepareHotChannel - hot channel file not found" << std::endl;
    gSystem->Exit(1);
    exit(1);
  }

  hot_tree_in = (TTree*)hot_file_in->Get(hot_tree_name.c_str());

  hot_tree_in->SetBranchAddress("IID",&IID);
  hot_tree_in->SetBranchAddress("Ichannel",&Ichannel);
  hot_tree_in->SetBranchAddress("Ichip",&Ichip);
  hot_tree_in->SetBranchAddress("Ifelix_channel",&Ifelix_channel);
  hot_tree_in->SetBranchAddress("Ifelix_server",&Ifelix_server);
  hot_tree_in->SetBranchAddress("Iflag",&Iflag);

  hot_channel_map.clear();

  for (int i=0; i<hot_tree_in->GetEntries(); i++)
  {
    hot_tree_in->GetEntry(i);
    hot_channel_map[Form("%d_%d_%d_%d",Ifelix_server,Ifelix_channel,Ichip,Ichannel)] = Form("%d",Iflag);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}