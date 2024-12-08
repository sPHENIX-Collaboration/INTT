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
  const bool clone_hit_remove_BCO_tag_in,
  const bool MBDNS_trigger_require_tag_in,
  const int trigger_MBDvtxZ_cm_in,
  const std::vector<int> adc_conversion_vec_in
):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in),
  ApplyHitQA(ApplyHitQA_in),
  clone_hit_remove_BCO_tag(clone_hit_remove_BCO_tag_in),
  MBDNS_trigger_require_tag(MBDNS_trigger_require_tag_in),
  trigger_MBDvtxZ_cm(trigger_MBDvtxZ_cm_in),
  adc_conversion_vec(adc_conversion_vec_in)

{
  std::cout << "INTTChipOccupancy::INTTChipOccupancy(const std::string &name) Calling ctor" << std::endl;

  if (trigger_MBDvtxZ_cm != 10 && trigger_MBDvtxZ_cm != 30) {
    std::cout<<"INTTChipOccupancy::INTTChipOccupancy - trigger_MBDvtxZ_cm is not 10 or 30, please check the input"<<std::endl;
    exit(1);
  }


  std::string job_index = std::to_string( process_id );
  int job_index_len = 5;
  job_index.insert(0, job_index_len - job_index.size(), '0');

  std::string runnumber_str = std::to_string( runnumber );
  int runnumber_str_len = 8;
  runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');


  output_filename = "ChipOccupancy";
  output_filename += (ApplyHitQA) ? "_HitQA" : "";
  output_filename += (clone_hit_remove_BCO_tag) ? "_CloneHitRemovedBCO" : "";
  output_filename += (MBDNS_trigger_require_tag) ? "_MBDNSTrig" : "";
  output_filename += (MBDNS_trigger_require_tag) ? Form("vtxZ%dcm",trigger_MBDvtxZ_cm) : "";
  output_filename += Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str());

  file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()),"RECREATE");
  SaturationChip_tree_out = new TTree("tree","saturation chip");
  SaturationChip_tree_out -> Branch("INTT_bco_full", &out_INTT_bco_full);
  SaturationChip_tree_out -> Branch("FELIX", &out_FELIX);
  SaturationChip_tree_out -> Branch("FELIX_ch", &out_FELIX_ch);
  SaturationChip_tree_out -> Branch("chip_id", &out_chip_id);
  SaturationChip_tree_out -> Branch("nHits", &out_nHits);

  SaturationChip_tree_out -> Branch("adc_vec", &out_adc_vec);
  SaturationChip_tree_out -> Branch("size_vec", &out_size_vec);
  SaturationChip_tree_out -> Branch("edge_l_vec", &out_edge_l_vec);
  SaturationChip_tree_out -> Branch("edge_r_vec", &out_edge_r_vec);
  SaturationChip_tree_out -> Branch("pos_vec", &out_pos_vec);
  SaturationChip_tree_out -> Branch("largest_size", &out_largest_size);

  h1_nChipHit_map.clear();

  for (int felix_i = 0; felix_i < nFelix; felix_i++){
    for (int fch_i = 0; fch_i < nFelix_channel; fch_i++){
      for (int chip_i = 0; chip_i < nChip; chip_i++){
        h1_nChipHit_map.insert(std::make_pair(Form("nChipHit_F%d_Fch%d_Chip%d",felix_i,fch_i,chip_i), new TH1D(Form("nChipHit_F%d_Fch%d_Chip%d",felix_i,fch_i,chip_i), Form("nChipHit_F%d_Fch%d_Chip%d;nHits (per chip and per HitBco);Entries",felix_i,fch_i,chip_i), 150, 0, 150)));
        // for (int hitbco_i = 0; hitbco_i < nHitBco; hitbco_i++){
        //   h1_nChipHit_map.insert(std::make_pair(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",felix_i,fch_i,chip_i,hitbco_i), new TH1D(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",felix_i,fch_i,chip_i,hitbco_i), Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d;nHits (per chip and HitBco);Entries",felix_i,fch_i,chip_i,hitbco_i), 150, 0, 150)));
        // }
      }
      
    }
  }

  h2_nINTTRawHit_nSaturation = new TH2D("h2_nINTTRawHit_nSaturation", "h2_nINTTRawHit_nSaturation;nINTTRawHits;nSaturationChip", 400,0,40000, 500,0,500);
  h2_Saturation_ChipMap = new TH2D("h2_Saturation_ChipMap", Form("h2_Saturation_ChipMap;FELIX x %d + FELIX_ch;chip_id", FELIXId_factor), 128,0,128, 26,0,26);

  h1_chip_hit_container = new TH1D("","",128,0,128);

  h1_ClusSize_SaturatedChip_normal = new TH1D("h1_ClusSize_SaturatedChip_normal","h1_ClusSize_SaturatedChip_normal;ClusPhiSize (Single chip, normal);Entries", 128,0,128);
  h1_ClusSize_SaturatedChip_halfentry = new TH1D("h1_ClusSize_SaturatedChip_halfentry","h1_ClusSize_SaturatedChip_halfentry;ClusPhiSize (Single chip, Half);Entries", 128,0,128);

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

  if (MBDNS_trigger_require_tag)
  {
    GetLiveTrigger(topNode);

    if (trigger_MBDvtxZ_cm == 10 && live_trigger_map.find(MBDNS_VtxZ10cm_Id) == live_trigger_map.end()){
      eID_count++;
      return Fun4AllReturnCodes::EVENT_OK;
    }
    
    if (trigger_MBDvtxZ_cm == 30 && live_trigger_map.find(MBDNS_VtxZ30cm_Id) == live_trigger_map.end()){
      eID_count++;
      return Fun4AllReturnCodes::EVENT_OK;
    }

  }

  if (inttcont->get_nhits() == 0) {
    std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - no INTT hit found" << std::endl;
    eID_count++;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  for (unsigned int i = 0; i < inttcont->get_nhits(); i++)
  {
    InttRawHit* intthit = inttcont->get_hit(i);

    if (i == 0) {evt_INTT_bco_full = intthit->get_bco();}

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
        evt_inttHits_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco, bco_diff};
      }
    }
    else // note : if we don't want to remove the clone hits
    {
      evt_inttHits_map[Form("%d",i)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco, bco_diff}; // note : only index i to make the key unique
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

  int nSaturationChip = 0;

  for (auto &pair : evt_ChipHit_count_map)
  {
    std::string evt_ChipHit_count_map_key = pair.first;
    std::string bco_removed_key = evt_ChipHit_count_map_key.substr(0,evt_ChipHit_count_map_key.find("_HitBco"));

    int felix_id = std::stoi( bco_removed_key.substr(bco_removed_key.find("_F")+2,bco_removed_key.find("_Fch") - bco_removed_key.find("_F")-2) );
    int felix_ch = std::stoi( bco_removed_key.substr(bco_removed_key.find("_Fch")+4,bco_removed_key.find("_Chip") - bco_removed_key.find("_Fch")-4) );
    int chip_id = std::stoi( bco_removed_key.substr(bco_removed_key.find("_Chip")+5,bco_removed_key.size() - bco_removed_key.find("_Chip")-5) );

    if (h1_nChipHit_map.find(bco_removed_key) == h1_nChipHit_map.end())
    {
      std::cout << "eID: "<< eID_count <<" INTTChipOccupancy::process_event - h1_nChipHit_map key not found, this key is : "<< bco_removed_key << std::endl;
      continue;
    }

    h1_nChipHit_map[bco_removed_key]->Fill(pair.second);

    if (normal_chip_map.find(bco_removed_key) != normal_chip_map.end() && pair.second > normal_threshold){
      nSaturationChip += 1;
      h2_Saturation_ChipMap->Fill(felix_id * FELIXId_factor + felix_ch, chip_id);

      out_INTT_bco_full = evt_INTT_bco_full;
      out_FELIX = felix_id;
      out_FELIX_ch = felix_ch;
      out_chip_id = chip_id;
      out_nHits = pair.second;

      INTTChipOccupancy::chip_clu_info clusters = SemiChipClustering(felix_id, felix_ch, chip_id);
      out_adc_vec = clusters.adc_vec;
      out_size_vec = clusters.size_vec;
      out_edge_l_vec = clusters.edge_l_vec;
      out_edge_r_vec = clusters.edge_r_vec;
      out_pos_vec = clusters.pos_vec;
      out_largest_size = clusters.largest_size;

      for(auto &size : out_size_vec){
        h1_ClusSize_SaturatedChip_normal -> Fill(size);
      }

      SaturationChip_tree_out -> Fill();

    }
    if (halfentry_chip_map.find(bco_removed_key) != halfentry_chip_map.end() && pair.second > halfentry_threshold){
      nSaturationChip += 1;
      h2_Saturation_ChipMap->Fill(felix_id * FELIXId_factor + felix_ch, chip_id);

      out_INTT_bco_full = evt_INTT_bco_full;
      out_FELIX = felix_id;
      out_FELIX_ch = felix_ch;
      out_chip_id = chip_id;
      out_nHits = pair.second;

      INTTChipOccupancy::chip_clu_info clusters = SemiChipClustering(felix_id, felix_ch, chip_id);
      out_adc_vec = clusters.adc_vec;
      out_size_vec = clusters.size_vec;
      out_edge_l_vec = clusters.edge_l_vec;
      out_edge_r_vec = clusters.edge_r_vec;
      out_pos_vec = clusters.pos_vec;
      out_largest_size = clusters.largest_size;

      for(auto &size : out_size_vec){
        h1_ClusSize_SaturatedChip_halfentry -> Fill(size);
      }

      SaturationChip_tree_out -> Fill();
    }
    
  }

  h2_nINTTRawHit_nSaturation->Fill(evt_inttHits_map.size(), nSaturationChip);

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
  
  SaturationChip_tree_out->Write();

  h1_ClusSize_SaturatedChip_normal->Write();
  h1_ClusSize_SaturatedChip_halfentry->Write();

  h2_nINTTRawHit_nSaturation->Write();
  h2_Saturation_ChipMap->Write();

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

void INTTChipOccupancy::GetLiveTrigger(PHCompositeNode *topNode)
{
    p_gl1 = findNode::getClass<Gl1Packetv2>(topNode, m_gl1NodeName); // note : for the selfgen DST, it may be the "GL1RAWHIT"

    if (p_gl1)
    {
        live_trigger_decimal = p_gl1->lValue(0,"LiveVector");
        live_trigger_map = INTTChipOccupancy::prepare_trigger_vec(live_trigger_decimal);

        scaled_trigger_decimal = p_gl1->lValue(0,"ScaledVector");
        scaled_trigger_map = INTTChipOccupancy::prepare_trigger_vec(scaled_trigger_decimal);

    }
    else
    {
        std::cout << "In INTTChipOccupancy::GetLiveTrigger, No GL1RAWHIT node found" << std::endl;
        exit(1);
    }
}

std::map<int,int> INTTChipOccupancy::prepare_trigger_vec(long long trigger_input)
{
    std::bitset<64> trigger_input_bitset(trigger_input);
    std::vector<int> output_vec; output_vec.clear();
    std::map<int,int> output_map; output_map.clear();

    for (unsigned int i=0; i<64; i++)
	{
	    if (trigger_input_bitset.test(i))
	    {
            output_vec.push_back(i);
	    }
	}

  for (int ele : output_vec){
    output_map[ele] = ele;
  }



    return output_map;

}

INTTChipOccupancy::chip_clu_info INTTChipOccupancy::SemiChipClustering(int FELIX_in, int FELIX_ch_in, int chip_in)
{
  h1_chip_hit_container->Reset("ICESM");

  std::map<double,int> channel_id_map; channel_id_map.clear();
  for (auto &pair : evt_inttHits_map){
    inttHitstr hit = pair.second;
    if (hit.hit_server == FELIX_in && hit.hit_felix_ch == FELIX_ch_in && hit.hit_chip == chip_in){
      channel_id_map.insert(std::make_pair(double(hit.hit_channel), hit.hit_adc));
    }
  }

  for (auto &pair : channel_id_map){
    h1_chip_hit_container->Fill(pair.first, pair.second);
  }

  return find_Ngroup(h1_chip_hit_container);
}

INTTChipOccupancy::chip_clu_info INTTChipOccupancy::find_Ngroup(TH1D * hist_in)
{
    double Highest_bin_Content __attribute__((unused)) = hist_in -> GetBinContent(hist_in -> GetMaximumBin());
    double Highest_bin_Center   = hist_in -> GetBinCenter(hist_in -> GetMaximumBin());

    int group_Nbin = 0;
    int peak_group_ID = -9999;
    double group_entry = 0;
    double peak_group_ratio __attribute__((unused));
    std::vector<int> group_Nbin_vec; group_Nbin_vec.clear();
    std::vector<double> group_entry_vec; group_entry_vec.clear();
    std::vector<double> group_widthL_vec; group_widthL_vec.clear();
    std::vector<double> group_widthR_vec; group_widthR_vec.clear();
    std::vector<double> clu_pos_vec; clu_pos_vec.clear();

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        // todo : the background rejection is here : Highest_bin_Content/2. for the time being
        // double bin_content = ( hist_in -> GetBinContent(i+1) <= Highest_bin_Content/2.) ? 0. : ( hist_in -> GetBinContent(i+1) - Highest_bin_Content/2. );

        double bin_content = hist_in -> GetBinContent(i+1);

        if (bin_content != 0){
            
            if (group_Nbin == 0) {
                group_widthL_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            }

            group_Nbin += 1; 
            group_entry += bin_content;
        }
        else if (bin_content == 0 && group_Nbin != 0){
            group_widthR_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            group_Nbin_vec.push_back(group_Nbin);
            group_entry_vec.push_back(group_entry);
            group_Nbin = 0;
            group_entry = 0;
        }
    }
    if (group_Nbin != 0) {
        group_Nbin_vec.push_back(group_Nbin);
        group_entry_vec.push_back(group_entry);
        group_widthR_vec.push_back(hist_in -> GetXaxis()->GetXmax());
    } // note : the last group at the edge

    // note : find the peak group
    for (int i = 0; i < int(group_Nbin_vec.size()); i++){
        if (group_widthL_vec[i] < Highest_bin_Center && Highest_bin_Center < group_widthR_vec[i]){
            peak_group_ID = i;
            break;
        }
    }
    
    // note : On Nov 6, 2024, we no longer need to calculate the ratio of the peak group
    // double denominator = (accumulate( group_entry_vec.begin(), group_entry_vec.end(), 0.0 ));
    // denominator = (denominator <= 0) ? 1. : denominator;
    // peak_group_ratio = group_entry_vec[peak_group_ID] / denominator;
    peak_group_ratio = -999;

    double peak_group_left __attribute__((unused))  = (double(group_Nbin_vec.size()) == 0) ? -999 : group_widthL_vec[peak_group_ID];
    double peak_group_right __attribute__((unused)) = (double(group_Nbin_vec.size()) == 0) ? 999  : group_widthR_vec[peak_group_ID];

    for (int i = 0; i < int(group_Nbin_vec.size()); i++){
        clu_pos_vec.push_back( CoM_cluster_pos(hist_in, group_widthL_vec[i], group_widthR_vec[i]) );
    }

    INTTChipOccupancy::chip_clu_info out_str;

    out_str.adc_vec = group_entry_vec;
    out_str.size_vec = group_Nbin_vec;
    out_str.edge_l_vec = group_widthL_vec;
    out_str.edge_r_vec = group_widthR_vec;
    out_str.pos_vec = clu_pos_vec;
    out_str.largest_size = *(std::max_element(group_Nbin_vec.begin(), group_Nbin_vec.end()));

    // for (int i = 0; i < group_Nbin_vec.size(); i++)
    // {
    //     std::cout<<" "<<std::endl;
    //     std::cout<<"group width : "<<group_Nbin_vec[i]<<std::endl;
    //     std::cout<<"group adc : "<<group_entry_vec[i]<<std::endl;
    //     std::cout<<group_widthL_vec[i]<<" "<<group_widthR_vec[i]<<std::endl;
    // }

    // cout<<" "<<endl;
    // cout<<"N group : "<<group_Nbin_vec.size()<<endl;
    // cout<<"Peak group ID : "<<peak_group_ID<<endl;
    // cout<<"peak group width : "<<group_widthL_vec[peak_group_ID]<<" "<<group_widthR_vec[peak_group_ID]<<endl;
    // cout<<"ratio : "<<peak_group_ratio<<endl;
    
    // note : {N_group, ratio (if two), peak widthL, peak widthR}
    return out_str;
}

double INTTChipOccupancy::CoM_cluster_pos(TH1D * hist_in, double edge_l, double edge_r)
{
    double sum = 0;
    double sum_weight = 0;

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        double bin_content = hist_in -> GetBinContent(i+1);
        double bin_center = hist_in -> GetBinCenter(i+1);

        if (edge_l < bin_center && bin_center < edge_r){
            sum += bin_content * bin_center;
            sum_weight += bin_content;
        }
    }

    return (sum_weight == 0) ? -999 : (sum / sum_weight) - hist_in -> GetBinWidth(1)/2.;
}