#include "InttHitCarryoverMonitor.h"


bool CheckCloneHits(InttRawHit* hit, InttRawHitStructure& structure);
std::unordered_map< Chip, std::vector<LayerAndBco> > PrepareHitInfoByChip(const InttRawHitStructure& event);
void CalculateHitBco(InttRawHitStructure& event, const int triggered_BcoDiff_, const int64_t intt_clock_offset);
bool EvaluateThisEvent(const InttRawHitStructure event);
void CountNumberOfHitsInNominalTiming(InttRawHitStructure& event, const int triggered_BcoDiff, const int64_t intt_clock_offset);
void CountCarriedOverHitsPerEventBasis(const InttRawHitStructure event_of_interest, InttRawHitStructure& next_event);
int DeriveTriggerBcoDiffOfARun(int run);
int GetNumberOfHitsInNormalTimingInAHalfladder(const std::map< HalfLadder, std::vector<LayerAndBco> > list, const HalfLadder halfladder, const int standard_timing);
int GetNumberOfHitsInAChip_HalfladderbyHalfladder(const std::map< HalfLadder, std::vector<LayerAndBco> > list, const HalfLadder halfladder, const int standard_timing);
int GetNumberOfHitsInNormalTimingInAHalfladder(const std::map< HalfLadder, std::vector<LayerAndBco> > list, const HalfLadder halfladder, const int gl1_bco, const int triggered_BcoDiff, const int64_t intt_clock_offset);
int GetNumberOfHitsInNormalTimingInAChip(const std::unordered_map< Chip, std::vector<LayerAndBco> > list, const Chip chip, const int standard_timing);
int GetNumberOfHitsInAChip_ChipByChip(const std::unordered_map< Chip, std::vector<LayerAndBco> > list, const Chip chip, const int standard_timing);


InttHitCarryoverMonitor::InttHitCarryoverMonitor(const std::string &name, const int run, const std::string arg_file_path_to_hot_channel_map, std::string job_index, bool save_tree):
  SubsysReco(name),
  save_tree_(save_tree),
  my_event_counter_(0),
  gl1_bco_previous_(0),
  intt_clock_offset_(0),
  // gl1_bco_ith_(0),
  // interval_from_previous_(0),
  // trigger_BcoDiff_(arg_trigger_Bcodiff),
  file_path_to_hot_channel_map_(arg_file_path_to_hot_channel_map)
{
  std::cout << "InttHitCarryoverMonitor(const std::string &name, int run) Calling the constructor" << std::endl;
  
  std::string filename = Form("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/%d/%d-%s.root", run, run, job_index.c_str());
  output_file_ = new TFile(filename.c_str(), "recreate");
  if ( !output_file_ || output_file_->IsZombie()){
    std::cerr << "ERROR : File '" << filename << "' not found or could not be opend." << std::endl;
    std::exit(1);
  }

  /* make braches */

  if ( save_tree_ ) {
    output_tree_ = new TTree("tree", "tree");
    output_tree_->Branch("n_intt_raw_hits", &ith_event_.n_intt_raw_hits);
    output_tree_->Branch("bco", &ith_event_.bco);
    output_tree_->Branch("felix_server",&ith_event_.felix_server);
    output_tree_->Branch("word",&ith_event_.word);
    output_tree_->Branch("felix_channel",&ith_event_.felix_channel);
    output_tree_->Branch("channel",&ith_event_.channel);
    output_tree_->Branch("chip",&ith_event_.chip);
    output_tree_->Branch("adc",&ith_event_.adc);
    output_tree_->Branch("FPHX_BCO",&ith_event_.FPHX_BCO);
    output_tree_->Branch("full_FPHX",&ith_event_.full_FPHX);
    output_tree_->Branch("full_ROC",&ith_event_.full_ROC);
    output_tree_->Branch("amplitude",&ith_event_.amplitude);
    output_tree_->Branch("is_MBDNS_tight_inclusive",&ith_event_.is_MBDNS_tight_inclusive);
    output_tree_->Branch("is_MBDNS_tight_vtx10cm",&ith_event_.is_MBDNS_tight_vtx10cm);
    output_tree_->Branch("is_MBDNS_tight_vtx30cm",&ith_event_.is_MBDNS_tight_vtx30cm);
    output_tree_->Branch("hitbco_peak",&ith_event_.hitbco_peak);
    output_tree_->Branch("bco_diff_peak",&ith_event_.bco_diff_peak);
    output_tree_->Branch("n_hit_inner",&ith_event_.n_hit_inner);
    output_tree_->Branch("n_hit_outer",&ith_event_.n_hit_outer);
    output_tree_->Branch("num_of_carried_hits",&ith_event_.num_of_carried_hits);
    output_tree_->Branch("num_of_carried_hits_inner",&ith_event_.num_of_carried_hits_inner);
    output_tree_->Branch("num_of_carried_hits_outer",&ith_event_.num_of_carried_hits_outer);
    // output_tree_->Branch("h1D_fphxbco",&ith_event_.h1D_fphxbco); /* probably will no be used. */
    // output_tree_->Branch("hit_info_by_halfladder",&ith_event_.hit_info_by_halfladder);
    // output_tree_->Branch("hit_info_by_chip",&ith_event_.hit_info_by_chip);
    output_tree_->Branch("gl1_bco_ith",&ith_event_.gl1_bco);
    output_tree_->Branch("gl1_bco_previous_",&gl1_bco_previous_);
    output_tree_->Branch("interval_from_previous",&ith_event_.interval_from_previous);
  }


  h2D_NInttHitInner_NInttHitOuter_ = new TH2D("h2D_NInttHitInner_NInttHitOuter","h2D_NInttHitInner_NInttHitOuter; NHits in normal timing (inner); NHits in normal timing (outer)", 400, 0, 10000, 400, 0, 10000);
  h2D_NInttHitInner_NInttHitOuter_corrected_ = new TH2D("h2D_NInttHitInner_NInttHitOuter_corrected","h2D_NInttHitInner_NInttHitOuter_corrected; NHits in normal timing (inner); NHits in normal timing (outer)", 400, 0, 10000, 400, 0, 10000);
  // h2D_EventBcoSpacing_NCarriedHits_ = new TH2D("h2D_BcoSpace_NCarriedHits","h2D_BcoSpace_NCarriedHits; Event-BCO spacing; N Carried over hits",150,0,150,2000,0,10000);
  h2D_NCarriedInner_NCarriedOuter_ = new TH2D("h2D_NCarriedInner_NCarriedOuter", "h2D_NCarriedInner_NCarriedOuter; inner; outer", 1000,0,5000, 1000,0,5000 );
  h2D_Halfladder_NCOH_ = new TH2D("h2D_Halfladder_NCOH","h2D_Halfladder_NCOH; Half ladder; Number of Carried-over hits", 113, 0, 113, 300, 0, 300);

  /* temp check */
  h1D_Halfladder_NHitInPreviousEvent_example_ = new TH1D("h1D_Halfladder_NHitInPreviousEvent_example_","h1D_Halfladder_NHitInPreviousEvent_example_; Half ladder; Number of Hit in ith-event", 113, 0, 113);
  h1D_Halfladder_NCOH_example_ = new TH1D("h1D_Halfladder_NCOH_example_","h1D_Halfladder_NCOH_example_; Half ladder; Number of Carried-over hits", 113, 0, 113);

  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_","h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 6000, 0, 24000);
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_all_event_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_all_event_", "h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_all_event_; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 500, 0, 500);
  h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_all_event_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_all_event_", "h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_all_event_; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 127, 0, 127);

  h2D_EventBcoSpacing_NHitInPreviousEvent_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent","h2D_EventBcoSpacing_NHitInPreviousEvent; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 5000, 0, 20000);
  h2D_EventBcoSpacing_NCohInNextEvent_ = new TH2D("h2D_EventBcoSpacing_NCohInNextEvent","h2D_EventBcoSpacing_NCohInNextEvent; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 5000, 0, 20000);
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder", "h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 500, 0, 500);
  h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent_bychip", "h2D_EventBcoSpacing_NHitInPreviousEvent_bychip; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 127, 0, 127);
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner", "h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 500, 0, 500);
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer_ = new TH2D("h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer", "h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer; Event-BCO Spacing; Number of Hit in ith-event", 128, 0, 128, 500, 0, 500);
  
  
  h2D_MisasignedBcoDiff_NHitInPreviousEvent_ = new TH2D("h2D_MisasignedBcoDiff_NHitInPreviousEvent","h2D_MisasignedBcoDiff_NHitInPreviousEvent; Misassigned BCO diff; Number of Hit in ith-event", 128, 0, 128, 5000, 0, 20000);
  h2D_MisasignedBcoDiff_NCohInNextEvent_ = new TH2D("h2D_MisasignedBcoDiff_NCohInNextEvent","h2D_MisasignedBcoDiff_NCohInNextEvent; Misassigned BCO diff; Number of Carried-over Hit in next-event", 128, 0, 128, 5000, 0, 20000);    

  h2D_NInttHitInner_NInttHitOuter_narrow_spacing_ = new TH2D("h2D_NInttHitInner_NInttHitOuter_narrow_spacing","h2D_NInttHitInner_NInttHitOuter_narrow_spacing; NHits in normal timing (inner); NHits in normal timing (outer)", 400, 0, 10000, 400, 0, 10000);
  
  h1D_Multiplicity_long_event_interval_ = new TH1D("h1D_Multiplicity_long_event_interval", "h1D_Multiplicity_long_event_interval; Multiplicity; Entries (normalized)", 5000,0,20000);
  h1D_Multiplicity_short_event_interval_ = new TH1D("h1D_Multiplicity_short_event_interval", "h1D_Multiplicity_short_event_interval; Multiplicity; Entries (normalized)", 5000,0,20000);
  h1D_Multiplicity_short_event_interval_corrected_ = new TH1D("h1D_Multiplicity_short_event_interval_corrected", "h1D_Multiplicity_short_event_interval_corrected; Multiplicity; Entries (normalized)", 5000,0,20000);

  h2D_EventBcoSpacing_CorrectedMultiplicity_ = new TH2D("h2D_EventBcoSpacing_CorrectedMultiplicity","h2D_EventBcoSpacing_CorrectedMultiplicity; Event-BCO Spacing; Multiplicity after push-back", 128, 0, 128, 5000, 0, 20000);
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_ = new TH2D("h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_","h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_; Event-BCO Spacing; Multiplicity after push-back", 128, 0, 128, 6000, 0, 24000);


  trigger_BcoDiff_ = DeriveTriggerBcoDiffOfARun(run);
};

InttHitCarryoverMonitor::~InttHitCarryoverMonitor(){
  std::cout << "InttHitCarryoverMonitor::~InttHitCarryoverMonitor() Calling the deconstructor" << std::endl;

};

int InttHitCarryoverMonitor::Init(PHCompositeNode *topNode){
  std::cout << "InttHitCarryoverMonitor::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  
  this->PrepareHotChannelMap();
  
  return Fun4AllReturnCodes::EVENT_OK;
};


int InttHitCarryoverMonitor::InitRun(PHCompositeNode *topNode){
  std::cout << "InttHitCarryoverMonitor::InitRun(PHCompositeNode *topNode) Initializing for Run XXX... " << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
};

int InttHitCarryoverMonitor::process_event(PHCompositeNode *topNode){
  if ( my_event_counter_ %200 == 0 ) {
    std::cout<<"eID: " << my_event_counter_ << std::endl;
  }

  for (int i=0; i<8; i++) {
    std::string node_name = Form("%s_%d",node_name_intt_raw_hit_.c_str(), i);
    container_ith_event_ = findNode::getClass<InttRawHitContainerv2>(topNode, node_name.c_str());
    /* Prepare ith_event_ here. Clone hits and hot channels are removed. */
    this->ConvertFromRawHit(container_ith_event_); 
  }

  // std::string node_name = Form("%s",node_name_intt_raw_hit_.c_str());
  // container_ith_event_ = findNode::getClass<InttRawHitContainerv2>(topNode, node_name.c_str());
  // /* Prepare ith_event_ here. Clone hits and hot channels are removed. */
  // this->ConvertFromRawHit(container_ith_event_); 

  GetTriggerInformation(topNode, ith_event_);
  CountNumberOfHitsInNominalTiming(ith_event_, trigger_BcoDiff_, intt_clock_offset_);

  if (ith_event_.n_intt_raw_hits == 0) {
    std::cout << "ith_event_.n_intt_raw_hits = 0"<< std::endl ;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  CalculateHitBco(ith_event_, trigger_BcoDiff_, intt_clock_offset_); 

  if ( my_event_counter_ == 0 ) {
    return Fun4AllReturnCodes::EVENT_OK; /* Move to next event since it's the first event and we need 2 events to start comparing. */
  }


  CountCarriedOverHitsPerEventBasis(previous_event_, ith_event_);
  CountCarriedOverHitsPerHalfladderBasis(previous_event_, ith_event_); /* `h2D_Halfladder_NCOH_` and `h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_` are filled here. */
  CountCarriedOverHitsPerChipBasis(previous_event_, ith_event_); /* `h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_` is filled here */

  this->FillHistograms();

  this->PushBackNarrowSpacingCarryover();

  CheckMultiplicity();

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttHitCarryoverMonitor::ResetEvent(PHCompositeNode *topNode){
  // std::cout << "InttHitCarryoverMonitor::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  
  if ( save_tree_ ) {
    output_tree_->Fill();
  }

  gl1_bco_previous_ = ith_event_.gl1_bco;

  /* temporary check */
  if ( ith_event_.n_intt_raw_hits==0 ) {
    std::cout << "-- n_intt_raw_hits == 0 met in event " << my_event_counter_ << ". " << std::endl;
  }

  /* temporary check */
  if ( ith_event_.num_of_carried_hits > 0 ) {
    int event_bco_spacing = ith_event_.interval_from_previous;

    std::cout << "-- Carryover found in event " << my_event_counter_ << ". " << std::endl;
    std::cout << "   Event interval = " << event_bco_spacing << std::endl;
    std::cout << "   The num of hits in the previous event = " << previous_event_.n_hit_inner << "(inner) + " << previous_event_.n_hit_outer << "(outer)." << std::endl;
    std::cout << "   The num_of_carried_hits = " << ith_event_.num_of_carried_hits << " = " << ith_event_.num_of_carried_hits_inner << " (inner) + " << ith_event_.num_of_carried_hits_outer << " (outer)." << std::endl;
  }


  previous_event_ = ith_event_;
  my_event_counter_++;

  ith_event_.n_intt_raw_hits=0;
  ith_event_.bco.clear();
  ith_event_.felix_server.clear();
  ith_event_.word.clear();
  ith_event_.felix_channel.clear();
  ith_event_.chip.clear();
  ith_event_.channel.clear();
  ith_event_.adc.clear();
  ith_event_.FPHX_BCO.clear();
  ith_event_.full_FPHX.clear();
  ith_event_.full_ROC.clear();
  ith_event_.amplitude.clear();

  ith_event_.h1D_fphxbco->Reset();
  ith_event_.hit_info_by_halfladder.clear();
  ith_event_.hit_info_by_chip.clear();
  ith_event_.hit_already_seen.clear();

  ith_event_.hitbco_peak = -999;
  ith_event_.bco_diff_peak = -999;

  ith_event_.num_of_carried_hits = 0;
  ith_event_.num_of_carried_hits_inner = 0;
  ith_event_.num_of_carried_hits_outer = 0;

  ith_event_.is_MBDNS_tight_inclusive = false;
  ith_event_.is_MBDNS_tight_vtx10cm = false;
  ith_event_.is_MBDNS_tight_vtx30cm = false;
  
  return Fun4AllReturnCodes::EVENT_OK;

};

int InttHitCarryoverMonitor::Reset(PHCompositeNode *topNode){
  std::cout << "InttHitCarryoverMonitor::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
};

int InttHitCarryoverMonitor::EndRun(const int runnumber){
  std::cout << "InttHitCarryoverMonitor::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
};

int InttHitCarryoverMonitor::End(PHCompositeNode *topNode){
  std::cout << "InttHitCarryoverMonitor::End(PHCompositeNode *topNode) This is the End... " << std::endl;

  output_file_->cd();

  h2D_NInttHitInner_NInttHitOuter_->Write();
  h2D_NInttHitInner_NInttHitOuter_corrected_->Write();
  // h2D_EventBcoSpacing_NCarriedHits_->Write();
  h2D_NCarriedInner_NCarriedOuter_->Write();
  h2D_Halfladder_NCOH_->Write();

  h1D_Halfladder_NHitInPreviousEvent_example_->Write();
  h1D_Halfladder_NCOH_example_->Write();

  h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_->Write();
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_all_event_->Write();
  h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_all_event_->Write();

  h2D_EventBcoSpacing_NHitInPreviousEvent_->Write();
  h2D_EventBcoSpacing_NCohInNextEvent_->Write(); 
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_->Write();
  h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_->Write();
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner_->Write(); 
  h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer_->Write(); 

  h2D_MisasignedBcoDiff_NHitInPreviousEvent_->Write();
  h2D_MisasignedBcoDiff_NCohInNextEvent_->Write();

  h2D_NInttHitInner_NInttHitOuter_narrow_spacing_->Write();

  h1D_Multiplicity_long_event_interval_->Scale( 1 / h1D_Multiplicity_long_event_interval_->GetEntries() );
  h1D_Multiplicity_long_event_interval_->Write();
  h1D_Multiplicity_short_event_interval_->Scale( 1/ h1D_Multiplicity_short_event_interval_->GetEntries() );
  h1D_Multiplicity_short_event_interval_->Write();
  h1D_Multiplicity_short_event_interval_corrected_->Scale( 1/ h1D_Multiplicity_short_event_interval_corrected_->GetEntries() );
  h1D_Multiplicity_short_event_interval_corrected_->Write();

  h2D_EventBcoSpacing_CorrectedMultiplicity_->Write();
  h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_->Write();
  
  if ( save_tree_ ) {
    output_tree_->Write();
  }
  output_file_->Close();

  return Fun4AllReturnCodes::EVENT_OK;
};

// void InttHitCarryoverMonitor::Print(const std::string &what) const {
//   std::cout << "InttHitCarryoverMonitor::Print(const std::string &what) const Printing info for " << what << std::endl;
// }


























void InttHitCarryoverMonitor::GetTriggerInformation(PHCompositeNode *topNode, InttRawHitStructure& event) {
  event.is_MBDNS_tight_inclusive = false; 
  event.is_MBDNS_tight_vtx10cm = false; 
  event.is_MBDNS_tight_vtx30cm = false; 

  Gl1Packetv3* gl1 = findNode::getClass<Gl1Packetv3>(topNode, node_name_gl1_raw_hit_.c_str());
  if (!gl1) {
    std::cerr << "InttHitCarryoverMonitor::process_event - GL1RAWHIT node not found" << std::endl;
    exit(1);
  }

  /* BCO information */
  event.gl1_bco = gl1->lValue(0,"BCO");
  event.interval_from_previous = event.gl1_bco - gl1_bco_previous_; /* gl1_bco_previous_ is hold by InttHitCarryoverMonitor, not by InttRawHitStructure */
  if ( (intt_clock_offset_ == 0) && (event.n_intt_raw_hits > 0) ) {
    uint64_t intt_bco = event.bco.at(0);
    intt_clock_offset_ = event.gl1_bco - intt_bco;
    std::cout << "intt_clock_offset_: " << intt_clock_offset_ << std::endl;
    if ( intt_bco != (event.gl1_bco-intt_clock_offset_) ) {
      std::cout << "bco not matched! gl1_bco: " << event.gl1_bco << "!= intt_bco(" << intt_bco << ") + offset(" << intt_clock_offset_ <<")." << std::endl;
    }
  }

  /* Live trigger information */
  long long live_trigger_decimal = gl1->lValue(0,"LiveVector");
  if (live_trigger_decimal < 0){
    std::cout << "InttHitCarryoverMonitor::GetTriggerInformation - live trigger is negative, "<< live_trigger_decimal << std::endl;
    return;
  }
  std::vector<int> live_trigger_vec = PrepareTriggerVector(live_trigger_decimal);
  /* if a trigger code found, set a corresponding flag as true*/
  event.is_MBDNS_tight_inclusive = std::find( live_trigger_vec.begin(), live_trigger_vec.end(), MBDNS_tight_inclusive_) != live_trigger_vec.end(); 
  event.is_MBDNS_tight_vtx10cm = std::find( live_trigger_vec.begin(), live_trigger_vec.end(), MBDNS_tight_vtx10cm_) != live_trigger_vec.end(); 
  event.is_MBDNS_tight_vtx30cm = std::find( live_trigger_vec.begin(), live_trigger_vec.end(), MBDNS_tight_vtx30cm_) != live_trigger_vec.end(); 

}


std::vector<int> InttHitCarryoverMonitor::PrepareTriggerVector(long long trigger_input) {
  std::bitset<64> trigger_input_bitset(trigger_input);
  std::vector<int> output_vec; output_vec.clear();

  for (unsigned int i=0; i<64; i++)	{
	  if (trigger_input_bitset.test(i)) { output_vec.push_back(i); }
	}

  return output_vec;
}


void InttHitCarryoverMonitor::PrepareHotChannelMap() {
// std::map<std::string,std::string> InttHitCarryoverMonitor::PrepareHotChannelMap() {
  TFile* input_file_hot_channel_map = TFile::Open(this->file_path_to_hot_channel_map_.c_str());
  if (!input_file_hot_channel_map)  {
    std::cout << "InttHitCarryoverMonitor::PrepareHotChannelMap - hot channel file not found" << std::endl;
    exit(1);
  }

  TTree* tree_hot_channel = (TTree*)input_file_hot_channel_map->Get(tree_name_hot_channel_.c_str());

  int IID;             
  int Ichannel;        
  int Ichip;           
  int Ifelix_channel;  
  int Ifelix_server;   
  int Iflag;

  tree_hot_channel->SetBranchAddress("IID",&IID);
  tree_hot_channel->SetBranchAddress("Ichannel",&Ichannel);
  tree_hot_channel->SetBranchAddress("Ichip",&Ichip);
  tree_hot_channel->SetBranchAddress("Ifelix_channel",&Ifelix_channel);
  tree_hot_channel->SetBranchAddress("Ifelix_server",&Ifelix_server);
  tree_hot_channel->SetBranchAddress("Iflag",&Iflag);

  // std::map<std::string,std::string> hot_channel_map; /* key はともかく、value を string にする必要はないような*/

  for (int i=0; i<tree_hot_channel->GetEntries(); i++) {
    tree_hot_channel->GetEntry(i);
    this->hot_channel_map_[Form("%d_%d_%d_%d",Ifelix_server,Ifelix_channel,Ichip,Ichannel)] = Form("%d",Iflag);
  }


}


// InttRawHitStructure InttHitCarryoverMonitor::ConvertFromRawHit(InttRawHitContainer* container, const std::map<std::string,std::string> hot_channel_map) {
void InttHitCarryoverMonitor::ConvertFromRawHit(InttRawHitContainer* container){//, const std::map<std::string,std::string> hot_channel_map) {

  // InttRawHitStructure event;
  
  int felix_server;
  int felix_channel;
  int chip;
  int channel;

  for ( unsigned int i = 0; i < container->get_nhits(); i++ ) {
    InttRawHit* hit = container->get_hit(i);

      /* some conversions needed to adjust to value range of hot_channel_map_ */
      felix_server = hit->get_packetid() - 3001;
      felix_channel = hit->get_fee();
      chip = (hit->get_chip_id() - 1) % 26;
      channel = hit->get_channel_id();

      if (hot_channel_map_.size() == 0) {std::cerr<<"map size zero"<<std::endl; exit(1);}
      bool is_from_hot_channel = hot_channel_map_.find(Form("%d_%d_%d_%d",felix_server,felix_channel,chip,channel)) != hot_channel_map_.end();
      if ( is_from_hot_channel ) {
        // std::cout << "from hot channel: " <<  Form("%d_%d_%d_%d",felix_server,felix_channel,chip,channel) << std::endl;
        continue; /* move to next hit */
      }

      bool is_clone_hit = CheckCloneHits(hit, ith_event_);
      if ( is_clone_hit ) {
        continue; /* move to next hit */
      }

      this->FillAHit(hit);
      ith_event_.n_intt_raw_hits++;

  }
}

void CountNumberOfHitsInNominalTiming(InttRawHitStructure& event, const int triggered_BcoDiff, const int64_t intt_clock_offset) {
  int n_hit_inner{0};
  int n_hit_outer{0};

  int triggered_BcoDiff_n1 = (triggered_BcoDiff - 1 + 128) % 128;
  int triggered_BcoDiff_p1 = (triggered_BcoDiff + 1 + 128) % 128;

  if ( !event.is_MBDNS_tight_vtx10cm ) {return;}

  for (int hit_index = 0; hit_index < event.n_intt_raw_hits; hit_index++) {
    InttNameSpace::RawData_s rawdata;
    rawdata.felix_server = event.felix_server.at(hit_index) - 3001;
    rawdata.felix_channel = event.felix_channel.at(hit_index);
    rawdata.chip = (event.chip.at(hit_index) - 1) % 26;
    rawdata.channel = event.channel.at(hit_index);

    InttNameSpace::Offline_s offlinedata = InttNameSpace::ToOffline(rawdata);
    if (offlinedata.layer != 3 && offlinedata.layer != 4 && offlinedata.layer != 5 && offlinedata.layer != 6){
      // std::cout << "pid: "<< rawdata.pid << ", fee: " << rawdata.fee << ", chp: " << rawdata.chp << ", chn: " << rawdata.chn << std::endl;
      // std::cout << "layer: " << offlinedata.layer << ", ladder_phi: " << offlinedata.ladder_phi << ", ladder_z: " << offlinedata.ladder_z << ", strip_z: " << offlinedata.strip_z << ", strip_phi: " << offlinedata.strip_phi << std::endl;
      // std::cout<<std::endl;
      continue;
    }

    // int bco_diff = (event.FPHX_BCO.at(hit_index) - (event.bco.at(0) & 0x7fU) + 128) % 128;
    // int bco_diff = (event.FPHX_BCO.at(hit_index) - (event.gl1_bco & 0x7fU) + 128) % 128;
    int bco_diff = (event.FPHX_BCO.at(hit_index) - ( (event.gl1_bco-intt_clock_offset) & 0x7fU) + 128) % 128;
    if (bco_diff != triggered_BcoDiff_n1 && bco_diff != triggered_BcoDiff && bco_diff != triggered_BcoDiff_p1) {
      continue;
    }


    if (offlinedata.layer == 3 || offlinedata.layer == 4){
      n_hit_inner++;
    } else if (offlinedata.layer == 5 || offlinedata.layer == 6) {
      n_hit_outer++;
    }
  }

  /* update the ith_event*/
  event.n_hit_inner = n_hit_inner;
  event.n_hit_outer = n_hit_outer;
}


void InttHitCarryoverMonitor::FillAHit(const InttRawHit* const hit) {

  this->ith_event_.bco.push_back( hit->get_bco() );
  this->ith_event_.felix_server.push_back( hit->get_packetid() );
  this->ith_event_.word.push_back( hit->get_word() );
  this->ith_event_.felix_channel.push_back( hit->get_fee() );
  this->ith_event_.chip.push_back( (hit->get_chip_id() - 1) % 26 );
  this->ith_event_.channel.push_back( hit->get_channel_id() );
  this->ith_event_.adc.push_back( hit->get_adc() );
  this->ith_event_.FPHX_BCO.push_back( hit->get_FPHX_BCO() );
  this->ith_event_.full_FPHX.push_back( hit->get_full_FPHX() );
  this->ith_event_.full_ROC.push_back( hit->get_full_ROC() );
  this->ith_event_.amplitude.push_back( hit->get_amplitude() );
  
}

bool CheckCloneHits(InttRawHit* hit, InttRawHitStructure& structure) {

  int bco = hit->get_bco();
  int felix_server = hit->get_packetid();
  int felix_channel = hit->get_fee();
  int chip = hit->get_chip_id();
  int channel = hit->get_channel_id();
  int adc = hit->get_adc();
  int fphx_bco = hit->get_FPHX_BCO();

  auto candidate = std::make_tuple(bco, felix_server, felix_channel, chip, channel, adc, fphx_bco);
  // auto candidate = std::make_tuple(bco, felix_server, felix_channel, chip, channel, adc);
  // auto candidate = std::make_tuple(bco, felix_server, felix_channel, chip, channel);
  
  if ( structure.hit_already_seen.find(candidate) == structure.hit_already_seen.end() ) {
    structure.hit_already_seen.insert(candidate); /* A new hit. Insert to the record. */
    return false;
  } else {
    return true; /* Clone hit. */
  }
  
}

/* structure.bco.at(0) の計算を含んでいるので n_intt_raw_hits == 0 に注意 */
void CalculateHitBco(InttRawHitStructure& event, const int triggered_BcoDiff_, const int64_t intt_clock_offset) {
  event.h1D_fphxbco->Reset();

  int triggered_BcoDiff_n1 = (triggered_BcoDiff_ - 1 + 128) % 128;
  int triggered_BcoDiff_p1 = (triggered_BcoDiff_ + 1 + 128) % 128;
  
  for (int hit_index = 0; hit_index < event.n_intt_raw_hits; hit_index++) {
    // int bco_diff = (event.FPHX_BCO.at(hit_index) - (event.bco.at(0) & 0x7fU) + 128) % 128;
    int bco_diff = (event.FPHX_BCO.at(hit_index) - ( (event.gl1_bco-intt_clock_offset) & 0x7fU) + 128) % 128;
    if (bco_diff != triggered_BcoDiff_n1 && bco_diff != triggered_BcoDiff_ && bco_diff != triggered_BcoDiff_p1) {
      continue;
    }
    event.h1D_fphxbco->Fill(event.FPHX_BCO.at(hit_index));
  }

  event.hitbco_peak = event.h1D_fphxbco->GetBinCenter( event.h1D_fphxbco->GetMaximumBin() );
  // event.bco_diff_peak = (event.hitbco_peak - (event.bco.at(0) & 0x7fU) + 128) % 128;
  event.bco_diff_peak = (event.hitbco_peak - ( (event.gl1_bco-intt_clock_offset)& 0x7fU) + 128) % 128;
    
}

bool EvaluateThisEvent(const InttRawHitStructure event){
  if ( event.n_intt_raw_hits == 0 ) {return false;} /*bad event*/
  if ( !event.is_MBDNS_tight_vtx10cm ) {return false;}
  return true; /* good event */
}


std::unordered_map< Chip, std::vector<LayerAndBco> > PrepareHitInfoByChip(const InttRawHitStructure& event) {
  std::unordered_map< Chip, std::vector<LayerAndBco> > output;

  for (int hit_index = 0; hit_index < event.n_intt_raw_hits ; hit_index++) {
    InttNameSpace::RawData_s rawdata;
    rawdata.felix_server = event.felix_server.at(hit_index) - 3001;
    rawdata.felix_channel = event.felix_channel.at(hit_index);
    rawdata.chip = (event.chip.at(hit_index) - 1) % 26;
    rawdata.channel = event.channel.at(hit_index);

    InttNameSpace::Offline_s offlinedata = InttNameSpace::ToOffline(rawdata);
    auto layer_of_the_hit = offlinedata.layer;
    if (layer_of_the_hit != 3 && layer_of_the_hit != 4 && layer_of_the_hit != 5 && layer_of_the_hit != 6){ 
      continue; 
    }

    auto fphx_bco_of_the_hit = event.FPHX_BCO.at(hit_index);

    // Chip hit_chip{event.felix_server.at(hit_index), event.felix_channel.at(hit_index), event.channel.at(hit_index)};
    Chip hit_chip{event.felix_server.at(hit_index), event.felix_channel.at(hit_index), event.chip.at(hit_index)};
    output[hit_chip].push_back(
      std::make_tuple( layer_of_the_hit, fphx_bco_of_the_hit )
    );
  }

  return output; 
}

std::map< HalfLadder, std::vector<LayerAndBco> > PrepareHitInfoByHalfladder(const InttRawHitStructure event) {
  std::map< HalfLadder, std::vector<LayerAndBco> > output; /* This will be copied to `event.hit_info_by_halfladder` */
  for (int hit_index = 0; hit_index < event.n_intt_raw_hits ; hit_index++) {
    InttNameSpace::RawData_s rawdata;
    rawdata.felix_server = event.felix_server.at(hit_index) - 3001;
    rawdata.felix_channel = event.felix_channel.at(hit_index);
    rawdata.chip = (event.chip.at(hit_index) - 1) % 26;
    rawdata.channel = event.channel.at(hit_index);

    InttNameSpace::Offline_s offlinedata = InttNameSpace::ToOffline(rawdata);
    auto layer_of_the_hit = offlinedata.layer;
    if (layer_of_the_hit != 3 && layer_of_the_hit != 4 && layer_of_the_hit != 5 && layer_of_the_hit != 6){ 
      continue; 
    }

    auto fphx_bco_of_the_hit = event.FPHX_BCO.at(hit_index);

    HalfLadder half_ladder{event.felix_server.at(hit_index), event.felix_channel.at(hit_index)};
    output[half_ladder].push_back(
      std::make_tuple( layer_of_the_hit, fphx_bco_of_the_hit )
    );
  }

  return output;
}

void CountCarriedOverHitsPerEventBasis(InttRawHitStructure event_of_interest, InttRawHitStructure& next_event){
  next_event.num_of_carried_hits = 0;
  next_event.num_of_carried_hits_inner=0;
  next_event.num_of_carried_hits_outer=0;
  
  if (next_event.n_intt_raw_hits == 0){ 
    return; 
  }

  
  // int full_bco_of_ith_event = event_of_interest.bco.at(0);
  // int full_bco_of_next_event = next_event.bco.at(0);
  // int event_bco_spacing = full_bco_of_next_event - full_bco_of_ith_event;
  // if ( event_bco_spacing > 126 ){
  //   return;
  // }

  if ( next_event.interval_from_previous > 126 ){
    return;
  }

  next_event.hit_info_by_halfladder = PrepareHitInfoByHalfladder(next_event);
  next_event.hit_info_by_chip = PrepareHitInfoByChip(next_event);

  // /* update if the interval is short ( 0 was misassigned for such event by mistake in most cases ) */ 
  // if ( 17 <= event_bco_spacing && event_bco_spacing <= 22) {
  //   event_of_interest.hitbco_peak = (next_event.hitbco_peak - event_bco_spacing + 128)%128;
  // }
  // // event_of_interest.hitbco_peak = (next_event.hitbco_peak - event_bco_spacing + 128)%128;

  int anticipated_timing = (next_event.hitbco_peak - next_event.interval_from_previous + 128)%128;
  /* calculate backward if no trigger timing hit was recorded in the previous event  */
  if ( event_of_interest.h1D_fphxbco->GetEntries() == 0 ) {
    event_of_interest.hitbco_peak = (next_event.hitbco_peak - next_event.interval_from_previous + 128)%128;
  }

  /* Count the number of carried over hits. */
  /* Each loop of the outer loop corresponds to each half ladder. */
  /* In the inner loop, carried-over hits are determined. */

  /* per-half-ladder basis analysis */
  for (auto key_value : next_event.hit_info_by_halfladder){
    HalfLadder halfladder = key_value.first; 
    // int n_hits_in_that_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.hitbco_peak);
    int n_hits_in_that_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder( PrepareHitInfoByHalfladder(event_of_interest), halfladder, event_of_interest.hitbco_peak);
    
    if ( n_hits_in_that_halfladder < 6.5*(next_event.interval_from_previous-25) ) {
      continue;
    }

    std::vector<LayerAndBco> pairs = key_value.second;
    for (int next_hit_i = 0; next_hit_i < int(pairs.size()); next_hit_i++) {
      
      LayerAndBco layer_bco = pairs[next_hit_i];
      int next_hit_layer = std::get<0>(layer_bco);
      int next_hit_bco = std::get<1>(layer_bco);

      int hit_bco_n1 = (anticipated_timing -1 + 128)%128;
      int hit_bco_p1 = (anticipated_timing +1 + 128)%128;
      if (next_hit_bco == anticipated_timing || next_hit_bco == hit_bco_n1 || next_hit_bco == hit_bco_p1) {
        if (next_hit_layer == 3 || next_hit_layer == 4) {next_event.num_of_carried_hits_inner++;}
        else if (next_hit_layer == 5 || next_hit_layer == 6) {next_event.num_of_carried_hits_outer++;}
        next_event.num_of_carried_hits++;
      } else { /* move to next halfladder after the first hit with a different hit bco than ith_event.hitbco_peak */
        // break; /* commented out to execute the criteria ver. 2 in the step 2 */
      }
    }
  }

  /* per-chip basis analysis */
  // for (auto key_value : next_event.hit_info_by_chip){
  //   Chip chip = key_value.first;
  //   std::vector<LayerAndBco> pairs = key_value.second;

  //   // int num_hits_in_that_chip = GetNumberOfHitsInNormalTimingInAChip(event_of_interest.hit_info_by_chip, chip, event_of_interest.hitbco_peak);
  //   int num_hits_in_that_chip = GetNumberOfHitsInNormalTimingInAChip(PrepareHitInfoByChip(event_of_interest), chip, event_of_interest.hitbco_peak);
  //   if ( num_hits_in_that_chip < 2*(next_event.interval_from_previous-25) ) {
  //     continue;
  //   }

  //   for (int next_hit_i = 0; next_hit_i < int(pairs.size()); next_hit_i++) {
      
  //     LayerAndBco layer_bco = pairs[next_hit_i];
  //     int next_hit_layer = std::get<0>(layer_bco);
  //     int next_hit_bco = std::get<1>(layer_bco);

  //     if (next_hit_bco == anticipated_timing || next_hit_bco == ((anticipated_timing -1+128)%128) || next_hit_bco == ((anticipated_timing +1+128)%128) ) {
  //       if (next_hit_layer == 3 || next_hit_layer == 4) {next_event.num_of_carried_hits_inner++;}
  //       else if (next_hit_layer == 5 || next_hit_layer == 6) {next_event.num_of_carried_hits_outer++;}
  //       next_event.num_of_carried_hits++;
  //     } else {
  //       //break; /* commented out to execute the criteria ver. 2 in the step 2 */
  //     }
  //   }
  // }
}

void InttHitCarryoverMonitor::CountCarriedOverHitsPerHalfladderBasis(InttRawHitStructure event_of_interest, InttRawHitStructure& next_event){
  
  if (next_event.n_intt_raw_hits == 0){ 
    return; 
  }

  // if (next_event.bco.size() == 0) {
  //   std::cout << " next_event.n_intt_raw_hits" << next_event.n_intt_raw_hits << std::endl;
  //   std::cout << " next_event.bco.size()" << next_event.bco.size() << std::endl;
  //   return;
  // }
  
  // int full_bco_of_ith_event = event_of_interest.bco.at(0);
  // int full_bco_of_next_event = next_event.bco.at(0);
  // int event_bco_spacing = full_bco_of_next_event - full_bco_of_ith_event;
  // if ( event_bco_spacing > 126 ){
  //   return;
  // }

  if ( next_event.interval_from_previous > 126 ){
    return;
  }


  next_event.hit_info_by_halfladder = PrepareHitInfoByHalfladder(next_event);



  /* update if the interval is short ( 0 was misassigned for such event by mistake in most cases ) */ 
  // if ( 17 <= event_bco_spacing && event_bco_spacing <= 126) {
  //   event_of_interest.hitbco_peak = (next_event.hitbco_peak - event_bco_spacing + 128)%128;
  // }
  int anticipated_timing = (next_event.hitbco_peak - next_event.interval_from_previous + 128)%128;
  /* (The method 2 in the step 1) calculate backward if no trigger-timing hit was recorded in the previous event  */
  if ( event_of_interest.h1D_fphxbco->GetEntries() == 0) {
    event_of_interest.hitbco_peak = (next_event.hitbco_peak - next_event.interval_from_previous + 128)%128;
  }

  /* Count the number of carried over hits. */
  /* Each loop of the outer loop corresponds to each half ladder. */
  /* In the inner loop, carried-over hits are determined. */
  int halfladder_id = 0;
  int n_carried_hits_by_halfladder;
  for (auto key_value : next_event.hit_info_by_halfladder){
    n_carried_hits_by_halfladder = 0;
    halfladder_id++;
    HalfLadder halfladder = key_value.first; 
    std::vector<LayerAndBco> pairs = key_value.second;

    // int n_hits_in_that_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.hitbco_peak);
    int n_hits_in_that_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(PrepareHitInfoByHalfladder(event_of_interest), halfladder, event_of_interest.hitbco_peak);
    // int n_hits_in_that_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.gl1_bco, trigger_BcoDiff_, intt_clock_offset_);
    // int n_hits_in_that_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder( PrepareHitInfoByHalfladder(event_of_interest), halfladder, event_of_interest.gl1_bco, trigger_BcoDiff_, intt_clock_offset_);
    h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_all_event_->Fill(next_event.interval_from_previous, n_hits_in_that_halfladder);

    if ( n_hits_in_that_halfladder < 6.5*(next_event.interval_from_previous-25) ) {
      continue;
    }


    for (int next_hit_i = 0; next_hit_i < int(pairs.size()); next_hit_i++) {
      
      LayerAndBco layer_bco = pairs[next_hit_i];
      // int next_hit_layer = std::get<0>(layer_bco); /* not used here */
      int next_hit_bco = std::get<1>(layer_bco);

      if (next_hit_bco == anticipated_timing || next_hit_bco == ((anticipated_timing-1+128)%128) || next_hit_bco == ((anticipated_timing+1+128)%128) ) {
        n_carried_hits_by_halfladder++;
      } else { 
        // if ( n_carried_hits_by_halfladder > 0 ){
        //   h2D_Halfladder_NCOH_->Fill(halfladder_id,n_carried_hits_by_halfladder);
        // }
        // if ( n_carried_hits_by_halfladder > 0 ) {

        //   // int number_of_hit_in_the_halfladder = GetNumberOfHitsInAChip_HalfladderbyHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.hitbco_peak);
        //   int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.hitbco_peak);
        //   h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_->Fill(next_event.interval_from_previous, number_of_hit_in_the_halfladder);
          // if (number_of_hit_in_the_halfladder>0) {
          //   h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_->Fill(event_bco_spacing, number_of_hit_in_the_halfladder);
          // }

          // auto iterator = event_of_interest.hit_info_by_halfladder.find(halfladder); //temp_info_by_half_ladder_forithevent.find(halfladder);
          // if ( iterator != event_of_interest.hit_info_by_halfladder.end() ) {
          //   //HalfLadder half_ith = iterator->first; /* not used here */
          //   std::vector<LayerAndBco> pairs = iterator->second;
          //   int number_of_hit_in_the_halfladder = pairs.size();
            
          //   h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_->Fill(event_bco_spacing, number_of_hit_in_the_halfladder);

          //   int layer = std::get<0>(layer_bco); /* derived from `next_event`, but it's the same halfladder/layer as that of previous event, since */
          //   if ( layer == 3 || layer == 4 ) {h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner_->Fill(event_bco_spacing,number_of_hit_in_the_halfladder);}
          //   if ( layer == 5 || layer == 6 ) {h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer_->Fill(event_bco_spacing,number_of_hit_in_the_halfladder);}
          // }
        } /* move to next halfladder after the first hit with a different hit bco than ith_event.hitbco_peak */
        // break; /* commented out to execute the criteria ver. 2 in the step 2 */
      }

    if ( n_carried_hits_by_halfladder > 0 ){
      h2D_Halfladder_NCOH_->Fill(halfladder_id,n_carried_hits_by_halfladder);
      // int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.hitbco_peak);
      int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(PrepareHitInfoByHalfladder(event_of_interest), halfladder, event_of_interest.hitbco_peak);
      // int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.gl1_bco, trigger_BcoDiff_, intt_clock_offset_);
      // int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(PrepareHitInfoByHalfladder(event_of_interest), halfladder, event_of_interest.gl1_bco, trigger_BcoDiff_, intt_clock_offset_);
      h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_->Fill(next_event.interval_from_previous, number_of_hit_in_the_halfladder);
    }
    
    /* temp check */
    if ( my_event_counter_ == 1428 ) {
      // int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(event_of_interest.hit_info_by_halfladder, halfladder, event_of_interest.gl1_bco, trigger_BcoDiff_, intt_clock_offset_);
      int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(PrepareHitInfoByHalfladder(event_of_interest), halfladder, event_of_interest.hitbco_peak);
      // int number_of_hit_in_the_halfladder = GetNumberOfHitsInNormalTimingInAHalfladder(PrepareHitInfoByHalfladder(event_of_interest), halfladder, event_of_interest.gl1_bco, trigger_BcoDiff_, intt_clock_offset_);
      for (int i=0; i<number_of_hit_in_the_halfladder; i++) {
        h1D_Halfladder_NHitInPreviousEvent_example_->Fill(halfladder_id);
      }
      for (int i=0; i<n_carried_hits_by_halfladder; i++) {
        h1D_Halfladder_NCOH_example_->Fill(halfladder_id);
       }
     // std::cout << "Saved an event for a check" << std::endl;
    }
  }
}

void InttHitCarryoverMonitor::CountCarriedOverHitsPerChipBasis(InttRawHitStructure event_of_interest, InttRawHitStructure& next_event){
  
  if (next_event.n_intt_raw_hits == 0){ 
    return; 
  }

  /* Can be omitted. Probably... */
  // if (next_event.bco.size() == 0) {
  //   std::cout << " next_event.n_intt_raw_hits" << next_event.n_intt_raw_hits << std::endl;
  //   std::cout << " next_event.bco.size()" << next_event.bco.size() << std::endl;
  //   return;
  // }
  
  // int full_bco_of_ith_event = event_of_interest.bco.at(0);
  // int full_bco_of_next_event = next_event.bco.at(0);
  // int event_bco_spacing = full_bco_of_next_event - full_bco_of_ith_event;
  // if ( event_bco_spacing > 126 ){
  //   return;
  // }
  if ( next_event.interval_from_previous > 126 ){
    return;
  }

  /* `ith_event.hit_info_by_chip` shoud be filled in the previous event loop. */
  next_event.hit_info_by_chip = PrepareHitInfoByChip(next_event);




  int anticipated_timing = (next_event.hitbco_peak - next_event.interval_from_previous + 128)%128;
  /* calculate backward if no trigger timing hit was recorded in the previous event  */
  if ( event_of_interest.h1D_fphxbco->GetEntries() == 0 ) {
    event_of_interest.hitbco_peak = (next_event.hitbco_peak - next_event.interval_from_previous + 128)%128;
  }

  /* Count the number of carried over hits. */
  /* Each loop of the outer loop corresponds to each half ladder. */
  /* In the inner loop, carried-over hits are determined. */
  int n_carried_hits_by_chip{0};
  for (auto key_value : next_event.hit_info_by_chip) {
    Chip chip = key_value.first;
    std::vector<LayerAndBco> pairs = key_value.second;

    // int num_hits_in_that_chip = GetNumberOfHitsInNormalTimingInAChip(event_of_interest.hit_info_by_chip, chip, event_of_interest.hitbco_peak);
    int num_hits_in_that_chip = GetNumberOfHitsInNormalTimingInAChip(PrepareHitInfoByChip(event_of_interest), chip, event_of_interest.hitbco_peak);
    h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_all_event_->Fill(next_event.interval_from_previous, num_hits_in_that_chip);
    // if ( num_hits_in_that_chip < 2*(next_event.interval_from_previous-25) ) {
    //   continue;
    // }


    for (int next_hit_i = 0; next_hit_i < int(pairs.size()); next_hit_i++) {
      
      LayerAndBco layer_bco = pairs[next_hit_i];
      // int next_hit_layer = std::get<0>(layer_bco); /* not used here */
      int next_hit_bco = std::get<1>(layer_bco);

      if (next_hit_bco == anticipated_timing || next_hit_bco == ((anticipated_timing -1+128)%128) || next_hit_bco == ((anticipated_timing +1+128)%128) ) {
        n_carried_hits_by_chip++;
      } else {
        // if ( n_carried_hits_by_chip > 0 ) {

        //   /* count the number of hits in that chip recorded in i-th hit  */
        //   int num_hits_in_that_chip = GetNumberOfHitsInNormalTimingInAChip(event_of_interest.hit_info_by_chip, chip, event_of_interest.hitbco_peak);
        //   h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_->Fill(event_bco_spacing, num_hits_in_that_chip);
          // if (num_hits_in_that_chip > 0) {
          //   h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_->Fill(event_bco_spacing, num_hits_in_that_chip);
          // }

          // int number_of_hit_in_the_chip;
          // auto iterator = event_of_interest.hit_info_by_chip.find(chip);
          // if ( iterator != event_of_interest.hit_info_by_chip.end() ) {
          //   //Chip chip_ith = iterator->first; /* not used here */
          //   std::vector<LayerAndBco> pairs = iterator->second;
          //   number_of_hit_in_the_chip = pairs.size();

          //   h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_->Fill(event_bco_spacing, number_of_hit_in_the_chip);
          // }
        // } /* move to next chip after the first hit with a different hit bco from ith_event.hitbco_peak */
        // break; /* commented out to execute the criteria ver. 2 in the step 2 */
      }
    }
    if ( n_carried_hits_by_chip > 0 ) {

          /* count the number of hits in that chip recorded in i-th hit  */
          // int num_hits_in_that_chip = GetNumberOfHitsInNormalTimingInAChip(event_of_interest.hit_info_by_chip, chip, event_of_interest.hitbco_peak);
          int num_hits_in_that_chip = GetNumberOfHitsInNormalTimingInAChip(PrepareHitInfoByChip(event_of_interest), chip, event_of_interest.hitbco_peak);
          h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_->Fill(next_event.interval_from_previous, num_hits_in_that_chip);
    }
  }
}


void InttHitCarryoverMonitor::FillHistograms() {

  if (!previous_event_.is_MBDNS_tight_vtx10cm) {
    return;
  }
  // if (previous_event_.n_intt_raw_hits == 0) {
  //   return;
  // }

  /* This was the cause of the distinction at interval=22*/
  // if ( previous_event_.bco_diff_peak != trigger_BcoDiff_ && previous_event_.bco_diff_peak != (trigger_BcoDiff_-1+128)%128 && previous_event_.bco_diff_peak != (trigger_BcoDiff_+1+128)%128) {
  //   return;
  // }

  h2D_NInttHitInner_NInttHitOuter_->Fill(previous_event_.n_hit_inner, previous_event_.n_hit_outer);

  if (ith_event_.n_intt_raw_hits == 0) {
    h2D_NInttHitInner_NInttHitOuter_corrected_->Fill(previous_event_.n_hit_inner, 
                                               previous_event_.n_hit_outer);
  } else if (ith_event_.num_of_carried_hits > 0){
    h2D_NInttHitInner_NInttHitOuter_corrected_->Fill(previous_event_.n_hit_inner + ith_event_.num_of_carried_hits_inner, 
                                               previous_event_.n_hit_outer + ith_event_.num_of_carried_hits_outer);
  } else {
    h2D_NInttHitInner_NInttHitOuter_corrected_->Fill(previous_event_.n_hit_inner ,
                                               previous_event_.n_hit_outer );
  }

  if ( ith_event_.n_intt_raw_hits > 0 ) {
    // int full_bco_of_ith_event = previous_event_.bco.at(0);
    // int full_bco_of_next_event = ith_event_.bco.at(0);
    // int event_bco_spacing = full_bco_of_next_event - full_bco_of_ith_event;
    int full_bco_of_next_event = ith_event_.gl1_bco-intt_clock_offset_;
    int event_bco_spacing = ith_event_.interval_from_previous;
    h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_->Fill(event_bco_spacing, previous_event_.n_hit_inner+previous_event_.n_hit_outer);
    h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_->Fill(event_bco_spacing, previous_event_.n_hit_inner + previous_event_.n_hit_outer + ith_event_.num_of_carried_hits);
    

    if (ith_event_.num_of_carried_hits > 0){
      if ( event_bco_spacing>127 ){ return; }
      // h2D_EventBcoSpacing_NCarriedHits_->Fill(event_bco_spacing, ith_event_.num_of_carried_hits);

      h2D_NCarriedInner_NCarriedOuter_->Fill( ith_event_.num_of_carried_hits_inner,  ith_event_.num_of_carried_hits_outer);
    
      h2D_EventBcoSpacing_NHitInPreviousEvent_->Fill(event_bco_spacing, previous_event_.n_hit_inner+previous_event_.n_hit_outer);
      h2D_EventBcoSpacing_NCohInNextEvent_->Fill(event_bco_spacing, ith_event_.num_of_carried_hits);
      //int misasigned_bco_diff = (previous_event_.hitbco_peak - ith_event_.hitbco_peak +1 + 128 ) % 128; 
      int misasigned_bco_diff = (previous_event_.hitbco_peak - ((full_bco_of_next_event)&(0x7fU)) + 128 - trigger_BcoDiff_ ) % 128; 
      h2D_MisasignedBcoDiff_NHitInPreviousEvent_->Fill(misasigned_bco_diff, previous_event_.n_hit_inner+previous_event_.n_hit_outer);
      h2D_MisasignedBcoDiff_NCohInNextEvent_->Fill(misasigned_bco_diff, ith_event_.num_of_carried_hits);
  
      h2D_EventBcoSpacing_CorrectedMultiplicity_->Fill(event_bco_spacing, previous_event_.n_hit_inner + previous_event_.n_hit_outer + ith_event_.num_of_carried_hits);
    }
  }    
}

int DeriveTriggerBcoDiffOfARun(int run){
  std::map<int, int> run_to_bcodiff;

  run_to_bcodiff.insert( std::pair<int,int>(68007, 89));
  run_to_bcodiff.insert( std::pair<int,int>(68008, 81));
  run_to_bcodiff.insert( std::pair<int,int>(68009, 17));
  run_to_bcodiff.insert( std::pair<int,int>(68010, 25));
  run_to_bcodiff.insert( std::pair<int,int>(68012, 89));
  run_to_bcodiff.insert( std::pair<int,int>(68013, 49));
  run_to_bcodiff.insert( std::pair<int,int>(68014, 1));
  run_to_bcodiff.insert( std::pair<int,int>(68015, 81));
  run_to_bcodiff.insert( std::pair<int,int>(66522, 97));
  run_to_bcodiff.insert( std::pair<int,int>(66682, 100));

  run_to_bcodiff.insert( std::pair<int,int>(68002, 33));
  run_to_bcodiff.insert( std::pair<int,int>(69404, 44));
  run_to_bcodiff.insert( std::pair<int,int>(71251, 9));
  run_to_bcodiff.insert( std::pair<int,int>(71345, 118));
  run_to_bcodiff.insert( std::pair<int,int>(71346, 94));
  run_to_bcodiff.insert( std::pair<int,int>(71347, 110));
  run_to_bcodiff.insert( std::pair<int,int>(71391, 80));
  run_to_bcodiff.insert( std::pair<int,int>(71456, 23));
  run_to_bcodiff.insert( std::pair<int,int>(75519, 11));
  run_to_bcodiff.insert( std::pair<int,int>(75535, 31));
  run_to_bcodiff.insert( std::pair<int,int>(75536, 103));
  
  run_to_bcodiff.insert( std::pair<int,int>(79244, 1));
  run_to_bcodiff.insert( std::pair<int,int>(79252, 40));
  run_to_bcodiff.insert( std::pair<int,int>(79261, 105));
  run_to_bcodiff.insert( std::pair<int,int>(79263, 16));
  run_to_bcodiff.insert( std::pair<int,int>(79264, 49));
  run_to_bcodiff.insert( std::pair<int,int>(79265, 121));
  run_to_bcodiff.insert( std::pair<int,int>(79266, 65));

  run_to_bcodiff.insert( std::pair<int,int>(82400, 71));

  if (run_to_bcodiff.contains(run)) {
    return run_to_bcodiff.at(run);
  } else {
    std::cerr << "ERROR!!! Run " << run << " does not exist in the dictionary of DeriveTriggerBcoDiffOfARun(run)" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

// int InttHitCarryoverMonitor::DeriveTriggerBcoDiff(){
//   ith_event_
// }

void InttHitCarryoverMonitor::PushBackNarrowSpacingCarryover(){
  // int full_bco_of_ith_event = previous_event_.bco.at(0);
  // int full_bco_of_next_event = ith_event_.bco.at(0);
  // int event_bco_spacing = full_bco_of_next_event - full_bco_of_ith_event;
  int full_bco_of_next_event = ith_event_.gl1_bco-intt_clock_offset_;
  int event_bco_spacing = ith_event_.interval_from_previous;
  if ( event_bco_spacing>22 ) {return;}
  if ( ith_event_.n_intt_raw_hits == 0 ) {return;}
  if ( 17 <= event_bco_spacing && event_bco_spacing <= 22){
    int n_hits_maybe_carried_inner(0);
    int n_hits_maybe_carried_outer(0);
    int misassigned_bco(0);
    for (int hit_index=0; hit_index<ith_event_.n_intt_raw_hits; hit_index++) {
      misassigned_bco = ( ith_event_.FPHX_BCO.at(hit_index) - ((full_bco_of_next_event)&(0x7fU)) +128)%128;
      if (misassigned_bco == (trigger_BcoDiff_-event_bco_spacing+128)%128 || misassigned_bco == (trigger_BcoDiff_-event_bco_spacing-1+128)%128 || misassigned_bco == (trigger_BcoDiff_-event_bco_spacing+1+128)%128 ){
        // std::cout << "event_bco_spacing: "<<event_bco_spacing <<", misassigned_bco: " <<misassigned_bco << std::endl;
        InttNameSpace::RawData_s rawdata;
        rawdata.felix_server = ith_event_.felix_server.at(hit_index) - 3001;
        rawdata.felix_channel = ith_event_.felix_channel.at(hit_index);
        rawdata.chip = (ith_event_.chip.at(hit_index) - 1) % 26;
        rawdata.channel = ith_event_.channel.at(hit_index);

        InttNameSpace::Offline_s offlinedata = InttNameSpace::ToOffline(rawdata);
        if (offlinedata.layer == 3 || offlinedata.layer == 4) { n_hits_maybe_carried_inner++; }
        else if (offlinedata.layer == 5 || offlinedata.layer == 6) { n_hits_maybe_carried_outer++; }
      }
    }
    
    h2D_NInttHitInner_NInttHitOuter_narrow_spacing_->Fill(n_hits_maybe_carried_inner, n_hits_maybe_carried_outer);
  }
}

void InttHitCarryoverMonitor::CheckMultiplicity() {
  // int full_bco_of_ith_event = previous_event_.bco.at(0);
  // int full_bco_of_next_event = ith_event_.bco.at(0);
  // int event_bco_spacing = full_bco_of_next_event - full_bco_of_ith_event;
  int event_bco_spacing = ith_event_.interval_from_previous;
  if ( event_bco_spacing > 80 ) {
    h1D_Multiplicity_long_event_interval_->Fill(previous_event_.n_hit_inner + previous_event_.n_hit_outer);
  }
  if ( 17 <= event_bco_spacing && event_bco_spacing <= 80) {
    h1D_Multiplicity_short_event_interval_->Fill(previous_event_.n_hit_inner + previous_event_.n_hit_outer);
    h1D_Multiplicity_short_event_interval_corrected_->Fill(previous_event_.n_hit_inner + previous_event_.n_hit_outer + ith_event_.num_of_carried_hits);
  }
}


int GetNumberOfHitsInNormalTimingInAHalfladder(const std::map< HalfLadder, std::vector<LayerAndBco> > list, const HalfLadder halfladder, const int standard_timing){
  int n_hits_in_that_halfladder{0};
  auto iterator_previous_event = list.find(halfladder);
  if ( iterator_previous_event != list.end() ) {
    std::vector<LayerAndBco> pairs_previous_event = iterator_previous_event->second;
    
    for (int hit_index = 0; hit_index < int(pairs_previous_event.size()); hit_index++) {
      int previous_hit_bco = std::get<1>(pairs_previous_event[hit_index]);
      if ( previous_hit_bco == standard_timing || previous_hit_bco == ((standard_timing -1+128)%128) || previous_hit_bco == ((standard_timing +1+128)%128) ) {
        n_hits_in_that_halfladder++;
      }
    }
  }
  return n_hits_in_that_halfladder;
}

int GetNumberOfHitsInNormalTimingInAHalfladder(const std::map< HalfLadder, std::vector<LayerAndBco> > list, const HalfLadder halfladder, const int gl1_bco, const int triggered_BcoDiff, const int64_t intt_clock_offset){
  int n_hits_in_that_halfladder{0};
  auto iterator_previous_event = list.find(halfladder);
  if ( iterator_previous_event != list.end() ) {
    std::vector<LayerAndBco> pairs_previous_event = iterator_previous_event->second;
    
    for (int hit_index = 0; hit_index < int(pairs_previous_event.size()); hit_index++) {
      int previous_hit_bco = std::get<1>(pairs_previous_event[hit_index]);
      int previous_hit_bcodiff = (previous_hit_bco - ((gl1_bco - intt_clock_offset)&0x7fU) +128)%128;
      if ( previous_hit_bcodiff == triggered_BcoDiff || previous_hit_bcodiff == ((triggered_BcoDiff -1+128)%128) || previous_hit_bcodiff == ((triggered_BcoDiff +1+128)%128) ) {
        n_hits_in_that_halfladder++;
      }
    }
  }
  return n_hits_in_that_halfladder;
}

int GetNumberOfHitsInAChip_HalfladderbyHalfladder(const std::map< HalfLadder, std::vector<LayerAndBco> > list, const HalfLadder halfladder, const int standard_timing){
  int n_hits_in_that_halfladder{0};
  auto iterator_previous_event = list.find(halfladder);
  if ( iterator_previous_event != list.end() ) {
    std::vector<LayerAndBco> pairs_previous_event = iterator_previous_event->second;
    n_hits_in_that_halfladder = pairs_previous_event.size();
  }
  return n_hits_in_that_halfladder;
}

int GetNumberOfHitsInNormalTimingInAChip(const std::unordered_map< Chip, std::vector<LayerAndBco> > list, const Chip chip, const int standard_timing){
  int n_hits_in_that_chip{0};
  auto iterator_previous_event = list.find(chip);
  if ( iterator_previous_event != list.end() ) {
    std::vector<LayerAndBco> pairs_previous_event = iterator_previous_event->second;
    
    for (int hit_index = 0; hit_index < int(pairs_previous_event.size()); hit_index++) {
      int previous_hit_bco = std::get<1>(pairs_previous_event[hit_index]);
      if ( previous_hit_bco == standard_timing || previous_hit_bco == ((standard_timing -1+128)%128) || previous_hit_bco == ((standard_timing +1+128)%128) ) {
        n_hits_in_that_chip++;
      }
    }
  }
  return n_hits_in_that_chip;
}

int GetNumberOfHitsInAChip_ChipByChip(const std::unordered_map< Chip, std::vector<LayerAndBco> > list, const Chip chip, const int standard_timing){
  int n_hits_in_that_chip{0};
  auto iterator_previous_event = list.find(chip);
  if ( iterator_previous_event != list.end() ) {
    std::vector<LayerAndBco> pairs_previous_event = iterator_previous_event->second;
    n_hits_in_that_chip = pairs_previous_event.size();
  }
  return n_hits_in_that_chip;
}

