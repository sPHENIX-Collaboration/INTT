// Fun4All analysis module for INTT hit carryover monitor, made by Ryotaro Koike 20250808
#ifndef INTT_HITCARRYOVER_MONITOR_H_20250808
#define INTT_HITCARRYOVER_MONITOR_H_20250808

#include <vector>
#include <unordered_map>
#include <set>
#include <bitset>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>


#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainerv2.h>
#include <ffarawobjects/Gl1Packetv3.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>
#include <phool/getClass.h>

#include <intt/InttMapping.h>

using HalfLadder = std::pair<uint32_t, uint16_t>; /* <packet_id, fee> */
using LayerAndBco = std::tuple<int, int>; /* <layer, bco>, which is denied for each hit> */

using Chip = std::tuple<uint32_t, uint16_t, int>; /* <packet_id, fee, channel> */
/* Below is an implementation of hash function. */ 
/* This is needed to use the `Chip` type as a key of `std::unorderd_map`. */
/* The use of `std::unorderd_map` makes the execution of this module much fast.*/
namespace std {
  template <>
  struct hash<Chip> {
    std::size_t operator()(const Chip& chip) const {
      std::size_t h1 = std::hash<uint32_t>{}(std::get<0>(chip));
      std::size_t h2 = std::hash<uint16_t>{}(std::get<1>(chip));
      std::size_t h3 = std::hash<int>{}(std::get<2>(chip));

      // ハッシュ値の合成（Boost 推奨のやり方に近い）
      std::size_t seed = h1;
      seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      return seed;
    }
  };
}






/* helper structure */
struct InttRawHitStructure {
  uint64_t gl1_bco; /* have value even when INTT has no hit (unlike the std::vector<uint64_t> bco) */
  int64_t interval_from_previous; 

  /* values that will be set by tree->GetEntry(i) */
  int n_intt_raw_hits = 0;
  std::vector<uint64_t> bco;
  std::vector<uint32_t> felix_server; // packetid;
  std::vector<uint32_t> word;
  std::vector<uint16_t> felix_channel; // fee;
  std::vector<uint16_t> channel; // channel_id;
  std::vector<uint16_t> chip; // chip_id;
  std::vector<uint16_t> adc;
  std::vector<uint16_t> FPHX_BCO;
  std::vector<uint16_t> full_FPHX;
  std::vector<uint16_t> full_ROC;
  std::vector<uint16_t> amplitude;
  bool is_MBDNS_tight_inclusive = false;
  bool is_MBDNS_tight_vtx10cm = false;
  bool is_MBDNS_tight_vtx30cm = false;

  /* values below will be derived from values above */
  int hitbco_peak = -999;
  int bco_diff_peak = -999;
  int n_hit_inner = 0;
  int n_hit_outer = 0;
  int num_of_carried_hits = 0; /* carried-over hits from the previous event */
  int num_of_carried_hits_inner = 0;
  int num_of_carried_hits_outer = 0;
  TH1D* h1D_fphxbco = new TH1D(Form("evt_this_INTTHitBco_%p", this), "evt_this_INTTHitBco;INTT BCO", 128, -0.5, 127.5);

  /* This must be `map`, not `unordered_map`, so that halfladder_id calculation makes sense. */
  std::map< HalfLadder, std::vector<LayerAndBco> > hit_info_by_halfladder; 
  /* This is `unordered_map` to have the excution time short. (Search of an element takes time.) */
  std::unordered_map< Chip, std::vector<LayerAndBco> > hit_info_by_chip;
  std::set< std::tuple<int, int, int, int, int, int, int> > hit_already_seen; /* for clone hit detection*/
  // std::set< std::tuple<int, int, int, int, int, int> > hit_already_seen; /* for clone hit detection*/
  // std::set< std::tuple<int, int, int, int, int> > hit_already_seen; /* for clone hit detection*/

};



class InttHitCarryoverMonitor : public SubsysReco {
  public:
    // InttHitCarryoverMonitor(const std::string &name, int run);
    InttHitCarryoverMonitor(const std::string &name, const int run, const std::string arg_file_path_to_hot_channel_map, std::string job_index, bool save_tree);
    ~InttHitCarryoverMonitor();
    
    // mandatory methods for Fun4All analysis module
    int Init(PHCompositeNode *topNode) override;
    int InitRun(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int ResetEvent(PHCompositeNode *topNode) override;
    int Reset(PHCompositeNode *topNode) override;
    int EndRun(const int runnumber) override;
    int End(PHCompositeNode *topNode) override;
    // void Print(const std::string &what) const override;



  private:
    bool save_tree_;
    TTree* output_tree_;
    
    int my_event_counter_;
    int trigger_BcoDiff_;

    uint64_t gl1_bco_previous_;
    // uint64_t gl1_bco_ith_;
    // int64_t interval_from_previous_;
    int64_t intt_clock_offset_;
    
    InttRawHitContainer* container_ith_event_;

    /* This structure is free from hot channels and clone hits. */
    InttRawHitStructure previous_event_; 
    InttRawHitStructure ith_event_;

    std::string file_path_to_hot_channel_map_;
    std::map<std::string,std::string> hot_channel_map_;

    TFile* output_file_;
    TH2D* h2D_NInttHitInner_NInttHitOuter_;
    TH2D* h2D_NInttHitInner_NInttHitOuter_corrected_;
    // TH2D* h2D_EventBcoSpacing_NCarriedHits_;
    TH2D* h2D_NCarriedInner_NCarriedOuter_;
    TH2D* h2D_Halfladder_NCOH_;

    TH1D* h1D_Halfladder_NHitInPreviousEvent_example_;
    TH1D* h1D_Halfladder_NCOH_example_;
  

    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_all_event_;
    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_all_event_;
    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_all_event_;

    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_;
    TH2D* h2D_EventBcoSpacing_NCohInNextEvent_; 
    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_;
    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_bychip_;
    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_inner_; 
    TH2D* h2D_EventBcoSpacing_NHitInPreviousEvent_byhalfladder_outer_; 

    TH2D* h2D_MisasignedBcoDiff_NHitInPreviousEvent_;
    TH2D* h2D_MisasignedBcoDiff_NCohInNextEvent_;

    TH2D* h2D_NInttHitInner_NInttHitOuter_narrow_spacing_;
    
    TH1D* h1D_Multiplicity_long_event_interval_;
    TH1D* h1D_Multiplicity_short_event_interval_;
    TH1D* h1D_Multiplicity_short_event_interval_corrected_;

    TH2D* h2D_EventBcoSpacing_CorrectedMultiplicity_;
    TH2D* h2D_EventBcoSpacing_CorrectedMultiplicity_all_event_;
    

    /* trigger_table */
    static const int MBDNS_loose_inclusive_ = 11; /* MBD N&S >= 1 */
    static const int MBDNS_tight_inclusive_ = 10; /* MBD N&S >= 2 */ 
    static const int MBDNS_tight_vtx10cm_   = 12; /* MBD N&S >= 2 && vtx < 10 cm */
    static const int MBDNS_tight_vtx30cm_   = 13; /* MBD N&S >= 2 && vtx < 30 cm */
    static const int MBDNS_tight_vtx60cm_   = 14; /* MBD N&S >= 2 && vtx < 60 cm */

    std::string node_name_intt_raw_hit_{"INTTRAWHIT"};
    std::string node_name_gl1_raw_hit_{"GL1RAWHIT"};
    std::string tree_name_hot_channel_{"Multiple"};

    void PrepareHotChannelMap();
    void FillAHit(const InttRawHit* const hit);
    // InttRawHitStructure ConvertFromRawHit(InttRawHitContainer* container, const std::map<std::string,std::string> hot_channel_map);
    void ConvertFromRawHit(InttRawHitContainer* container);//, const std::map<std::string,std::string> hot_channel_map);
    void FillHistograms();
    void GetTriggerInformation(PHCompositeNode *topNode, InttRawHitStructure& event);
    std::vector<int> PrepareTriggerVector(long long trigger_input);
    void CountCarriedOverHitsPerHalfladderBasis(InttRawHitStructure event_of_interest, InttRawHitStructure& next_event);
    void CountCarriedOverHitsPerChipBasis(InttRawHitStructure event_of_interest, InttRawHitStructure& next_event);
    
    void PushBackNarrowSpacingCarryover();

    void CheckMultiplicity();


};




#endif // INTT_HITCARRYOVER_MONITOR_H_20250808