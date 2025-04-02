// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTRAWHITSANITYCHECK_H
#define INTTRAWHITSANITYCHECK_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>   // for PHIODataNode
#include <phool/PHNodeIterator.h> // for PHNodeIterator
#include <phool/PHObject.h>       // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/recoConsts.h>

#include <ffarawobjects/Gl1RawHit.h>
#include <ffarawobjects/Gl1Packetv2.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <intt/InttMap.h>
#include <intt/InttFeeMap.h>

#include <TMath.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <THStack.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <numeric>

class PHCompositeNode;
class Gl1Packetv2;
class InttRawHitContainer;

class TH1;
class TTree;
class TCanvas;

class INTTRawHitSanityCheck : public SubsysReco
{
 public:

    INTTRawHitSanityCheck(
        const std::string &name = "INTTRawHitSanityCheck",
        const int process_id_in = 0,
        const int runnumber_in = 0,
        const std::string output_directory_in = "no_directory",
        
        const std::string hotchannel_full_directory_in = "no_directory",
        const int triggered_BcoDiff_in = 55, // note : -1 for streaming (maybe)
        const bool apply_hotchannel_in = true,
        const bool clone_hit_remove_BCO_in = true,
        const bool hitQA_in = true,
        const bool check_clone_hit_in = true,

        const bool HaveGL1_in = true,
        const bool Get_InttRawHit_ntuple_in = true,
        const bool CountHitsBack_in = true,

        const std::vector<int> adc_conversion_vec_in = {35, 45, 60, 90, 120, 150, 180, 210}
    );

    ~INTTRawHitSanityCheck() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
    */
    int Init(PHCompositeNode *topNode) override;

    /** Called for first event when run number is known.
        Typically this is where you may want to fetch data from
        database, because you know the run number. A place
        to book histograms which have to know the run number.
    */
    int InitRun(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
    */
    int process_event(PHCompositeNode *topNode) override;

    /// Clean up internals after each event.
    int ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of each run.
    int EndRun(const int runnumber) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    /// Reset
    int Reset(PHCompositeNode * /*topNode*/) override;

    void Print(const std::string &what = "ALL") const override;

    std::string GetOutputFileName() { return output_file_name; }

 private:
    struct inttHitstr{
        int hit_server;
        int hit_felix_ch;
        int hit_chip;
        int hit_channel;
        int hit_adc;
        int hit_bco;
    };

    std::vector<int> prepare_trigger_vec(long long trigger_input);
    std::map<int,int> prepare_trigger_map(std::vector<int> trigger_vec_input);
    int PrepareGL1(PHCompositeNode *topNode);
    int PrepareINTT(PHCompositeNode *topNode);
    int PrepareHotChannel();
    int PrepareOutFile();
    std::vector<int> GetHistBinEntry(TH1D * hist);
    int PrepareHistInfo(TH1D * hist);
    int PrepareCloneHitInfo();

    int process_id;
    int runnumber;
    std::string output_directory;
    std::string hotchannel_full_directory;
    int triggered_BcoDiff;
    bool apply_hotchannel_tag;
    bool clone_hit_remove_BCO_tag;
    bool hitQA_tag;
    bool check_clone_hit_tag;
    bool HaveGL1_tag;
    bool Get_InttRawHit_ntuple;
    bool CountHitsBack;
    std::vector<int> adc_conversion_vec;

    long long eID_count;
    std::map<std::string, inttHitstr> evt_inttHits_map;
    std::map<std::string, inttHitstr> evt_inttHits_BCO_map;
    static const int nFELIX = 8;
    static const int nFELIXch = 14;
    static const int nChip = 26;
    static const int nChannel = 128;
    int evt_clone_hit_count[nFELIX][nFELIXch][nChip][nChannel];
    // vector<inttHitstr> evt_ladder_clone_hit[8][14];
    // map<string, vector<inttHitstr> evt_clone_hit_map; 

    std::string output_file_name;

    std::string m_InttRawNodeName = "INTTRAWHIT";
    InttRawHitContainer* inttcont;
    Gl1Packet* gl1;

    // uint64_t gl1rawhitbco;
    long long live_trigger_decimal;
    // std::vector<int> live_trigger_vec;
    std::map<int,int> live_trigger_map;

    std::vector<TH1D *> evt_felix_BcoDiffHist_vec;
    std::vector<TH1D *> all_felix_BcoDiffHist_vec;

    // note : -------------------------------- Hot channel branch ------------------------------
    TFile * hot_file_in;
    TTree * hot_tree_in;
    std::string hot_tree_name = "Multiple";
    int IID;             
    int Ichannel;        
    int Ichip;           
    int Ifelix_channel;  
    int Ifelix_server;   
    int Iflag;           

    std::map<std::string,std::string> hot_channel_map;

    // note : -------------------------------- INTTRawHit ntuples --------------------------------
    void GetInttRawHitInfo(PHCompositeNode *topNode);
    TTree* tree_out_RawHit;

    // InttRawHit information
    int NInttRawHits_;
    std::vector<uint64_t> InttRawHit_bco_;
    std::vector<uint32_t> InttRawHit_packetid_;
    std::vector<uint32_t> InttRawHit_word_;
    std::vector<uint16_t> InttRawHit_fee_;
    std::vector<uint16_t> InttRawHit_channel_id_;
    std::vector<uint16_t> InttRawHit_chip_id_;
    std::vector<uint16_t> InttRawHit_adc_;
    std::vector<uint16_t> InttRawHit_FPHX_BCO_;
    std::vector<uint16_t> InttRawHit_full_FPHX_;
    std::vector<uint16_t> InttRawHit_full_ROC_;
    std::vector<uint16_t> InttRawHit_amplitude_;

    // note : -------------------------------- tree out branch --------------------------------
    TFile* file_out;
    TTree* tree_out;

    int out_MBDNS_tight_inclusive;
    int out_MBDNS_tight_vtx10cm;
    int out_MBDNS_tight_vtx30cm;
    
    long long out_total_nHit;
    long long out_total_nHit_post; // note : post the hot channel cut, clone hit remove, if applied
    long long out_GL1_bco;
    long long out_INTT_bcofull;
    
    std::vector<int> out_spikeM1_location; // note : spike - 1
    std::vector<int> out_spikeM1_height;
    std::vector<int> out_spikeM2_location; // note : spike - 2
    std::vector<int> out_spikeM2_height;
    std::vector<int> out_spikeM3_location; // note : spike - 3
    std::vector<int> out_spikeM3_height;

    std::vector<int> out_spikeP1_location; // note : spike + 1
    std::vector<int> out_spikeP1_height;
    std::vector<int> out_spikeP2_location; // note : spike + 2
    std::vector<int> out_spikeP2_height;
    std::vector<int> out_spikeP3_location; // note : spike + 3
    std::vector<int> out_spikeP3_height;

    std::vector<int> out_HeightFirst_location; // note : First highest
    std::vector<int> out_HeightFirst_height;
    std::vector<int> out_HeightSecond_location; // note : second highest
    std::vector<int> out_HeightSecond_height;
    std::vector<int> out_HeightThird_location; // note : third highest
    std::vector<int> out_HeightThird_height;
    std::vector<int> out_HeightFourth_location; // note : fourth highest
    std::vector<int> out_HeightFourth_height;

    std::vector<int> out_felix0_bcodiff; // note: per event bcodiff
    std::vector<int> out_felix1_bcodiff; // note: per event bcodiff
    std::vector<int> out_felix2_bcodiff; // note: per event bcodiff
    std::vector<int> out_felix3_bcodiff; // note: per event bcodiff
    std::vector<int> out_felix4_bcodiff; // note: per event bcodiff
    std::vector<int> out_felix5_bcodiff; // note: per event bcodiff
    std::vector<int> out_felix6_bcodiff; // note: per event bcodiff
    std::vector<int> out_felix7_bcodiff; // note: per event bcodiff

    int out_totalHit_forCloneStudy;
    int out_total_nCloneHit;
    std::vector<int> out_felix0_NCloneHit; // note : the N clone hit of each half-ladder in each server.
    std::vector<int> out_felix1_NCloneHit;
    std::vector<int> out_felix2_NCloneHit;
    std::vector<int> out_felix3_NCloneHit;
    std::vector<int> out_felix4_NCloneHit;
    std::vector<int> out_felix5_NCloneHit;
    std::vector<int> out_felix6_NCloneHit;
    std::vector<int> out_felix7_NCloneHit;

    TH2D * h2D_NInttHitInner_NInttHitOuter;
    TH2D * h2D_NInttHit_TrigSpikeSize;
    TH1D * h1D_SemiClusPhiSize;

    TH2D * h2D_NInttHitInner_NInttHitOuter_corr;
    TH2D * h2D_BcoSpace_NCarriedHits_corr;

    std::map<std::string, int> evt_ChipHit_count_map;
    std::map<std::string, TH1D*> h1D_nChipHit_map;

    // note : -------------------------------- INTT fee map --------------------------------
    InttFeeMap m_feemap;
    std::string intt_fee_map_directory = "/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/InttFeeMap/f5/3a/f53a887d8d05ca6ae2edc0533a26e4c8_InttFeeMap.root"; // todo:

    // note : -------------------------------- for Semi clustering --------------------------------
    struct chip_clu_info{
        std::vector<double> adc_vec;
        std::vector<int> size_vec;
        std::vector<double> edge_l_vec;
        std::vector<double> edge_r_vec;
        std::vector<double> pos_vec;
        int largest_size;
    };
    TH1D * h1_chip_hit_container;
    INTTRawHitSanityCheck::chip_clu_info SemiChipClustering(int FELIX_in, int FELIX_ch_in, int chip_in, int hit_bco_in);
    INTTRawHitSanityCheck::chip_clu_info find_Ngroup(TH1D * hist_in);
    double CoM_cluster_pos(TH1D * hist_in, double edge_l, double edge_r);

    // note : -------------------------------- For counting hits back --------------------------------
    TH1D * evt_this_INTTHitBco;
    void Func_CountHitsBack();

    // note : -------------------------------- trigger_table --------------------------------
    static const int MBDNS_loose_inclusive = 11; // note : MBD N&S >= 1
    static const int MBDNS_tight_inclusive = 10; // note : MBD N&S >= 2
    static const int MBDNS_tight_vtx10cm   = 12; // note : MBD N&S >= 2 && vtx < 10 cm
    static const int MBDNS_tight_vtx30cm   = 13; // note : MBD N&S >= 2 && vtx < 30 cm
    static const int MBDNS_tight_vtx60cm   = 14; // note : MBD N&S >= 2 && vtx < 60 cm

};

#endif // INTTRawHitSanityCheck_H
