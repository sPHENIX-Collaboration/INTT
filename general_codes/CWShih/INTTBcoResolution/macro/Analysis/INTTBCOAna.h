#ifndef INTTBCOANA_H
#define INTTBCOANA_H

#include <iostream>
#include <numeric>

#include "BCOResolutionTree.h"
#include "sPhenixStyle.h"

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLatex.h>

class INTTBCOAna
{
    public:
        INTTBCOAna(
            std::string input_directory_in,
            std::string input_filename_in,
            std::string output_directory_in,
            long long run_nEvent_in = -1,
            bool evt_BCODiff_QA_FileOut_in = false,
            std::string output_filename_in = "evt_BCODiff_QA.root",
            int expected_spike_location_in = 55,
            std::string sPH_labeling_in = "Internal"
        );

        void ProcessEvents();
        void DrawPlots();
        void SaveHistFile(std::string output_rootfilename);
        void EndRun();

    protected: 
        void Init();
        void Prepare_outfile();
        int  IsPeaksCorrect();
        std::vector<int> CombineVector(std::vector<std::vector<int> *> vector_in);
        double PrepareSurvivalRate(std::vector<int> *bcodiff_vector, int spike_location_in);
        bool CheckEventSpike(std::vector<int> *HeightFirst_location_in, int spike_location_in);
        void PrepareEvtBcoDiffHist(int eID_in, long long total_nHit_post_in, double event_BCOcut_survival_rate_in);

        TCanvas * c1;
        TCanvas * c2;

        TFile *file_in;
        TTree *tree_in;
        BCOResolutionTree *bco_tree;

        std::string input_directory;
        std::string input_filename;
        std::string output_directory;
        long long run_nEvent;
        int expected_spike_location;
        std::string sPH_labeling;
        bool evt_BCODiff_QA_FileOut;
        std::string output_filename;

        const int special_bcodiff_check;

        long long nEvent_post_trigger_cut;
        long long nEvent_good_spike;
        long long nEvent_with_clone_hit;
        long long nEvent_BCOcut_98survival;

        // note : output file
        TFile * file_out;
        TTree * tree_out;
        Long64_t        out_eID;
        Int_t           out_MBDNS_tight_inclusive;
        Int_t           out_MBDNS_tight_vtx10cm;
        Int_t           out_MBDNS_tight_vtx30cm;
        Long64_t        out_GL1_bco;
        Long64_t        out_INTT_bcofull;
        Long64_t        out_total_nHit;
        Long64_t        out_total_nHit_post;
        double          out_survival_rate;
        int             out_is_peaks_correct;

        // note : X axis : the quantities
        // note : Y axis : the server ID
        TH2F * h2_spike_location;
        TH2F * spike_content_ratio_inclusive;
        TH2F * spike_content_ratio_P1;
        TH2F * spike_content_ratio_P12;
        TH2F * spike_content_ratio_P123;
        TH2F * spike_content_ratio_M1;
        TH2F * spike_content_ratio_M12;
        TH2F * spike_content_ratio_M123;
        TH2F * spike_content_ratio_PM1;
        TH2F * spike_content_ratio_PM12;
        TH2F * spike_content_ratio_PM123;
        
        TH2F * BCOcut_survival_rate;
        TH1F * BCOcut_survival_rate_all;
        TH1F * BCOcut_survival_rate_all_zoomin;
        
        TH2F * BCOcut_survival_rate_all_NClus;

        // note : event-by-event INTT BCO_diff of each server
        TH2F * BCO_diff_event_by_event;
        
        // note : X axis : number of clusters
        // note : Y axis : the quantities
        // note : vector : server ID
        std::vector<TH2F *> spike_content_ratio_P1_NClus;
        std::vector<TH2F *> spike_content_ratio_P12_NClus;
        std::vector<TH2F *> spike_content_ratio_P123_NClus;

        // note : X axis : second spike location
        // note : Y axis : the ratio of the entries
        // note : vector : server ID
        std::vector<TH2F *> second_spike_distance_ratio;

        // note : all bco_diff
        std::vector<TH1F *> all_felix_BcoDiffHist_vec;

        std::vector<TH2F *> spike_location_NClus;

        // note : for the clone hit
        TH1F * NHit_hist_ZoomIn;
        TH1F * NHit_hist_post_ZoomIn;
        TH1F * NHit_hist;
        TH1F * NHit_hist_post; // note : post the hot channel cut, clone hit remove, if applied
        TH1F * NCloneHit_hist;
        TH1F * CloneHit_ratio; // note : Nclone / NTotal Hit
        TH2F * CloneHitRatio_NHit; // note : Nclone / NTotal Hit vs NTotal Hit
        TH2F * clone_hit_map;

        // note : cosmetic 
        TLatex * ltx;
        TLatex * draw_text;
};

#endif