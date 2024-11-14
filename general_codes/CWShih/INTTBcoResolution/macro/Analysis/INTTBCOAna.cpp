#include "INTTBCOAna.h"

INTTBCOAna::INTTBCOAna(
    std::string input_directory_in,
    std::string input_filename_in,
    std::string output_directory_in,
    long long run_nEvent_in,
    bool evt_BCODiff_QA_FileOut_in,
    std::string output_filename_in,
    int expected_spike_location_in,
    std::string sPH_labeling_in
):
input_directory(input_directory_in),
input_filename(input_filename_in),
output_directory(output_directory_in),
run_nEvent(run_nEvent_in),
evt_BCODiff_QA_FileOut(evt_BCODiff_QA_FileOut_in),
output_filename(output_filename_in),
expected_spike_location(expected_spike_location_in),
sPH_labeling(sPH_labeling_in),
special_bcodiff_check(80)
{
    file_in = TFile::Open( (input_directory + "/" + input_filename).c_str() );
    
    for(int i = 0; i < 8; i++){
        all_felix_BcoDiffHist_vec.push_back((TH1F*)file_in->Get(Form("all_felix_BcoDiffHist_%d",i)));
        all_felix_BcoDiffHist_vec.back()->GetXaxis()->SetTitle(Form("(Server_%d) BCO_diff",i));
    }
    
    tree_in = (TTree*)file_in->Get("tree");

    if (!tree_in) {std::cout<<"Error : tree not found"<<std::endl; exit(1);}

    std::cout<<"N events in tree: "<<tree_in -> GetEntries()<<std::endl;

    bco_tree = new BCOResolutionTree(tree_in);

    if (run_nEvent == -1) {run_nEvent = tree_in -> GetEntries();}
    if (run_nEvent > tree_in -> GetEntries()) {run_nEvent = tree_in -> GetEntries();}

    Init();

    nEvent_post_trigger_cut = 0;
    nEvent_good_spike = 0;
    nEvent_with_clone_hit = 0;
    nEvent_BCOcut_98survival = 0;

    if (evt_BCODiff_QA_FileOut) {Prepare_outfile();}

}

void INTTBCOAna::Prepare_outfile()
{
    file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()),"recreate");
    tree_out = new TTree("tree","tree");
    tree_out -> Branch("eID", &out_eID);
    tree_out -> Branch("MBDNS_tight_inclusive", &out_MBDNS_tight_inclusive);
    tree_out -> Branch("MBDNS_tight_vtx10cm", &out_MBDNS_tight_vtx10cm);
    tree_out -> Branch("MBDNS_tight_vtx30cm", &out_MBDNS_tight_vtx30cm);
    tree_out -> Branch("GL1_bco", &out_GL1_bco);
    tree_out -> Branch("INTT_bcofull", &out_INTT_bcofull);
    tree_out -> Branch("total_nHit", &out_total_nHit);
    tree_out -> Branch("total_nHit_post", &out_total_nHit_post);
    tree_out -> Branch("survival_rate", &out_survival_rate);
    tree_out -> Branch("is_peaks_correct", &out_is_peaks_correct);
}

void INTTBCOAna::Init()
{
    SetsPhenixStyle();

    c2 = new TCanvas("c2","c2",3800, 1600); 
    c2 -> Divide(4,2);
    c1 = new TCanvas("c1","c1",950,800);

    if (!evt_BCODiff_QA_FileOut) {c1 -> Print(Form("%s/INTT_bco_diff_weird.pdf(", output_directory.c_str()));}

    // note : the ratio of the number of events that the spikes are ALL in selected region
    // note : the ratio that 98% of hits are kept in the BCO window
    // note : the distribution of kept hits ratio in the BCO window

    h2_spike_location             = new TH2F("",";Spike location;Felix Server",128,0,128,8,0,8);
    h2_spike_location             -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_inclusive = new TH2F("",";Spike content ratio (inclusive);Felix Server",100,0,1, 8,0,8);
    spike_content_ratio_inclusive -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_P1        = new TH2F("",";Spike content ratio (P1);Felix Server"       ,100,0,1, 8,0,8);
    spike_content_ratio_P1        -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_P12       = new TH2F("",";Spike content ratio (P12);Felix Server"      ,100,0,1, 8,0,8);
    spike_content_ratio_P12       -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_P123      = new TH2F("",";Spike content ratio (P123);Felix Server"     ,100,0,1, 8,0,8);
    spike_content_ratio_P123      -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_M1        = new TH2F("",";Spike content ratio (M1);Felix Server"       ,100,0,1, 8,0,8);
    spike_content_ratio_M1        -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_M12       = new TH2F("",";Spike content ratio (M12);Felix Server"      ,100,0,1, 8,0,8);
    spike_content_ratio_M12       -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_M123      = new TH2F("",";Spike content ratio (M123);Felix Server"     ,100,0,1, 8,0,8);
    spike_content_ratio_M123      -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_PM1       = new TH2F("",";Spike content ratio (PM1);Felix Server"      ,100,0,1, 8,0,8);
    spike_content_ratio_PM1       -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_PM12      = new TH2F("",";Spike content ratio (PM12);Felix Server"     ,100,0,1, 8,0,8);
    spike_content_ratio_PM12      -> GetXaxis() -> SetNdivisions(505);

    spike_content_ratio_PM123     = new TH2F("",";Spike content ratio (PM123);Felix Server"    ,100,0,1, 8,0,8);
    spike_content_ratio_PM123     -> GetXaxis() -> SetNdivisions(505);

    BCOcut_survival_rate          = new TH2F("",";BCOcut survival rate;Felix Server"           ,100,0,1, 8,0,8);
    BCOcut_survival_rate          -> GetXaxis() -> SetNdivisions(505);

    BCOcut_survival_rate_all = new TH1F("", ";BCOcut survival rate (All);Entries (/0.01)", 100, 0, 1);
    BCOcut_survival_rate_all -> GetXaxis() -> SetNdivisions(505);

    BCOcut_survival_rate_all_zoomin = new TH1F("", ";BCOcut survival rate (All);Entries (/0.002)", 100, 0.8, 1);
    BCOcut_survival_rate_all_zoomin -> GetXaxis() -> SetNdivisions(505);


    BCOcut_survival_rate_all_NClus = new TH2F("", ";Number of INTT raw hits;BCOcut survival rate (All)", 100, 0, 25000, 100, 0, 1);
    BCOcut_survival_rate_all_NClus -> GetXaxis() -> SetNdivisions(505);

    BCO_diff_event_by_event = new TH2F("", ";BCO_diff;Felix Server", 128,0,128, 8, 0, 8);

    spike_content_ratio_P1_NClus.clear();
    spike_content_ratio_P12_NClus.clear();
    spike_content_ratio_P123_NClus.clear();
    second_spike_distance_ratio.clear();
    spike_location_NClus.clear();
    
    for (int i = 0; i < 8; i++)
    {
        spike_content_ratio_P1_NClus.push_back(new TH2F("",Form("server_%d;Number of INTT raw hits;spike content ratio (P1)",i),100,0,25000,100,0,1));   
        spike_content_ratio_P1_NClus.back() -> GetXaxis() -> SetNdivisions(505);

        spike_content_ratio_P12_NClus.push_back(new TH2F("",Form("server_%d;Number of INTT raw hits;spike content ratio (P12)",i),100,0,25000,100,0,1));  
        spike_content_ratio_P12_NClus.back() -> GetXaxis() -> SetNdivisions(505);

        spike_content_ratio_P123_NClus.push_back(new TH2F("",Form("server_%d;Number of INTT raw hits;spike content ratio (P123)",i),100,0,25000,100,0,1)); 
        spike_content_ratio_P123_NClus.back() -> GetXaxis() -> SetNdivisions(505);


        second_spike_distance_ratio.push_back(new TH2F("",Form("server_%d;|spike^{1st} - spike^{2nd}|;second spike content ratio (inclusive)",i),128,0,128,100,0,1));
        second_spike_distance_ratio.back() -> GetXaxis() -> SetNdivisions(505);

        spike_location_NClus.push_back(new TH2F("",Form("server_%d;Number of INTT raw hits;spike location",i),100,0,25000,128,0,128));
        spike_location_NClus.back() -> GetXaxis() -> SetNdivisions(505);

    }
    
    NHit_hist             = new TH1F("",";Number of INTT raw hits;Entries",100,0,25000);
    NHit_hist_ZoomIn      = new TH1F("",";Number of INTT raw hits;Entries",100,0,1000);
    
    NHit_hist_post        = new TH1F("",";Number of INTT raw hits;Entries",100,0,25000);
    NHit_hist_post_ZoomIn = new TH1F("",";Number of INTT raw hits;Entries",100,0,1000);
    NHit_hist_post -> SetLineColor(kRed);
    NHit_hist_post_ZoomIn -> SetLineColor(kRed);

    NCloneHit_hist = new TH1F("",";N clone hits;Entries",30,0,30);
    CloneHit_ratio = new TH1F("",";NCloneHit / NTotalHit;Entries",100,0,1);
    CloneHitRatio_NHit = new TH2F("",";Number of INTT raw hits;NCloneHit / NTotalHit",100,0,25000,100,0,1);
    clone_hit_map = new TH2F("",";FELIX channel;FELIX server",14,0,14,8,0,8);
    
    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);
    
    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);
}

std::vector<int> INTTBCOAna::CombineVector(std::vector<std::vector<int> *> vector_in)
{
    vector<int> combined_vector;

    for (int i = 0; i < vector_in.size(); i++)
    {
        if (i == 0) {combined_vector = *vector_in[i];}
        else {
            for (int j = 0; j < vector_in[i]->size(); j++)
            {
                combined_vector[j] += vector_in[i]->at(j);
            }
        }
    }

    return combined_vector;
}

double INTTBCOAna::PrepareSurvivalRate(std::vector<int> *bcodiff_vector, int spike_location_in)
{
    double kept_count = 0;

    if (spike_location_in - 1 < 0) {std::cout<<"Warning : spike location - 1 is less than 0"<<std::endl; exit(1);}
    if (spike_location_in + 1 > 127) {std::cout<<"Warning : spike location + 1 is greater than 127"<<std::endl; exit(1);}

    kept_count += bcodiff_vector->at(spike_location_in - 1);
    kept_count += bcodiff_vector->at(spike_location_in);
    kept_count += bcodiff_vector->at(spike_location_in + 1);

    return kept_count / std::accumulate(bcodiff_vector->begin(), bcodiff_vector->end(), 0);
}

bool INTTBCOAna::CheckEventSpike(std::vector<int> *HeightFirst_location_in, int spike_location_in)
{
    if (HeightFirst_location_in->size() != 8) {
        std::cout<<"Error : HeightFirst_location_in size is not 8"<<std::endl;
        exit(1);
    }
    
    for (int i = 0; i < HeightFirst_location_in->size(); i++)
    {
        if (HeightFirst_location_in->at(i) != spike_location_in) {return false;}
    }

    return true;
}

int INTTBCOAna::IsPeaksCorrect()
{
    int n_correct_peaks = 0;

    for (int i = 0; i < 8; i++)
    {
        if (bco_tree -> HeightFirst_location -> at(i) == expected_spike_location) {n_correct_peaks++;}
    }

    return n_correct_peaks;
}

void INTTBCOAna::PrepareEvtBcoDiffHist(int eID_in, long long total_nHit_post_in, double event_BCOcut_survival_rate_in)
{
    BCO_diff_event_by_event -> Reset("ICESM");
    BCO_diff_event_by_event -> SetTitle(Form("eID : %d, Nhit: %d;bco_diff;FELIX", eID_in, total_nHit_post_in));

    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 0), bco_tree -> felix0_bcodiff -> at(hit_i));}
    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 1), bco_tree -> felix1_bcodiff -> at(hit_i));}
    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 2), bco_tree -> felix2_bcodiff -> at(hit_i));}
    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 3), bco_tree -> felix3_bcodiff -> at(hit_i));}
    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 4), bco_tree -> felix4_bcodiff -> at(hit_i));}
    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 5), bco_tree -> felix5_bcodiff -> at(hit_i));}
    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 6), bco_tree -> felix6_bcodiff -> at(hit_i));}
    for (int hit_i = 0; hit_i < 128; hit_i++) {BCO_diff_event_by_event -> SetBinContent(BCO_diff_event_by_event->FindBin(hit_i, 7), bco_tree -> felix7_bcodiff -> at(hit_i));}

    c1 -> cd();
    BCO_diff_event_by_event -> Draw("colz0");

    draw_text->DrawLatex(0.6, 0.85, Form("eID : %d, Nhit: %d", eID_in, total_nHit_post_in));
    draw_text->DrawLatex(0.6, 0.81, Form("survival rate : %.3f",event_BCOcut_survival_rate_in));
}

void INTTBCOAna::ProcessEvents()
{
    for (int i = 0; i < run_nEvent; i++)
    {
        if (i % 3000 == 0) {std::cout<<"Processing event "<<i<<std::endl;}

        bco_tree -> LoadTree(i);
        bco_tree -> GetEntry(i);

        if (!evt_BCODiff_QA_FileOut && bco_tree->MBDNS_tight_vtx30cm == 0) continue;
        if (!evt_BCODiff_QA_FileOut && bco_tree->total_nHit == 0) continue;

        out_eID = bco_tree -> eID; 
        out_MBDNS_tight_inclusive = bco_tree -> MBDNS_tight_inclusive; 
        out_MBDNS_tight_vtx10cm = bco_tree -> MBDNS_tight_vtx10cm; 
        out_MBDNS_tight_vtx30cm = bco_tree -> MBDNS_tight_vtx30cm; 
        out_GL1_bco = bco_tree -> GL1_bco; 
        out_INTT_bcofull = bco_tree -> INTT_bcofull; 
        out_total_nHit = bco_tree -> total_nHit; 
        out_total_nHit_post = bco_tree -> total_nHit_post; 

        out_survival_rate = -999;
        out_is_peaks_correct = -999;

        if (evt_BCODiff_QA_FileOut && bco_tree->total_nHit == 0) {
            tree_out -> Fill();
            continue;
        }

        nEvent_post_trigger_cut++;

        if (bco_tree->total_nCloneHit > 0) {nEvent_with_clone_hit++;}

        if (CheckEventSpike(bco_tree->HeightFirst_location, expected_spike_location)) {nEvent_good_spike++;}

        long long total_nHit = bco_tree -> total_nHit;
        long long total_nHit_post = bco_tree -> total_nHit_post; // note : after applying the hitQA, hot channel removal, and clone hit removal, if is set
        long long total_nCloneHit = bco_tree -> total_nCloneHit;
        int       totalHit_forCloneStudy = bco_tree -> totalHit_forCloneStudy;

        // note : total counts of each FELIX
        std::vector<int> felix_hit_count = {
            std::accumulate( bco_tree->felix0_bcodiff->begin(), bco_tree->felix0_bcodiff->end(), 0 ),
            std::accumulate( bco_tree->felix1_bcodiff->begin(), bco_tree->felix1_bcodiff->end(), 0 ),
            std::accumulate( bco_tree->felix2_bcodiff->begin(), bco_tree->felix2_bcodiff->end(), 0 ),
            std::accumulate( bco_tree->felix3_bcodiff->begin(), bco_tree->felix3_bcodiff->end(), 0 ),
            std::accumulate( bco_tree->felix4_bcodiff->begin(), bco_tree->felix4_bcodiff->end(), 0 ),
            std::accumulate( bco_tree->felix5_bcodiff->begin(), bco_tree->felix5_bcodiff->end(), 0 ),
            std::accumulate( bco_tree->felix6_bcodiff->begin(), bco_tree->felix6_bcodiff->end(), 0 ),
            std::accumulate( bco_tree->felix7_bcodiff->begin(), bco_tree->felix7_bcodiff->end(), 0 )
        };

        // note : checking the whole event, the hits within the BCOcut window / total hits
        std::vector<int> combined_vector = CombineVector({
            bco_tree -> felix0_bcodiff,
            bco_tree -> felix1_bcodiff,
            bco_tree -> felix2_bcodiff,
            bco_tree -> felix3_bcodiff,
            bco_tree -> felix4_bcodiff,
            bco_tree -> felix5_bcodiff,
            bco_tree -> felix6_bcodiff,
            bco_tree -> felix7_bcodiff
        });
        double event_BCOcut_survival_rate =  PrepareSurvivalRate(
            &combined_vector,
            expected_spike_location
        );

        out_survival_rate = event_BCOcut_survival_rate;
        out_is_peaks_correct = IsPeaksCorrect();

        if (evt_BCODiff_QA_FileOut) {
            tree_out -> Fill();
            continue;
        }

        BCOcut_survival_rate_all -> Fill( event_BCOcut_survival_rate );
        BCOcut_survival_rate_all_zoomin -> Fill( event_BCOcut_survival_rate );
        if (event_BCOcut_survival_rate > 0.98) {nEvent_BCOcut_98survival++;}

        BCOcut_survival_rate_all_NClus -> Fill(total_nHit_post, event_BCOcut_survival_rate);

        for (int felix_i = 0; felix_i < 8; felix_i++)
        {
            double denominator_inclusive = felix_hit_count[felix_i];

            double spike_height = bco_tree -> HeightFirst_height -> at(felix_i);
            double spike_location = bco_tree -> HeightFirst_location -> at(felix_i);

            if (spike_location == -999) {continue;}

            double P1Height = (bco_tree -> spikeP1_height -> at(felix_i) == -999) ? 0. : bco_tree -> spikeP1_height -> at(felix_i);
            double P2Height = (bco_tree -> spikeP2_height -> at(felix_i) == -999) ? 0. : bco_tree -> spikeP2_height -> at(felix_i);
            double P3Height = (bco_tree -> spikeP3_height -> at(felix_i) == -999) ? 0. : bco_tree -> spikeP3_height -> at(felix_i);

            double M1Height = (bco_tree -> spikeM1_height -> at(felix_i) == -999) ? 0. : bco_tree -> spikeM1_height -> at(felix_i);
            double M2Height = (bco_tree -> spikeM2_height -> at(felix_i) == -999) ? 0. : bco_tree -> spikeM2_height -> at(felix_i);
            double M3Height = (bco_tree -> spikeM3_height -> at(felix_i) == -999) ? 0. : bco_tree -> spikeM3_height -> at(felix_i);

            h2_spike_location ->Fill(bco_tree->HeightFirst_location->at(felix_i), felix_i);
            spike_content_ratio_inclusive ->Fill(spike_height / denominator_inclusive, felix_i);

            spike_location_NClus[felix_i] -> Fill(total_nHit_post, spike_location);

            if (spike_location == expected_spike_location){

                spike_content_ratio_P1   ->Fill(spike_height / (spike_height + P1Height), felix_i); // note : for time scan
                spike_content_ratio_P12  ->Fill(spike_height / (spike_height + P1Height + P2Height), felix_i); // note : for time scan
                spike_content_ratio_P123 ->Fill(spike_height / (spike_height + P1Height + P2Height + P3Height), felix_i); // note : for time scan

                spike_content_ratio_M1   ->Fill(spike_height / (spike_height + M1Height), felix_i);
                spike_content_ratio_M12  ->Fill(spike_height / (spike_height + M1Height + M2Height), felix_i);
                spike_content_ratio_M123 ->Fill(spike_height / (spike_height + M1Height + M2Height + M3Height), felix_i);

                spike_content_ratio_PM1   ->Fill(spike_height / (spike_height + P1Height + M1Height), felix_i);
                spike_content_ratio_PM12  ->Fill(spike_height / (spike_height + P1Height + P2Height + M1Height + M2Height), felix_i);
                spike_content_ratio_PM123 ->Fill(spike_height / (spike_height + P1Height + P2Height + P3Height + M1Height + M2Height + M3Height), felix_i);

                spike_content_ratio_P1_NClus[felix_i]   -> Fill(total_nHit_post,spike_height / (spike_height + P1Height));
                spike_content_ratio_P12_NClus[felix_i]  -> Fill(total_nHit_post,spike_height / (spike_height + P1Height + P2Height));
                spike_content_ratio_P123_NClus[felix_i] -> Fill(total_nHit_post,spike_height / (spike_height + P1Height + P2Height + P3Height));

                second_spike_distance_ratio[felix_i] -> Fill(fabs(bco_tree->HeightFirst_location->at(felix_i) - bco_tree->HeightSecond_location->at(felix_i)), bco_tree -> HeightSecond_height -> at(felix_i) / denominator_inclusive);
            }

        }

        // note : draw the event by event plot
        if (event_BCOcut_survival_rate < 0.8 && (i%25 == 0)) {
            PrepareEvtBcoDiffHist(i, total_nHit_post, event_BCOcut_survival_rate);

            c1 -> Print(Form("%s/INTT_bco_diff_weird.pdf", output_directory.c_str()));
            c1 -> Clear();
        }
        else if (event_BCOcut_survival_rate < 0.8 && total_nHit_post > 2000 && (i%5 == 0)){
            PrepareEvtBcoDiffHist(i, total_nHit_post, event_BCOcut_survival_rate);

            c1 -> Print(Form("%s/INTT_bco_diff_weird.pdf", output_directory.c_str()));
            c1 -> Clear();
        }
        else if (
            bco_tree->HeightFirst_location->at(0) == special_bcodiff_check ||
            bco_tree->HeightFirst_location->at(1) == special_bcodiff_check ||
            bco_tree->HeightFirst_location->at(2) == special_bcodiff_check ||
            bco_tree->HeightFirst_location->at(3) == special_bcodiff_check
        ){
            PrepareEvtBcoDiffHist(i, total_nHit_post, event_BCOcut_survival_rate);

            draw_text->DrawLatex(0.5, 0.77, Form("Special check: %d",special_bcodiff_check));
            c1 -> Print(Form("%s/INTT_bco_diff_weird.pdf", output_directory.c_str()));
            c1 -> Clear();
        }

        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix0_bcodiff, expected_spike_location), 0);  
        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix1_bcodiff, expected_spike_location), 1);  
        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix2_bcodiff, expected_spike_location), 2);  
        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix3_bcodiff, expected_spike_location), 3);  
        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix4_bcodiff, expected_spike_location), 4);  
        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix5_bcodiff, expected_spike_location), 5);  
        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix6_bcodiff, expected_spike_location), 6);  
        BCOcut_survival_rate -> Fill(PrepareSurvivalRate(bco_tree -> felix7_bcodiff, expected_spike_location), 7);  


        // note : for the comparison before and after the hitQA, hot channel removal, and clone hit removal, if applied
        NHit_hist -> Fill(total_nHit);
        NHit_hist_ZoomIn -> Fill(total_nHit);

        NHit_hist_post -> Fill(total_nHit_post);
        NHit_hist_post_ZoomIn -> Fill(total_nHit_post);

        // note : for clone hit plots
        CloneHit_ratio -> Fill(total_nCloneHit / double(totalHit_forCloneStudy));
        CloneHitRatio_NHit -> Fill(total_nHit, total_nCloneHit / double(totalHit_forCloneStudy));

        for (int Fch = 0; Fch < bco_tree -> felix0_NCloneHit -> size(); Fch++ )
        {
            clone_hit_map -> Fill(double(Fch), 0., double(bco_tree->felix0_NCloneHit->at(Fch)));
            clone_hit_map -> Fill(double(Fch), 1., double(bco_tree->felix1_NCloneHit->at(Fch)));
            clone_hit_map -> Fill(double(Fch), 2., double(bco_tree->felix2_NCloneHit->at(Fch)));
            clone_hit_map -> Fill(double(Fch), 3., double(bco_tree->felix3_NCloneHit->at(Fch)));
            clone_hit_map -> Fill(double(Fch), 4., double(bco_tree->felix4_NCloneHit->at(Fch)));
            clone_hit_map -> Fill(double(Fch), 5., double(bco_tree->felix5_NCloneHit->at(Fch)));
            clone_hit_map -> Fill(double(Fch), 6., double(bco_tree->felix6_NCloneHit->at(Fch)));
            clone_hit_map -> Fill(double(Fch), 7., double(bco_tree->felix7_NCloneHit->at(Fch)));
        }

        NCloneHit_hist -> Fill(total_nCloneHit);

    }
}

void INTTBCOAna::DrawPlots()
{   

    if (!evt_BCODiff_QA_FileOut) {c1 -> Print(Form("%s/INTT_bco_diff_weird.pdf)", output_directory.c_str()));}

    for (int i = 0; i < 8; i++)
    {
        c2 -> cd(i+1);
        spike_content_ratio_P1_NClus[i] -> Draw("colz0");
        c2 -> cd(i+1) -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    }
    c2 -> Print(Form("%s/spike_content_ratio_P1_NClus.pdf", output_directory.c_str()));
    c2 -> Clear();

    c1 -> Print(Form("%s/spike_content_ratio_P1_NClus_separate.pdf(", output_directory.c_str()));
    for (int i = 0; i < 8; i++)
    {
        c1 -> cd();
        spike_content_ratio_P1_NClus[i] -> Draw("colz0");
        c1 -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
        c1 -> Print(Form("%s/spike_content_ratio_P1_NClus_separate.pdf", output_directory.c_str()));
    }
    c1 -> Print(Form("%s/spike_content_ratio_P1_NClus_separate.pdf)", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);




    c2 -> Divide(4,2);
    for (int i = 0; i < 8; i++)
    {
        c2 -> cd(i+1);
        spike_content_ratio_P12_NClus[i] -> Draw("colz0");
        c2 -> cd(i+1) -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    }
    c2 -> Print(Form("%s/spike_content_ratio_P12_NClus.pdf", output_directory.c_str()));
    c2 -> Clear();

    c1 -> Print(Form("%s/spike_content_ratio_P12_NClus_separate.pdf(", output_directory.c_str()));
    for (int i = 0; i < 8; i++)
    {
        c1 -> cd();
        spike_content_ratio_P12_NClus[i] -> Draw("colz0");
        c1 -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
        c1 -> Print(Form("%s/spike_content_ratio_P12_NClus_separate.pdf", output_directory.c_str()));
    }
    c1 -> Print(Form("%s/spike_content_ratio_P12_NClus_separate.pdf)", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);





    c2 -> Divide(4,2);
    for (int i = 0; i < 8; i++)
    {
        c2 -> cd(i+1);
        spike_content_ratio_P123_NClus[i] -> Draw("colz0");
        c2 -> cd(i+1) -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    }
    c2 -> Print(Form("%s/spike_content_ratio_P123_NClus.pdf", output_directory.c_str()));
    c2 -> Clear();

    c1 -> Print(Form("%s/spike_content_ratio_P123_NClus_separate.pdf(", output_directory.c_str()));
    for (int i = 0; i < 8; i++)
    {
        c1 -> cd();
        spike_content_ratio_P123_NClus[i] -> Draw("colz0");
        c1 -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
        c1 -> Print(Form("%s/spike_content_ratio_P123_NClus_separate.pdf", output_directory.c_str()));
    }
    c1 -> Print(Form("%s/spike_content_ratio_P123_NClus_separate.pdf)", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);





    c2 -> Divide(4,2);
    for (int i = 0; i < 8; i++)
    {
        c2 -> cd(i+1);
        second_spike_distance_ratio[i] -> Draw("colz0");
        c2 -> cd(i+1) -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    }
    c2 -> Print(Form("%s/second_spike_distance_ratio.pdf", output_directory.c_str()));
    c2 -> Clear();

    c1 -> Print(Form("%s/second_spike_distance_ratio_separate.pdf(", output_directory.c_str()));
    for (int i = 0; i < 8; i++)
    {
        c1 -> cd();
        second_spike_distance_ratio[i] -> Draw("colz0");
        c1 -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
        c1 -> Print(Form("%s/second_spike_distance_ratio_separate.pdf", output_directory.c_str()));
    }
    c1 -> Print(Form("%s/second_spike_distance_ratio_separate.pdf)", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);




    c2 -> Divide(4,2);
    for(int i = 0; i < 8; i++)
    {
        c2 -> cd(i+1);
        all_felix_BcoDiffHist_vec[i] -> Draw("hist");
        c2 -> cd(i+1) -> SetLogy(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    }
    c2 -> Print(Form("%s/all_felix_BcoDiffHist.pdf", output_directory.c_str()));
    c2 -> Clear();

    c1 -> Print(Form("%s/all_felix_BcoDiffHist_separate.pdf(", output_directory.c_str()));
    for (int i = 0; i < 8; i++)
    {
        c1 -> cd();
        all_felix_BcoDiffHist_vec[i] -> Draw("colz0");
        c1 -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
        c1 -> Print(Form("%s/all_felix_BcoDiffHist_separate.pdf", output_directory.c_str()));
    }
    c1 -> Print(Form("%s/all_felix_BcoDiffHist_separate.pdf)", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);





    c2 -> Divide(4,2);
    for(int i = 0; i< 8; i++)
    {
        c2 -> cd(i+1);
        spike_location_NClus[i] -> Draw("colz0");
        c2 -> cd(i+1) -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    }
    c2 -> Print(Form("%s/spike_location_NClus.pdf", output_directory.c_str()));
    c2 -> Clear();

    c1 -> Print(Form("%s/spike_location_NClus_separate.pdf(", output_directory.c_str()));
    for (int i = 0; i < 8; i++)
    {
        c1 -> cd();
        spike_location_NClus[i] -> Draw("colz0");
        c1 -> SetLogz(1);
        ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
        c1 -> Print(Form("%s/spike_location_NClus_separate.pdf", output_directory.c_str()));
    }
    c1 -> Print(Form("%s/spike_location_NClus_separate.pdf)", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);







    c1 -> cd();
    h2_spike_location -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/h2_spike_location.pdf", output_directory.c_str()));
    c1 -> SetLogz(1);
    c1 -> Print(Form("%s/h2_spike_location_log.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);
    
    c1 -> cd();
    spike_content_ratio_inclusive -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_inclusive.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_P1 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_P1.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_P12 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_P12.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_P123 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> SetLogz(1);
    c1 -> Print(Form("%s/spike_content_ratio_P123.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);
    
    c1 -> cd();
    spike_content_ratio_M1 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_M1.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_M12 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_M12.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_M123 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_M123.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_PM1 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_PM1.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_PM12 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_PM12.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    spike_content_ratio_PM123 -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/spike_content_ratio_PM123.pdf", output_directory.c_str()));
    c1 -> Clear();
    
    c1 -> cd();
    BCOcut_survival_rate -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> Print(Form("%s/BCOcut_survival_rate.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    BCOcut_survival_rate_all_NClus -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetGridy(1);
    c1 -> SetLogz(1);
    c1 -> Print(Form("%s/BCOcut_survival_rate_all_NClus.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogz(0);


    c1 -> cd();
    BCOcut_survival_rate_all -> Draw("hist");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetLogy(1);
    c1 -> Print(Form("%s/BCOcut_survival_rate_all.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);

    c1 -> cd();
    BCOcut_survival_rate_all_zoomin -> SetMinimum(0);
    BCOcut_survival_rate_all_zoomin -> Draw("hist");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> Print(Form("%s/BCOcut_survival_rate_all_zoomin.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    NHit_hist -> Draw("hist");
    NHit_hist_post -> Draw("hist same");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> Print(Form("%s/NHit_hist.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    NHit_hist -> Draw("hist");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetLogy(1);
    c1 -> Print(Form("%s/NHit_hist_log.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);

    c1 -> cd();
    NHit_hist_post -> Draw("hist same");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetLogy(1);
    c1 -> Print(Form("%s/NHit_hist_post_log.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);

    c1 -> cd();
    NHit_hist_ZoomIn -> Draw("hist");
    NHit_hist_post_ZoomIn -> Draw("hist same");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> Print(Form("%s/NHit_hist_ZoomIn.pdf", output_directory.c_str()));
    c1 -> Clear();


    // note : for the clone hit plots 
    c1 -> cd();
    CloneHit_ratio -> Draw("hist");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> Print(Form("%s/CloneHit_ratio.pdf", output_directory.c_str()));
    c1 -> SetLogy(1);
    c1 -> Print(Form("%s/CloneHit_ratio_log.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);

    c1 -> cd();
    NCloneHit_hist -> Draw("hist");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> SetLogy(1);
    c1 -> Print(Form("%s/NCloneHit_hist.pdf", output_directory.c_str()));
    c1 -> Clear();
    c1 -> SetLogy(0);

    c1 -> cd();
    CloneHitRatio_NHit -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> Print(Form("%s/CloneHitRatio_NHit.pdf", output_directory.c_str()));    
    c1 -> Clear();

    c1 -> cd();
    clone_hit_map -> Draw("colz0");
    ltx -> DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));
    c1 -> Print(Form("%s/clone_hit_map.pdf", output_directory.c_str()));
    c1 -> Clear();

}

void INTTBCOAna::SaveHistFile(std::string output_rootfilename)
{
    TFile * file_out = new TFile(output_rootfilename.c_str(), "RECREATE");
    file_out -> cd();


    for (int i = 0; i < 8; i++)
    {
        spike_content_ratio_P1_NClus[i] -> Write(Form("spike_content_ratio_P1_NClus_%d",i));
    }

    for (int i = 0; i < 8; i++)
    {
        spike_content_ratio_P12_NClus[i] -> Write(Form("spike_content_ratio_P12_NClus_%d",i));
    }

    for (int i = 0; i < 8; i++)
    {
        spike_content_ratio_P123_NClus[i] -> Write(Form("spike_content_ratio_P123_NClus_%d",i));
    }

    for (int i = 0; i < 8; i++)
    {
        second_spike_distance_ratio[i] -> Write(Form("second_spike_distance_ratio_%d",i));
    }

    for(int i = 0; i < 8; i++)
    {
        all_felix_BcoDiffHist_vec[i] -> Write(Form("all_felix_BcoDiffHist_vec_%d",i));
    }



    h2_spike_location -> Write("h2_spike_location");
    
    spike_content_ratio_inclusive -> Write("spike_content_ratio_inclusive");
    
    spike_content_ratio_P1 -> Write("spike_content_ratio_P1");
    
    spike_content_ratio_P12 -> Write("spike_content_ratio_P12");
    
    spike_content_ratio_P123 -> Write("spike_content_ratio_P123");
    
    spike_content_ratio_M1 -> Write("spike_content_ratio_M1");
    
    spike_content_ratio_M12 -> Write("spike_content_ratio_M12");
    
    spike_content_ratio_M123 -> Write("spike_content_ratio_M123");
    
    spike_content_ratio_PM1 -> Write("spike_content_ratio_PM1");
    
    spike_content_ratio_PM12 -> Write("spike_content_ratio_PM12");
    
    spike_content_ratio_PM123 -> Write("spike_content_ratio_PM123");
    
    BCOcut_survival_rate -> Write("BCOcut_survival_rate");

    NHit_hist -> Write("NHit_hist");

    NHit_hist_post -> Write("NHit_hist_post");

    NHit_hist_ZoomIn -> Write("NHit_hist_ZoomIn");
    
    NHit_hist_post_ZoomIn -> Write("NHit_hist_post_ZoomIn");


    // note : for the clone hit plots 
    CloneHit_ratio -> Write("CloneHit_ratio");

    NCloneHit_hist -> Write("NCloneHit_hist");

    CloneHitRatio_NHit -> Write("CloneHitRatio_NHit");

    clone_hit_map -> Write("clone_hit_map");

    file_out -> Close();
}

void INTTBCOAna::EndRun()
{
    if (evt_BCODiff_QA_FileOut) 
    {
        tree_out -> Write();
        file_out -> Close();
        return;
    }

    std::cout<<"nEvent_post_trigger_cut: "<<nEvent_post_trigger_cut<<std::endl;
    std::cout<<"nEvent_good_spike: "<<nEvent_good_spike<<std::endl;
    std::cout<<"Good spike ratio: "<<nEvent_good_spike / double(nEvent_post_trigger_cut)<<std::endl;
    std::cout<<std::endl;

    std::cout<<"nEvent_post_trigger_cut: "<<nEvent_post_trigger_cut<<std::endl;
    std::cout<<"nEvent_with_clone_hit: "<<nEvent_with_clone_hit<<std::endl;
    std::cout<<"clone hit ratio: "<<nEvent_with_clone_hit / double(nEvent_post_trigger_cut)<<std::endl; 
    std::cout<<std::endl;

    std::cout<<"nEvent_post_trigger_cut: "<<nEvent_post_trigger_cut<<std::endl;
    std::cout<<"nEvent_BCOcut_98survival: "<<nEvent_BCOcut_98survival<<std::endl;
    std::cout<<"BCOcut_98survival_ratio: "<<nEvent_BCOcut_98survival / double(nEvent_post_trigger_cut)<<std::endl;
    std::cout<<std::endl;

    std::cout<<"spike_content_ratio_P123 -> GetMean(): "<<spike_content_ratio_P123 -> GetMean(1)<<std::endl;
    std::cout<<"spike_content_ratio_P123 -> GetStdDev(): "<<spike_content_ratio_P123 -> GetStdDev(1)<<std::endl;
    std::cout<<"spike_content_ratio_PM123 -> GetMean(): "<<spike_content_ratio_PM123 -> GetMean(1)<<std::endl;
    std::cout<<"spike_content_ratio_PM123 -> GetStdDev(): "<<spike_content_ratio_PM123 -> GetStdDev(1)<<std::endl;
}