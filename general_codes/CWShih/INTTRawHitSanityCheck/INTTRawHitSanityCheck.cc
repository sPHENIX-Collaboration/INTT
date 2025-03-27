//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in INTTRawHitSanityCheck.h.
//
// INTTRawHitSanityCheck(const std::string &name = "INTTRawHitSanityCheck")
// everything is keyed to INTTRawHitSanityCheck, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// INTTRawHitSanityCheck::~INTTRawHitSanityCheck()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int INTTRawHitSanityCheck::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int INTTRawHitSanityCheck::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int INTTRawHitSanityCheck::process_event(PHCompositeNode *topNode)
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
// int INTTRawHitSanityCheck::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int INTTRawHitSanityCheck::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int INTTRawHitSanityCheck::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int INTTRawHitSanityCheck::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void INTTRawHitSanityCheck::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "INTTRawHitSanityCheck.h"

#include <cmath>
#include <bitset>
#include <numeric>

//____________________________________________________________________________..
INTTRawHitSanityCheck::INTTRawHitSanityCheck(
  const std::string &name,
  const int process_id_in,
  const int runnumber_in,
  const std::string output_directory_in,
  const std::string hotchannel_full_directory_in,

  const int triggered_BcoDiff_in,
  const bool apply_hotchannel_in,
  const bool clone_hit_remove_BCO_in,
  const bool hitQA_in,
  const bool check_clone_hit_in,

  const bool HaveGL1_in,
  const bool Get_InttRawHit_ntuple_in,

  const std::vector<int> adc_conversion_vec_in
):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in),
  hotchannel_full_directory(hotchannel_full_directory_in),
  triggered_BcoDiff(triggered_BcoDiff_in),
  apply_hotchannel_tag(apply_hotchannel_in),
  clone_hit_remove_BCO_tag(clone_hit_remove_BCO_in),
  hitQA_tag(hitQA_in),
  check_clone_hit_tag(check_clone_hit_in),
  HaveGL1_tag(HaveGL1_in),
  Get_InttRawHit_ntuple(Get_InttRawHit_ntuple_in),
  adc_conversion_vec(adc_conversion_vec_in),
  inttcont(nullptr),
  gl1(nullptr)
  
{
  std::cout << "INTTRawHitSanityCheck::INTTRawHitSanityCheck(const std::string &name) Calling ctor" << std::endl;

  // if (clone_hit_remove_BCO_tag) {
  //   std::cout<<"INTTRawHitSanityCheck::INTTRawHitSanityCheck - clone_hit_remove and clone_hit_remove_BCO cannot be true at the same time"<<std::endl;

  //   gSystem->Exit(1);
  //   exit(1);
  // }

  output_file_name = "BcoDiffNtuple_"+std::to_string(runnumber);
  output_file_name += (clone_hit_remove_BCO_tag) ? "_clonehitremoveBCO" : "";
  output_file_name += (apply_hotchannel_tag) ? "_hotchannelremove" : "";
  output_file_name += (hitQA_tag) ? "_hitQA" : "";
  output_file_name += (check_clone_hit_tag) ? "_checkclonehit" : "";

  std::string job_index = std::to_string( process_id );
  int total_index_len = 5;
  job_index.insert(0, total_index_len - job_index.size(), '0');
  output_file_name += "_"+job_index;

  output_file_name += ".root";

  eID_count = 0;

  m_feemap.LoadFromFile(intt_fee_map_directory);

  h1_chip_hit_container = new TH1D("","",128,0,128);
}

//____________________________________________________________________________..
INTTRawHitSanityCheck::~INTTRawHitSanityCheck()
{
  std::cout << "INTTRawHitSanityCheck::~INTTRawHitSanityCheck() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int INTTRawHitSanityCheck::Init(PHCompositeNode *topNode)
{
  std::cout << "INTTRawHitSanityCheck::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  out_MBDNS_tight_inclusive = -999;
  out_MBDNS_tight_vtx10cm = -999;
  out_MBDNS_tight_vtx30cm = -999;

  evt_felix_BcoDiffHist_vec.clear();
  all_felix_BcoDiffHist_vec.clear();

  for (int i = 0; i < 8; i++)
  {
    evt_felix_BcoDiffHist_vec.push_back(new TH1D(Form("evt_felix_BcoDiffHist_%d",i),Form("evt_felix_BcoDiffHist_%d;BCO_Diff;Entries (/1)",i),128,0,128));
    all_felix_BcoDiffHist_vec.push_back(new TH1D(Form("all_felix_BcoDiffHist_%d",i),Form("all_felix_BcoDiffHist_%d;BCO_Diff;Entries (/1)",i),128,0,128));
  }

  memset(evt_clone_hit_count, 0, sizeof(evt_clone_hit_count));
  evt_inttHits_map.clear();
  evt_inttHits_BCO_map.clear();

  out_spikeM1_location.clear();
  out_spikeM1_height.clear();
  out_spikeM2_location.clear();
  out_spikeM2_height.clear();
  out_spikeM3_location.clear();
  out_spikeM3_height.clear();

  out_spikeP1_location.clear();
  out_spikeP1_height.clear();
  out_spikeP2_location.clear();
  out_spikeP2_height.clear();
  out_spikeP3_location.clear();
  out_spikeP3_height.clear();

  out_HeightFirst_location.clear();
  out_HeightFirst_height.clear();
  out_HeightSecond_location.clear();
  out_HeightSecond_height.clear();
  out_HeightThird_location.clear();
  out_HeightThird_height.clear();
  out_HeightFourth_location.clear();
  out_HeightFourth_height.clear();


  // for (int i = 0; i < 8; i++) {
  //   for (int j = 0; j < 14; j++) {
  //     evt_ladder_clone_hit[i][j].clear();
  //   }
  // }

  hot_channel_map.clear();

  // note : load hot channel map
  if (apply_hotchannel_tag){PrepareHotChannel();}

  // note : prepare the structure of the output root file
  PrepareOutFile();

  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTRawHitSanityCheck::PrepareOutFile()
{
  
  file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_file_name.c_str()),"recreate");
  tree_out = new TTree("tree","tree");

  tree_out -> Branch("eID", &eID_count);

  tree_out -> Branch("MBDNS_tight_inclusive", &out_MBDNS_tight_inclusive); 
  tree_out -> Branch("MBDNS_tight_vtx10cm", &out_MBDNS_tight_vtx10cm); 
  tree_out -> Branch("MBDNS_tight_vtx30cm", &out_MBDNS_tight_vtx30cm); 

  tree_out -> Branch("GL1_bco", &out_GL1_bco);
  tree_out -> Branch("INTT_bcofull", &out_INTT_bcofull);
  tree_out -> Branch("total_nHit", &out_total_nHit); 
  tree_out -> Branch("total_nHit_post", &out_total_nHit_post);
  
  // tree_out -> Branch("spikeM1_location", &out_spikeM1_location); 
  // tree_out -> Branch("spikeM1_height", &out_spikeM1_height); 
  // tree_out -> Branch("spikeM2_location", &out_spikeM2_location); 
  // tree_out -> Branch("spikeM2_height", &out_spikeM2_height); 
  // tree_out -> Branch("spikeM3_location", &out_spikeM3_location); 
  // tree_out -> Branch("spikeM3_height", &out_spikeM3_height); 

  // tree_out -> Branch("spikeP1_location", &out_spikeP1_location); 
  // tree_out -> Branch("spikeP1_height", &out_spikeP1_height); 
  // tree_out -> Branch("spikeP2_location", &out_spikeP2_location); 
  // tree_out -> Branch("spikeP2_height", &out_spikeP2_height); 
  // tree_out -> Branch("spikeP3_location", &out_spikeP3_location); 
  // tree_out -> Branch("spikeP3_height", &out_spikeP3_height); 

  // tree_out -> Branch("HeightFirst_location", &out_HeightFirst_location); 
  // tree_out -> Branch("HeightFirst_height", &out_HeightFirst_height);
  // tree_out -> Branch("HeightSecond_location", &out_HeightSecond_location); 
  // tree_out -> Branch("HeightSecond_height", &out_HeightSecond_height); 
  // tree_out -> Branch("HeightThird_location", &out_HeightThird_location); 
  // tree_out -> Branch("HeightThird_height", &out_HeightThird_height); 
  // tree_out -> Branch("HeightFourth_location", &out_HeightFourth_location); 
  // tree_out -> Branch("HeightFourth_height", &out_HeightFourth_height); 

  // tree_out -> Branch("felix0_bcodiff", &out_felix0_bcodiff); 
  // tree_out -> Branch("felix1_bcodiff", &out_felix1_bcodiff); 
  // tree_out -> Branch("felix2_bcodiff", &out_felix2_bcodiff); 
  // tree_out -> Branch("felix3_bcodiff", &out_felix3_bcodiff); 
  // tree_out -> Branch("felix4_bcodiff", &out_felix4_bcodiff); 
  // tree_out -> Branch("felix5_bcodiff", &out_felix5_bcodiff); 
  // tree_out -> Branch("felix6_bcodiff", &out_felix6_bcodiff); 
  // tree_out -> Branch("felix7_bcodiff", &out_felix7_bcodiff); 

  if (check_clone_hit_tag){
    tree_out -> Branch("totalHit_forCloneStudy", &out_totalHit_forCloneStudy);
    tree_out -> Branch("total_nCloneHit",  &out_total_nCloneHit);
    tree_out -> Branch("felix0_NCloneHit", &out_felix0_NCloneHit);
    tree_out -> Branch("felix1_NCloneHit", &out_felix1_NCloneHit);
    tree_out -> Branch("felix2_NCloneHit", &out_felix2_NCloneHit);
    tree_out -> Branch("felix3_NCloneHit", &out_felix3_NCloneHit);
    tree_out -> Branch("felix4_NCloneHit", &out_felix4_NCloneHit);
    tree_out -> Branch("felix5_NCloneHit", &out_felix5_NCloneHit);
    tree_out -> Branch("felix6_NCloneHit", &out_felix6_NCloneHit);
    tree_out -> Branch("felix7_NCloneHit", &out_felix7_NCloneHit);
  }

  if (Get_InttRawHit_ntuple)
  {
    tree_out_RawHit = new TTree("EventTree","EventTree");
    tree_out_RawHit->Branch("NInttRawHits", &NInttRawHits_);
    tree_out_RawHit->Branch("InttRawHit_bco", &InttRawHit_bco_);
    tree_out_RawHit->Branch("InttRawHit_packetid", &InttRawHit_packetid_);
    tree_out_RawHit->Branch("InttRawHit_word", &InttRawHit_word_);
    tree_out_RawHit->Branch("InttRawHit_fee", &InttRawHit_fee_);
    tree_out_RawHit->Branch("InttRawHit_channel_id", &InttRawHit_channel_id_);
    tree_out_RawHit->Branch("InttRawHit_chip_id", &InttRawHit_chip_id_);
    tree_out_RawHit->Branch("InttRawHit_adc", &InttRawHit_adc_);
    tree_out_RawHit->Branch("InttRawHit_FPHX_BCO", &InttRawHit_FPHX_BCO_);
    tree_out_RawHit->Branch("InttRawHit_full_FPHX", &InttRawHit_full_FPHX_);
    tree_out_RawHit->Branch("InttRawHit_full_ROC", &InttRawHit_full_ROC_);
    tree_out_RawHit->Branch("InttRawHit_amplitude", &InttRawHit_amplitude_);

    tree_out_RawHit -> Branch("MBDNS_tight_inclusive", &out_MBDNS_tight_inclusive); 
    tree_out_RawHit -> Branch("MBDNS_tight_vtx10cm", &out_MBDNS_tight_vtx10cm); 
    tree_out_RawHit -> Branch("MBDNS_tight_vtx30cm", &out_MBDNS_tight_vtx30cm);
  }


  h2D_NInttHitInner_NInttHitOuter = new TH2D("h2D_NInttHitInner_NInttHitOuter","h2D_NInttHitInner_NInttHitOuter;NHits (inner);NHits (outer)", 400, 0, 10000, 400, 0, 10000);

  h2D_NInttHit_TrigSpikeSize = new TH2D("h2D_NInttHit_TrigSpikeSize","h2D_NInttHit_TrigSpikeSize;NInttHits (Trig Bin-1 + TrigBin + TrigBin+1);Trigger Bin content ratio", 400, 0, 40000, 200, 0, 1.2);

  h1D_SemiClusPhiSize = new TH1D("h1D_SemiClusPhiSize","h1D_SemiClusPhiSize;h1D_SemiClusPhiSize;ClusPhiSize (single cell);Entries", 128,0,128);

  h1D_nChipHit_map.clear();

  for (int felix_i = 0; felix_i < nFELIX; felix_i++){
    for (int fch_i = 0; fch_i < nFELIXch; fch_i++){
      for (int chip_i = 0; chip_i < nChip; chip_i++){
        h1D_nChipHit_map.insert(std::make_pair(Form("nChipHit_F%d_Fch%d_Chip%d",felix_i,fch_i,chip_i), new TH1D(Form("nChipHit_F%d_Fch%d_Chip%d",felix_i,fch_i,chip_i), Form("nChipHit_F%d_Fch%d_Chip%d;nHits (per chip and per HitBco);Entries",felix_i,fch_i,chip_i), 150, 0, 150)));
        // for (int hitbco_i = 0; hitbco_i < nHitBco; hitbco_i++){
        //   h1D_nChipHit_map.insert(std::make_pair(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",felix_i,fch_i,chip_i,hitbco_i), new TH1D(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",felix_i,fch_i,chip_i,hitbco_i), Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d;nHits (per chip and HitBco);Entries",felix_i,fch_i,chip_i,hitbco_i), 150, 0, 150)));
        // }
      }
      
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTRawHitSanityCheck::PrepareHotChannel()
{
  hot_file_in = TFile::Open(hotchannel_full_directory.c_str());

  if (!hot_file_in)
  {
    std::cout << "INTTRawHitSanityCheck::PrepareHotChannel - hot channel file not found" << std::endl;
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

//____________________________________________________________________________..
int INTTRawHitSanityCheck::InitRun(PHCompositeNode *topNode)
{
  std::cout << "INTTRawHitSanityCheck::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<int> INTTRawHitSanityCheck::prepare_trigger_vec(long long trigger_input)
{
  std::bitset<64> trigger_input_bitset(trigger_input);
  std::vector<int> output_vec; output_vec.clear();

  for (unsigned int i=0; i<64; i++)
	{
	  if (trigger_input_bitset.test(i))
	  {
        output_vec.push_back(i);
	  }
	}

  return output_vec;

}

std::map<int,int> INTTRawHitSanityCheck::prepare_trigger_map(std::vector<int> trigger_vec_input)
{
    std::map<int,int> output_map; output_map.clear();

    for (int trig : trigger_vec_input){
      output_map[trig] = trig;
    }

    return output_map;
}

int INTTRawHitSanityCheck::PrepareGL1(PHCompositeNode *topNode)
{
  gl1 = nullptr;
  out_GL1_bco = -999;
  live_trigger_decimal = -999;
  out_MBDNS_tight_inclusive = -999;
  out_MBDNS_tight_vtx10cm = -999;
  out_MBDNS_tight_vtx30cm = -999;
  
  gl1 = findNode::getClass<Gl1Packetv2>(topNode, "GL1RAWHIT");

  if (!gl1)
  {
    std::cout << "INTTRawHitSanityCheck::process_event - GL1RAWHIT node not found" << std::endl;
    
    gSystem->Exit(1);
    exit(1);
  }

  // gl1rawhitbco = gl1->lValue(0, "BCO");
  out_GL1_bco = gl1->getBCO();
  live_trigger_decimal = gl1->lValue(0,"LiveVector");

  if (live_trigger_decimal < 0)
  {
    std::cout << "INTTRawHitSanityCheck::PrepareGL1 - live trigger is negative, "<< live_trigger_decimal << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  live_trigger_map = prepare_trigger_map(prepare_trigger_vec(live_trigger_decimal));

  out_MBDNS_tight_inclusive = (live_trigger_map.find(MBDNS_tight_inclusive) != live_trigger_map.end()) ? 1 : 0;
  out_MBDNS_tight_vtx10cm = (live_trigger_map.find(MBDNS_tight_vtx10cm) != live_trigger_map.end()) ? 1 : 0;
  out_MBDNS_tight_vtx30cm = (live_trigger_map.find(MBDNS_tight_vtx30cm) != live_trigger_map.end()) ? 1 : 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<int> INTTRawHitSanityCheck::GetHistBinEntry(TH1D * hist)
{
  std::vector<int> output_vec; output_vec.clear();

  for (int i = 1; i <= hist->GetNbinsX(); i++)
  {
    output_vec.push_back(hist->GetBinContent(i));
  }

  return output_vec;
}

int INTTRawHitSanityCheck::PrepareINTT(PHCompositeNode *topNode)
{

  out_INTT_bcofull = -999;
  out_total_nHit = -999;
  out_total_nHit_post = -999;

  // note : <bco_diff, count>
  std::map<int,int> NTrigHit_inner_map; NTrigHit_inner_map.clear();
  std::map<int,int> NTrigHit_outer_map; NTrigHit_outer_map.clear();
  for (int i = 0; i < 128; i++)
  {
    NTrigHit_inner_map[i] = 0;
    NTrigHit_outer_map[i] = 0;
  }

  for (int DST_felix_i = 0; DST_felix_i < 8; DST_felix_i++)// note : FELIX server
  {
    inttcont = nullptr;
    inttcont = findNode::getClass<InttRawHitContainer>(topNode, Form("%s_%d",m_InttRawNodeName.c_str(),DST_felix_i));
    // inttcont = findNode::getClass<InttRawHitContainer>(topNode, Form("%s",m_InttRawNodeName.c_str()));
    if (!inttcont)
    {
      std::cout << PHWHERE << std::endl;
      std::cout << "InttCombinedRawDataDecoder::process_event(PHCompositeNode* topNode)" << std::endl;
      std::cout << "Could not get \"" << m_InttRawNodeName << "\" from Node Tree" << std::endl;
      std::cout << "Exiting" << std::endl;

      gSystem->Exit(1);
      exit(1);
    }

    out_total_nHit = inttcont->get_nhits();
    if (out_total_nHit == 0) {
      out_INTT_bcofull = -999;
      std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - no INTT hit found" << std::endl;
      return Fun4AllReturnCodes::EVENT_OK;
    }

    for (unsigned int i = 0; i < inttcont->get_nhits(); i++)
    {
      InttRawHit* intthit = inttcont->get_hit(i);

      if (i == 0) {out_INTT_bcofull = intthit->get_bco();}

      // amp = intthit->get_amplitude();
  
      int server = intthit->get_packetid() - 3001; // note : the felix server ID
      int felix_ch = intthit->get_fee(); // note : the felix channel ID 0 - 13
      int chip = (intthit->get_chip_id() - 1) % 26; // note : chip ID 0 - 25
      int channel = intthit->get_channel_id();
      int adc = intthit->get_adc();
      int bco = intthit->get_FPHX_BCO();

      if (hitQA_tag && (server < 0 || server > 7)) // note : server ID 0 - 7
      {
        std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - server out of range, this server is : "<< server << std::endl;
        continue;
      }

      if (hitQA_tag && (felix_ch < 0 || felix_ch > 13)) // note : felix channel ID 0 - 13
      {
        std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - felix channel out of range, this channel is : "<< felix_ch << std::endl;
        continue;
      }

      if (hitQA_tag && (chip < 0 || chip > 25)) // note : chip ID 0 - 25
      {
        std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - chip out of range, this chip is : "<< chip << std::endl;
        continue;
      }

      if (hitQA_tag && (channel < 0 || channel > 127)) // note : channel ID 0 - 127
      {
        std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - channel out of range, this channel is : "<< channel << std::endl;
        continue;
      }

      if (hitQA_tag && (adc < 0 || adc > 7)) // note : adc ID 0 - 7
      {
        std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - adc out of range, this adc is : "<< adc << std::endl;
        continue;
      }
      
      if (hitQA_tag && (bco < 0 || bco > 127)) // note : bco ID 0 - 127
      {
        std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - bco out of range, this bco is : "<< bco << std::endl;
        continue;
      }

      

      if (apply_hotchannel_tag) // note : if we want to remove the hot channels
      {
        if (hot_channel_map.find(Form("%d_%d_%d_%d",server,felix_ch,chip,channel)) != hot_channel_map.end())
        {
          continue;
        }
      }

      if (clone_hit_remove_BCO_tag)
      {
        if (evt_inttHits_map.find(Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)) == evt_inttHits_map.end()) // note : if it's not found, we just add it
        {
          evt_inttHits_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco};
        }
      }
      else // note : if we don't want to remove the clone hits
      {
        evt_inttHits_map[Form("%d",i)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco}; // note : only index i to make the key unique
      }

      out_totalHit_forCloneStudy += 1;

      if (check_clone_hit_tag)
      {
        if (evt_inttHits_BCO_map.find(Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)) == evt_inttHits_BCO_map.end()) // note : if it's not found, we just add it
        {
          evt_inttHits_BCO_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc_conversion_vec[adc],bco};
        }
        else // note : here leaves the window for the clone hit study for the future, if needed.
        {
          evt_clone_hit_count[server][felix_ch][chip][channel] += 1; // note : 1 means, there are two hits in the same position 
        }
      }

    } // note : end of INTT raw hit loop

  } // note: done with all the FELIX servers

  out_total_nHit_post = evt_inttHits_map.size();

  for (const auto& pair : evt_inttHits_map) // note : total hits post HitQA, BadChannelMask, CloneHitRemoval, if selected
  {
    int server = pair.second.hit_server;
    // int felix_ch = pair.second.hit_felix_ch;
    // int chip = pair.second.hit_chip;
    // int channel = pair.second.hit_channel;
    // int adc = pair.second.hit_adc;
    int bco = pair.second.hit_bco;

    inttHitstr hit = pair.second; 

    int bco_diff = (bco - (out_INTT_bcofull & 0x7fU) + 128) % 128;

    if (bco_diff < 0 || bco_diff > 127) // note : bco_diff ID 0 - 127
    {
      std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::PrepareINTT - bco_diff out of range, this bco_diff is : "<< bco_diff<<" hitbco : "<<bco<<" bco_full : "<<out_INTT_bcofull<< std::endl;
      
      gSystem->Exit(1);
      exit(1);
    }

    evt_felix_BcoDiffHist_vec[server] -> Fill(bco_diff); // todo: right now the unit is down to server, not half ladder

    if (HaveGL1_tag && out_MBDNS_tight_vtx30cm == 1){
      all_felix_BcoDiffHist_vec[server] -> Fill(bco_diff);
    }
    else if (!HaveGL1_tag){
      all_felix_BcoDiffHist_vec[server] -> Fill(bco_diff);
    }

    // Division:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if (evt_ChipHit_count_map.find(Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",hit.hit_server,hit.hit_felix_ch,hit.hit_chip,hit.hit_bco)) == evt_ChipHit_count_map.end()) // note : if it's not found, we just add it
    {
      evt_ChipHit_count_map[Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",hit.hit_server,hit.hit_felix_ch,hit.hit_chip,hit.hit_bco)] = 1;
    }
    else
    {
      evt_ChipHit_count_map[Form("nChipHit_F%d_Fch%d_Chip%d_HitBco%d",hit.hit_server,hit.hit_felix_ch,hit.hit_chip,hit.hit_bco)] += 1;
    }    


    // Division:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    InttMap::RawData_s rawdata;
    rawdata.pid = pair.second.hit_server + 3001;
    rawdata.fee = pair.second.hit_felix_ch;
    rawdata.chp = pair.second.hit_chip;
    rawdata.chn = pair.second.hit_channel;

    InttMap::Offline_s offlinedata = m_feemap.ToOffline(rawdata);
    if (offlinedata.layer != 3 && offlinedata.layer != 4 && offlinedata.layer != 5 && offlinedata.layer != 6){
        std::cout << "pid: "<< rawdata.pid << ", fee: " << rawdata.fee << ", chp: " << rawdata.chp << ", chn: " << rawdata.chn << std::endl;
        std::cout << "layer: " << offlinedata.layer << ", ladder_phi: " << offlinedata.ladder_phi << ", ladder_z: " << offlinedata.ladder_z << ", strip_z: " << offlinedata.strip_z << ", strip_phi: " << offlinedata.strip_phi << std::endl;
        std::cout<<std::endl;
        continue;
    }

    if (offlinedata.layer == 3 || offlinedata.layer == 4)
    {
      NTrigHit_inner_map[bco_diff] += 1;
    }
    else if (offlinedata.layer == 5 || offlinedata.layer == 6)
    {
      NTrigHit_outer_map[bco_diff] += 1;
    }

  } // note : end of the hit loop

  // Division:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  if (triggered_BcoDiff != -1){ // note : triggered

    int total_inner_Nhits = NTrigHit_inner_map[(triggered_BcoDiff - 1 + 128)%128] + NTrigHit_inner_map[(triggered_BcoDiff + 128)%128] + NTrigHit_inner_map[(triggered_BcoDiff + 1 + 128)%128];
    int total_outer_Nhits = NTrigHit_outer_map[(triggered_BcoDiff - 1 + 128)%128] + NTrigHit_outer_map[(triggered_BcoDiff + 128)%128] + NTrigHit_outer_map[(triggered_BcoDiff + 1 + 128)%128];
    int total_trigger_Nhits = NTrigHit_inner_map[(triggered_BcoDiff + 128)%128] + NTrigHit_outer_map[(triggered_BcoDiff + 128)%128];

    if ((HaveGL1_tag && out_MBDNS_tight_vtx10cm == 1) || !HaveGL1_tag) {
      h2D_NInttHitInner_NInttHitOuter -> Fill( // note : bco_filter cut, assuming the trigger crossings are identical for all the half-ladders
        total_inner_Nhits,
        total_outer_Nhits
      );

      h2D_NInttHit_TrigSpikeSize -> Fill(
        total_inner_Nhits + total_outer_Nhits,
        double(total_trigger_Nhits) / double(total_inner_Nhits + total_outer_Nhits)
      );
    }
  }
  else { // note : streaming mode 
    for (int i = 0; i < 128; i++)
    {
      if (NTrigHit_inner_map[i] + NTrigHit_outer_map[i] > 0)
      {
        h2D_NInttHitInner_NInttHitOuter -> Fill(
          NTrigHit_inner_map[i],
          NTrigHit_outer_map[i]
        );
      }
    } 
  }
  // Division:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  for (auto &pair : evt_ChipHit_count_map)
  {
    std::string evt_ChipHit_count_map_key = pair.first;
    std::string bco_removed_key = evt_ChipHit_count_map_key.substr(0,evt_ChipHit_count_map_key.find("_HitBco"));

    int felix_id = std::stoi( bco_removed_key.substr(bco_removed_key.find("_F")+2,bco_removed_key.find("_Fch") - bco_removed_key.find("_F")-2) );
    int felix_ch = std::stoi( bco_removed_key.substr(bco_removed_key.find("_Fch")+4,bco_removed_key.find("_Chip") - bco_removed_key.find("_Fch")-4) );
    int chip_id = std::stoi( bco_removed_key.substr(bco_removed_key.find("_Chip")+5,bco_removed_key.size() - bco_removed_key.find("_Chip")-5) );

    if (h1D_nChipHit_map.find(bco_removed_key) == h1D_nChipHit_map.end())
    {
      std::cout << "eID: "<< eID_count <<" INTTRawHitSanityCheck::process_event - h1D_nChipHit_map key not found, this key is : "<< bco_removed_key << std::endl;
      continue;
    }

    h1D_nChipHit_map[bco_removed_key]->Fill(pair.second);

    
    INTTRawHitSanityCheck::chip_clu_info clusters = SemiChipClustering(felix_id, felix_ch, chip_id);
    for (auto &size : clusters.size_vec){
      h1D_SemiClusPhiSize -> Fill(size);
    }
    // out_adc_vec = clusters.adc_vec;
    // out_size_vec = clusters.size_vec;
    // out_edge_l_vec = clusters.edge_l_vec;
    // out_edge_r_vec = clusters.edge_r_vec;
    // out_pos_vec = clusters.pos_vec;
    // out_largest_size = clusters.largest_size;

  }

  // Division:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // out_felix0_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[0]); // todo: right now the unit is down to server, not half ladder
  // out_felix1_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[1]);
  // out_felix2_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[2]);
  // out_felix3_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[3]);
  // out_felix4_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[4]);
  // out_felix5_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[5]);
  // out_felix6_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[6]);
  // out_felix7_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[7]);

  // for (int i = 0; i < 8; i++)
  // {
  //   PrepareHistInfo(evt_felix_BcoDiffHist_vec[i]);
  // }

  if (check_clone_hit_tag)
  {
    PrepareCloneHitInfo();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTRawHitSanityCheck::PrepareHistInfo(TH1D * hist)
{
  if (hist -> GetEntries() == 0) {

    out_HeightFirst_location.push_back(-999);
    out_HeightFirst_height.push_back(-999);
    out_HeightSecond_location.push_back(-999);
    out_HeightSecond_height.push_back(-999);
    out_HeightThird_location.push_back(-999);
    out_HeightThird_height.push_back(-999);
    out_HeightFourth_location.push_back(-999);
    out_HeightFourth_height.push_back(-999);


    out_spikeM1_location.push_back(-999);
    out_spikeM1_height.push_back(-999);
    out_spikeM2_location.push_back(-999);
    out_spikeM2_height.push_back(-999);
    out_spikeM3_location.push_back(-999);
    out_spikeM3_height.push_back(-999);

    out_spikeP1_location.push_back(-999);
    out_spikeP1_height.push_back(-999);
    out_spikeP2_location.push_back(-999);
    out_spikeP2_height.push_back(-999);
    out_spikeP3_location.push_back(-999);
    out_spikeP3_height.push_back(-999);

    return Fun4AllReturnCodes::EVENT_OK;
  }

  std::vector<int> entry_vec = GetHistBinEntry(hist);

  int Nele = entry_vec.size();
  int sort_entry_index[Nele];
  TMath::Sort(Nele, &entry_vec[0], sort_entry_index); // note : from big to small
  
  out_HeightFirst_location.push_back(sort_entry_index[0]); // note : 0 - 127
  out_HeightFirst_height.push_back(entry_vec[sort_entry_index[0]]);
  out_HeightSecond_location.push_back(sort_entry_index[1]); // note : 0 - 127
  out_HeightSecond_height.push_back(entry_vec[sort_entry_index[1]]);
  out_HeightThird_location.push_back(sort_entry_index[2]); // note : 0 - 127
  out_HeightThird_height.push_back(entry_vec[sort_entry_index[2]]);
  out_HeightFourth_location.push_back(sort_entry_index[3]); // note : 0 - 127
  out_HeightFourth_height.push_back(entry_vec[sort_entry_index[3]]);


  out_spikeM1_location.push_back( (sort_entry_index[0] - 1 < 0) ? -999 : sort_entry_index[0] - 1 ); // note : 0 - 127
  out_spikeM1_height.push_back( (sort_entry_index[0] - 1 < 0) ? -999 : entry_vec[sort_entry_index[0] - 1] );
  out_spikeM2_location.push_back( (sort_entry_index[0] - 2 < 0) ? -999 : sort_entry_index[0] - 2 ); // note : 0 - 127
  out_spikeM2_height.push_back( (sort_entry_index[0] - 2 < 0) ? -999 : entry_vec[sort_entry_index[0] - 2] );
  out_spikeM3_location.push_back( (sort_entry_index[0] - 3 < 0) ? -999 : sort_entry_index[0] - 3 ); // note : 0 - 127
  out_spikeM3_height.push_back( (sort_entry_index[0] - 3 < 0) ? -999 : entry_vec[sort_entry_index[0] - 3] );

  out_spikeP1_location.push_back( (sort_entry_index[0] + 1 > 127) ? -999 : sort_entry_index[0] + 1 ); // note : 0 - 127
  out_spikeP1_height.push_back( (sort_entry_index[0] + 1 > 127) ? -999 : entry_vec[sort_entry_index[0] + 1] );
  out_spikeP2_location.push_back( (sort_entry_index[0] + 2 > 127) ? -999 : sort_entry_index[0] + 2 ); // note : 0 - 127
  out_spikeP2_height.push_back( (sort_entry_index[0] + 2 > 127) ? -999 : entry_vec[sort_entry_index[0] + 2] );
  out_spikeP3_location.push_back( (sort_entry_index[0] + 3 > 127) ? -999 : sort_entry_index[0] + 3 ); // note : 0 - 127
  out_spikeP3_height.push_back( (sort_entry_index[0] + 3 > 127) ? -999 : entry_vec[sort_entry_index[0] + 3] );

  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTRawHitSanityCheck::PrepareCloneHitInfo()
{
  out_total_nCloneHit = 0;
  out_felix0_NCloneHit = std::vector<int>(nFELIXch,0);
  out_felix1_NCloneHit = std::vector<int>(nFELIXch,0);
  out_felix2_NCloneHit = std::vector<int>(nFELIXch,0);
  out_felix3_NCloneHit = std::vector<int>(nFELIXch,0);
  out_felix4_NCloneHit = std::vector<int>(nFELIXch,0);
  out_felix5_NCloneHit = std::vector<int>(nFELIXch,0);
  out_felix6_NCloneHit = std::vector<int>(nFELIXch,0);
  out_felix7_NCloneHit = std::vector<int>(nFELIXch,0);

  for (int i = 0; i < nFELIX; i++)
  {
    for (int j = 0; j < nFELIXch; j++)
    {
      for (int k = 0; k < nChip; k++)
      {
        for (int l = 0; l < nChannel; l++)
        {
          out_total_nCloneHit += evt_clone_hit_count[i][j][k][l];

          if (i == 0) {out_felix0_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
          if (i == 1) {out_felix1_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
          if (i == 2) {out_felix2_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
          if (i == 3) {out_felix3_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
          if (i == 4) {out_felix4_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
          if (i == 5) {out_felix5_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
          if (i == 6) {out_felix6_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
          if (i == 7) {out_felix7_NCloneHit[j] += evt_clone_hit_count[i][j][k][l];}
        }
      }
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;

}

//____________________________________________________________________________..
int INTTRawHitSanityCheck::process_event(PHCompositeNode *topNode)
{
  // std::cout << "INTTRawHitSanityCheck::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  out_MBDNS_tight_inclusive = -999;
  out_MBDNS_tight_vtx10cm = -999;
  out_MBDNS_tight_vtx30cm = -999;

  InttRawHit_bco_.clear();
  InttRawHit_packetid_.clear();
  InttRawHit_word_.clear();
  InttRawHit_fee_.clear();
  InttRawHit_channel_id_.clear();
  InttRawHit_chip_id_.clear();
  InttRawHit_adc_.clear();
  InttRawHit_FPHX_BCO_.clear();
  InttRawHit_full_FPHX_.clear();
  InttRawHit_full_ROC_.clear();
  InttRawHit_amplitude_.clear();

  for (auto& hist : evt_felix_BcoDiffHist_vec) {hist -> Reset("ICESM");}
  memset(evt_clone_hit_count, 0, sizeof(evt_clone_hit_count));
  evt_inttHits_map.clear();
  evt_ChipHit_count_map.clear();
  evt_inttHits_BCO_map.clear(); // note : for clone hit

  out_spikeM1_location.clear();
  out_spikeM1_height.clear();
  out_spikeM2_location.clear();
  out_spikeM2_height.clear();
  out_spikeM3_location.clear();
  out_spikeM3_height.clear();

  out_spikeP1_location.clear();
  out_spikeP1_height.clear();
  out_spikeP2_location.clear();
  out_spikeP2_height.clear();
  out_spikeP3_location.clear();
  out_spikeP3_height.clear();

  out_HeightFirst_location.clear();
  out_HeightFirst_height.clear();
  out_HeightSecond_location.clear();
  out_HeightSecond_height.clear();
  out_HeightThird_location.clear();
  out_HeightThird_height.clear();
  out_HeightFourth_location.clear();
  out_HeightFourth_height.clear();

  out_totalHit_forCloneStudy = 0;

  if (evt_felix_BcoDiffHist_vec[0]->GetEntries() != 0) {
    std::cout << "eID: "<< eID_count << " INTTRawHitSanityCheck::process_event - evt_felix_BcoDiffHist_vec[0] is not empty" << std::endl;
    gSystem->Exit(1);
    exit(1);
  }

  // for (int i = 0; i < 8; i++) {
  //   for (int j = 0; j < 14; j++) {
  //     evt_ladder_clone_hit[i][j].clear();
  //   }
  // }

  if (eID_count % 200 == 0) {std::cout << "eID: "<< eID_count << std::endl;}

  if (HaveGL1_tag){PrepareGL1(topNode);}

  PrepareINTT(topNode);

  if(Get_InttRawHit_ntuple){
    GetInttRawHitInfo(topNode);
    tree_out_RawHit -> Fill();
  }

  tree_out -> Fill();

  eID_count += 1;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTRawHitSanityCheck::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "INTTRawHitSanityCheck::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTRawHitSanityCheck::EndRun(const int runnumber)
{
  file_out->cd();

  for (int i = 0; i < int(all_felix_BcoDiffHist_vec.size()); i++)
  {
    all_felix_BcoDiffHist_vec[i] -> Write();
  }

  h2D_NInttHitInner_NInttHitOuter -> Write();
  h2D_NInttHit_TrigSpikeSize -> Write();
  h1D_SemiClusPhiSize -> Write();

  tree_out->Write();
  if (Get_InttRawHit_ntuple){tree_out_RawHit -> Write();}

  THStack * hstack = new THStack("hstack","hstack");
  for (auto &pair : h1D_nChipHit_map){
    hstack -> Add(pair.second);
  }
  TH1D * h1D_nChipHit_map_stacked = (TH1D*)hstack -> GetStack() -> Last();
  h1D_nChipHit_map_stacked -> Write("h1D_nChipHit_map_stacked");

  for (auto &pair : h1D_nChipHit_map){
    pair.second -> Write();
  }

  file_out->Close();

  std::cout << "INTTRawHitSanityCheck::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTRawHitSanityCheck::End(PHCompositeNode *topNode)
{
  std::cout << "INTTRawHitSanityCheck::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTRawHitSanityCheck::Reset(PHCompositeNode *topNode)
{
 std::cout << "INTTRawHitSanityCheck::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void INTTRawHitSanityCheck::Print(const std::string &what) const
{
  std::cout << "INTTRawHitSanityCheck::Print(const std::string &what) const Printing info for " << what << std::endl;
}


INTTRawHitSanityCheck::chip_clu_info INTTRawHitSanityCheck::SemiChipClustering(int FELIX_in, int FELIX_ch_in, int chip_in)
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

INTTRawHitSanityCheck::chip_clu_info INTTRawHitSanityCheck::find_Ngroup(TH1D * hist_in)
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

    INTTRawHitSanityCheck::chip_clu_info out_str;

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

double INTTRawHitSanityCheck::CoM_cluster_pos(TH1D * hist_in, double edge_l, double edge_r)
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

void INTTRawHitSanityCheck::GetInttRawHitInfo(PHCompositeNode *topNode)
{
    std::cout << "Get InttRawHit info." << std::endl;

    for (int DST_felix_i = 0; DST_felix_i < 8; DST_felix_i++)// note : FELIX server
    {
      inttcont = nullptr;
      inttcont = findNode::getClass<InttRawHitContainer>(topNode, Form("%s_%d",m_InttRawNodeName.c_str(),DST_felix_i));

      if (!inttcont)
      {
        std::cout << PHWHERE << "Error, can't find INTTRAWHIT" << std::endl;
        exit(1);
      }

      for (unsigned int hit_i = 0; hit_i < inttcont->get_nhits(); hit_i++)
      {
          InttRawHit *intthit = inttcont->get_hit(hit_i);

          InttRawHit_bco_.push_back(intthit->get_bco());
          InttRawHit_packetid_.push_back(intthit->get_packetid());
          InttRawHit_word_.push_back(intthit->get_word());
          InttRawHit_fee_.push_back(intthit->get_fee());
          InttRawHit_channel_id_.push_back(intthit->get_channel_id());
          InttRawHit_chip_id_.push_back(intthit->get_chip_id());
          InttRawHit_adc_.push_back(intthit->get_adc());
          InttRawHit_FPHX_BCO_.push_back(intthit->get_FPHX_BCO());
          InttRawHit_full_FPHX_.push_back(intthit->get_full_FPHX());
          InttRawHit_full_ROC_.push_back(intthit->get_full_ROC());
          InttRawHit_amplitude_.push_back(intthit->get_amplitude());
      }

    }

    NInttRawHits_ = int(InttRawHit_bco_.size());    
}