// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTBCODIFFRECO_H
#define INTTBCODIFFRECO_H

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

#include <TMath.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <numeric>

#include <cmath>
#include <bitset>
#include <numeric>

class PHCompositeNode;
class Gl1Packetv2;
class InttRawHitContainer;

class TH1;
class TTree;
class TCanvas;

class INTTBcoDiffReco : public SubsysReco
{
 public:

    INTTBcoDiffReco(
        const std::string &name = "INTTBcoDiffReco",
        const int process_id_in = 0,
        const int runnumber_in = 0,
        const std::string output_directory_in = "no_directory",
        const std::string hotchannel_full_directory_in = "no_directory",
        const bool apply_hotchannel_in = true,
        const bool clone_hit_remove_in = false,
        const bool clone_hit_remove_BCO_in = true,
        const bool hitQA_in = true,
        const bool check_clone_hit_in = true
    );

    ~INTTBcoDiffReco() override;

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
    std::vector<int> GetHistBinEntry(TH1F * hist);
    int PrepareHistInfo(TH1F * hist);
    int PrepareCloneHitInfo();

    int process_id;
    int runnumber;
    std::string output_directory;
    std::string hotchannel_full_directory;
    bool apply_hotchannel_tag;
    bool clone_hit_remove_tag;
    bool clone_hit_remove_BCO_tag;
    bool hitQA_tag;
    bool check_clone_hit_tag;

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

    std::vector<TH1F *> evt_felix_BcoDiffHist_vec;
    std::vector<TH1F *> all_felix_BcoDiffHist_vec;

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

    // note : -------------------------------- trigger_table --------------------------------
    static const int MBDNS_loose_inclusive = 11; // note : MBD N&S >= 1
    static const int MBDNS_tight_inclusive = 10; // note : MBD N&S >= 2
    static const int MBDNS_tight_vtx10cm   = 12; // note : MBD N&S >= 2 && vtx < 10 cm
    static const int MBDNS_tight_vtx30cm   = 13; // note : MBD N&S >= 2 && vtx < 30 cm
    static const int MBDNS_tight_vtx60cm   = 14; // note : MBD N&S >= 2 && vtx < 60 cm

};

#endif // INTTBcoDiffReco_H
