//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in INTTBcoResolution.h.
//
// INTTBcoResolution(const std::string &name = "INTTBcoResolution")
// everything is keyed to INTTBcoResolution, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// INTTBcoResolution::~INTTBcoResolution()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int INTTBcoResolution::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int INTTBcoResolution::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int INTTBcoResolution::process_event(PHCompositeNode *topNode)
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
// int INTTBcoResolution::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int INTTBcoResolution::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int INTTBcoResolution::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int INTTBcoResolution::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void INTTBcoResolution::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "INTTBcoResolution.h"

#include <cmath>
#include <bitset>
#include <numeric>

//____________________________________________________________________________..
INTTBcoResolution::INTTBcoResolution(
  const std::string &name,
  const int process_id_in,
  const int runnumber_in,
  const std::string output_directory_in,
  const std::string hotchannel_full_directory_in,
  const bool apply_hotchannel_in,
  const bool clone_hit_remove_in,
  const bool clone_hit_remove_BCO_in,
  const bool hitQA_in,
  const bool check_clone_hit_in
):
  SubsysReco(name),
  process_id(process_id_in),
  runnumber(runnumber_in),
  output_directory(output_directory_in),
  hotchannel_full_directory(hotchannel_full_directory_in),
  apply_hotchannel_tag(apply_hotchannel_in),
  clone_hit_remove_tag(clone_hit_remove_in),
  clone_hit_remove_BCO_tag(clone_hit_remove_BCO_in),
  hitQA_tag(hitQA_in),
  check_clone_hit_tag(check_clone_hit_in),
  inttcont(nullptr),
  gl1(nullptr)
  
{
  std::cout << "INTTBcoResolution::INTTBcoResolution(const std::string &name) Calling ctor" << std::endl;

  if (clone_hit_remove_tag && clone_hit_remove_BCO_tag) {
    std::cout<<"INTTBcoResolution::INTTBcoResolution - clone_hit_remove and clone_hit_remove_BCO cannot be true at the same time"<<std::endl;

    gSystem->Exit(1);
    exit(1);
  }

  output_file_name = "BcoDiffNtuple_"+std::to_string(runnumber);
  output_file_name += (clone_hit_remove_tag) ? "_clonehitremove" : "";
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
}

//____________________________________________________________________________..
INTTBcoResolution::~INTTBcoResolution()
{
  std::cout << "INTTBcoResolution::~INTTBcoResolution() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int INTTBcoResolution::Init(PHCompositeNode *topNode)
{
  std::cout << "INTTBcoResolution::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  out_MBDNS_tight_inclusive = -999;
  out_MBDNS_tight_vtx10cm = -999;
  out_MBDNS_tight_vtx30cm = -999;

  evt_felix_BcoDiffHist_vec.clear();
  all_felix_BcoDiffHist_vec.clear();

  for (int i = 0; i < 8; i++)
  {
    evt_felix_BcoDiffHist_vec.push_back(new TH1F(Form("evt_felix_BcoDiffHist_%d",i),Form("evt_felix_BcoDiffHist_%d;BCO_Diff;Entries (/1)",i),128,0,128));
    all_felix_BcoDiffHist_vec.push_back(new TH1F(Form("all_felix_BcoDiffHist_%d",i),Form("all_felix_BcoDiffHist_%d;BCO_Diff;Entries (/1)",i),128,0,128));
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

  // note : load hot channel map
  PrepareHotChannel();

  // note : prepare the structure of the output root file
  PrepareOutFile();

  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTBcoResolution::PrepareOutFile()
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
  
  tree_out -> Branch("spikeM1_location", &out_spikeM1_location); 
  tree_out -> Branch("spikeM1_height", &out_spikeM1_height); 
  tree_out -> Branch("spikeM2_location", &out_spikeM2_location); 
  tree_out -> Branch("spikeM2_height", &out_spikeM2_height); 
  tree_out -> Branch("spikeM3_location", &out_spikeM3_location); 
  tree_out -> Branch("spikeM3_height", &out_spikeM3_height); 

  tree_out -> Branch("spikeP1_location", &out_spikeP1_location); 
  tree_out -> Branch("spikeP1_height", &out_spikeP1_height); 
  tree_out -> Branch("spikeP2_location", &out_spikeP2_location); 
  tree_out -> Branch("spikeP2_height", &out_spikeP2_height); 
  tree_out -> Branch("spikeP3_location", &out_spikeP3_location); 
  tree_out -> Branch("spikeP3_height", &out_spikeP3_height); 

  tree_out -> Branch("HeightFirst_location", &out_HeightFirst_location); 
  tree_out -> Branch("HeightFirst_height", &out_HeightFirst_height);
  tree_out -> Branch("HeightSecond_location", &out_HeightSecond_location); 
  tree_out -> Branch("HeightSecond_height", &out_HeightSecond_height); 
  tree_out -> Branch("HeightThird_location", &out_HeightThird_location); 
  tree_out -> Branch("HeightThird_height", &out_HeightThird_height); 
  tree_out -> Branch("HeightFourth_location", &out_HeightFourth_location); 
  tree_out -> Branch("HeightFourth_height", &out_HeightFourth_height); 

  tree_out -> Branch("felix0_bcodiff", &out_felix0_bcodiff); 
  tree_out -> Branch("felix1_bcodiff", &out_felix1_bcodiff); 
  tree_out -> Branch("felix2_bcodiff", &out_felix2_bcodiff); 
  tree_out -> Branch("felix3_bcodiff", &out_felix3_bcodiff); 
  tree_out -> Branch("felix4_bcodiff", &out_felix4_bcodiff); 
  tree_out -> Branch("felix5_bcodiff", &out_felix5_bcodiff); 
  tree_out -> Branch("felix6_bcodiff", &out_felix6_bcodiff); 
  tree_out -> Branch("felix7_bcodiff", &out_felix7_bcodiff); 

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


  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTBcoResolution::PrepareHotChannel()
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

//____________________________________________________________________________..
int INTTBcoResolution::InitRun(PHCompositeNode *topNode)
{
  std::cout << "INTTBcoResolution::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<int> INTTBcoResolution::prepare_trigger_vec(long long trigger_input)
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

std::map<int,int> INTTBcoResolution::prepare_trigger_map(std::vector<int> trigger_vec_input)
{
    std::map<int,int> output_map; output_map.clear();

    for (int trig : trigger_vec_input){
      output_map[trig] = trig;
    }

    return output_map;
}

int INTTBcoResolution::PrepareGL1(PHCompositeNode *topNode)
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
    std::cout << "INTTBcoResolution::process_event - GL1RAWHIT node not found" << std::endl;
    
    gSystem->Exit(1);
    exit(1);
  }

  // gl1rawhitbco = gl1->lValue(0, "BCO");
  out_GL1_bco = gl1->getBCO();
  live_trigger_decimal = gl1->lValue(0,"LiveVector");

  if (live_trigger_decimal < 0)
  {
    std::cout << "INTTBcoResolution::PrepareGL1 - live trigger is negative, "<< live_trigger_decimal << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  live_trigger_map = prepare_trigger_map(prepare_trigger_vec(live_trigger_decimal));

  out_MBDNS_tight_inclusive = (live_trigger_map.find(MBDNS_tight_inclusive) != live_trigger_map.end()) ? 1 : 0;
  out_MBDNS_tight_vtx10cm = (live_trigger_map.find(MBDNS_tight_vtx10cm) != live_trigger_map.end()) ? 1 : 0;
  out_MBDNS_tight_vtx30cm = (live_trigger_map.find(MBDNS_tight_vtx30cm) != live_trigger_map.end()) ? 1 : 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<int> INTTBcoResolution::GetHistBinEntry(TH1F * hist)
{
  std::vector<int> output_vec; output_vec.clear();

  for (int i = 1; i <= hist->GetNbinsX(); i++)
  {
    output_vec.push_back(hist->GetBinContent(i));
  }

  return output_vec;
}

int INTTBcoResolution::PrepareINTT(PHCompositeNode *topNode)
{

  out_INTT_bcofull = -999;
  out_total_nHit = -999;
  out_total_nHit_post = -999;

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

  out_total_nHit = inttcont->get_nhits();
  if (out_total_nHit == 0) {
    out_INTT_bcofull = -999;
    std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - no INTT hit found" << std::endl;
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
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - server out of range, this server is : "<< server << std::endl;
      continue;
    }

    if (hitQA_tag && (felix_ch < 0 || felix_ch > 13)) // note : felix channel ID 0 - 13
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - felix channel out of range, this channel is : "<< felix_ch << std::endl;
      continue;
    }

    if (hitQA_tag && (chip < 0 || chip > 25)) // note : chip ID 0 - 25
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - chip out of range, this chip is : "<< chip << std::endl;
      continue;
    }

    if (hitQA_tag && (channel < 0 || channel > 127)) // note : channel ID 0 - 127
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - channel out of range, this channel is : "<< channel << std::endl;
      continue;
    }

    if (hitQA_tag && (adc < 0 || adc > 7)) // note : adc ID 0 - 7
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - adc out of range, this adc is : "<< adc << std::endl;
      continue;
    }
    
    if (hitQA_tag && (bco < 0 || bco > 127)) // note : bco ID 0 - 127
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - bco out of range, this bco is : "<< bco << std::endl;
      continue;
    }

    

    if (apply_hotchannel_tag) // note : if we want to remove the hot channels
    {
      if (hot_channel_map.find(Form("%d_%d_%d_%d",server,felix_ch,chip,channel)) != hot_channel_map.end())
      {
        continue;
      }
    }

    if (clone_hit_remove_tag || clone_hit_remove_BCO_tag) // note : can only be one of each
    {
      if (clone_hit_remove_tag)
      {
        if (evt_inttHits_map.find(Form("%d_%d_%d_%d",server,felix_ch,chip,channel)) == evt_inttHits_map.end()) // note : if it's not found, we just add it
        {
          evt_inttHits_map[Form("%d_%d_%d_%d",server,felix_ch,chip,channel)] = {server,felix_ch,chip,channel,adc,bco};
        }
      }
      else if (clone_hit_remove_BCO_tag)
      {
        if (evt_inttHits_map.find(Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)) == evt_inttHits_map.end()) // note : if it's not found, we just add it
        {
          evt_inttHits_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc,bco};
        } 
      }
    }
    else // note : if we don't want to remove the clone hits
    {
      evt_inttHits_map[Form("%d",i)] = {server,felix_ch,chip,channel,adc,bco}; // note : only index i to make the key unique
    }

    out_totalHit_forCloneStudy += 1;

    if (check_clone_hit_tag)
    {
      if (evt_inttHits_BCO_map.find(Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)) == evt_inttHits_BCO_map.end()) // note : if it's not found, we just add it
      {
        evt_inttHits_BCO_map[Form("%d_%d_%d_%d_%d",server,felix_ch,chip,channel,bco)] = {server,felix_ch,chip,channel,adc,bco};
      }
      else // note : here leaves the window for the clone hit study for the future, if needed.
      {
        evt_clone_hit_count[server][felix_ch][chip][channel] += 1; // note : 1 means, there are two hits in the same position 
      }
    }

  } // note : end of INTT raw hit loop

  out_total_nHit_post = evt_inttHits_map.size();

  for (const auto& pair : evt_inttHits_map)
  {
    int server = pair.second.hit_server;
    // int felix_ch = pair.second.hit_felix_ch;
    // int chip = pair.second.hit_chip;
    // int channel = pair.second.hit_channel;
    // int adc = pair.second.hit_adc;
    int bco = pair.second.hit_bco;

    int bco_diff = (bco - (out_INTT_bcofull & 0x7fU) + 128) % 128;

    if (bco_diff < 0 || bco_diff > 127) // note : bco_diff ID 0 - 127
    {
      std::cout << "eID: "<< eID_count <<" INTTBcoResolution::PrepareINTT - bco_diff out of range, this bco_diff is : "<< bco_diff<<" hitbco : "<<bco<<" bco_full : "<<out_INTT_bcofull<< std::endl;
      
      gSystem->Exit(1);
      exit(1);
    }

    evt_felix_BcoDiffHist_vec[server] -> Fill(bco_diff); // todo: right now the unit is down to server, not half ladder
    all_felix_BcoDiffHist_vec[server] -> Fill(bco_diff);
  }
  
  out_felix0_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[0]); // todo: right now the unit is down to server, not half ladder
  out_felix1_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[1]);
  out_felix2_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[2]);
  out_felix3_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[3]);
  out_felix4_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[4]);
  out_felix5_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[5]);
  out_felix6_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[6]);
  out_felix7_bcodiff = GetHistBinEntry(evt_felix_BcoDiffHist_vec[7]);

  for (int i = 0; i < 8; i++)
  {
    PrepareHistInfo(evt_felix_BcoDiffHist_vec[i]);
  }

  if (check_clone_hit_tag)
  {
    PrepareCloneHitInfo();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int INTTBcoResolution::PrepareHistInfo(TH1F * hist)
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

int INTTBcoResolution::PrepareCloneHitInfo()
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
int INTTBcoResolution::process_event(PHCompositeNode *topNode)
{
  // std::cout << "INTTBcoResolution::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

  for (auto& hist : evt_felix_BcoDiffHist_vec) {hist -> Reset("ICESM");}
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

  out_totalHit_forCloneStudy = 0;

  if (evt_felix_BcoDiffHist_vec[0]->GetEntries() != 0) {
    std::cout << "eID: "<< eID_count << " INTTBcoResolution::process_event - evt_felix_BcoDiffHist_vec[0] is not empty" << std::endl;
    gSystem->Exit(1);
    exit(1);
  }

  // for (int i = 0; i < 8; i++) {
  //   for (int j = 0; j < 14; j++) {
  //     evt_ladder_clone_hit[i][j].clear();
  //   }
  // }

  if (eID_count % 1000 == 0) {std::cout << "eID: "<< eID_count << std::endl;}

  PrepareGL1(topNode);

  PrepareINTT(topNode);

  tree_out -> Fill();

  eID_count += 1;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTBcoResolution::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "INTTBcoResolution::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTBcoResolution::EndRun(const int runnumber)
{
  file_out->cd();

  for (int i = 0; i < int(all_felix_BcoDiffHist_vec.size()); i++)
  {
    all_felix_BcoDiffHist_vec[i] -> Write();
  }

  tree_out->Write();
  file_out->Close();

  std::cout << "INTTBcoResolution::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTBcoResolution::End(PHCompositeNode *topNode)
{
  std::cout << "INTTBcoResolution::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int INTTBcoResolution::Reset(PHCompositeNode *topNode)
{
 std::cout << "INTTBcoResolution::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void INTTBcoResolution::Print(const std::string &what) const
{
  std::cout << "INTTBcoResolution::Print(const std::string &what) const Printing info for " << what << std::endl;
}

