void check() {
    
  // TFile *file = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/completed/0All.root");
  TFile *file = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/1000files_Criteria2_TTree_0hitHandled.root");
  // TFile *file = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/1000files_Criteria2_TTree_0hitHandled_halfladderCutted.root");
  if (!file || file->IsZombie()) {
    std::cerr << "Error: cannot open file" << std::endl;
    return;
  }

  TTree *tree = (TTree*)file->Get("tree");
  if (!tree) {
    std::cerr << "Error: cannot find TTree" << std::endl;
    return;
  }


  // まず全部 OFF
  tree->SetBranchStatus("*", 0);

  // 必要なブランチだけ ON
  tree->SetBranchStatus("n_intt_raw_hits", 1);
  tree->SetBranchStatus("num_of_carried_hits", 1);
  tree->SetBranchStatus("num_of_carried_hits_inner", 1);
  tree->SetBranchStatus("num_of_carried_hits_outer", 1);
  tree->SetBranchStatus("interval_from_previous", 1);
  tree->SetBranchStatus("is_MBDNS_tight_vtx10cm", 1);

  int n_intt_raw_hits;
  int num_of_carried_hits;
  int num_of_carried_hits_inner;
  int num_of_carried_hits_outer;
  int64_t interval_from_previous;
  bool is_MBDNS_tight_vtx10cm;
  tree->SetBranchAddress("n_intt_raw_hits", &n_intt_raw_hits);
  tree->SetBranchAddress("num_of_carried_hits", &num_of_carried_hits);
  tree->SetBranchAddress("num_of_carried_hits_inner", &num_of_carried_hits_inner);
  tree->SetBranchAddress("num_of_carried_hits_outer", &num_of_carried_hits_outer);
  tree->SetBranchAddress("interval_from_previous",  &interval_from_previous);
  tree->SetBranchAddress("is_MBDNS_tight_vtx10cm", &is_MBDNS_tight_vtx10cm);

  int previous_event_n_intt_raw_hits;
  int previous_event_interval_from_previous;
  bool previous_event_is_MBDNS_tight_vtx10cm;
  
  TH1D* h_temp_all = new TH1D("h_temp_all","h_temp_all; interval [BCO]; Entries",127,0,127);
  TH1D* h_temp_at_least_1_hit = new TH1D("h_temp_at_least_1_hit","h_temp_at_least_1_hit; interval [BCO]; Entries",127,0,127);
  TH1D* h_temp_with_carryover = new TH1D("h_temp_with_carryover","h_temp_with_carryover; interval [BCO]; Entries",127,0,127);
  TH1D* h_temp_with_carryover_at_least_1_hit = new TH1D("h_temp_with_carryover_at_least_1_hit","h_temp_with_carryover_at_least_1_hit; interval [BCO]; Entries",127,0,127);
  TH2D* h_misidentified_num_of_carried_hits = new TH2D("h_misidentified_num_of_carried_hits","(interval > 80);inner;outer",5000,0,5000,5000,0,5000);
  TH2D* h_misidentified_num_of_carried_hits_short_interval = new TH2D("h_misidentified_num_of_carried_hits_short_interval","(16 < interval <= 22 );inner;outer",5000,0,5000,5000,0,5000);
  TH2D* h_misidentified_num_of_carried_hits_short_interval_1 = new TH2D("h_misidentified_num_of_carried_hits_short_interval_1","(30 < interval <= 35 );inner;outer",5000,0,5000,5000,0,5000);
  TH2D* h_misidentified_num_of_carried_hits_short_interval_2 = new TH2D("h_misidentified_num_of_carried_hits_short_interval_2","(35 < interval <= 40 );inner;outer",5000,0,5000,5000,0,5000);
  TH2D* h_misidentified_num_of_carried_hits_short_interval_3 = new TH2D("h_misidentified_num_of_carried_hits_short_interval_3","(40 < interval <= 45 );inner;outer",5000,0,5000,5000,0,5000);
  TH2D* h_misidentified_num_of_carried_hits_short_interval_4 = new TH2D("h_misidentified_num_of_carried_hits_short_interval_4","(45 < interval <= 50 );inner;outer",5000,0,5000,5000,0,5000);
  TH2D* h_misidentified_num_of_carried_hits_short_interval_5 = new TH2D("h_misidentified_num_of_carried_hits_short_interval_5","(50 < interval <= 55 );inner;outer",5000,0,5000,5000,0,5000);
  TH2D* h_misidentified_num_of_carried_hits_short_interval_6 = new TH2D("h_misidentified_num_of_carried_hits_short_interval_6","(55 < interval <= 60 );inner;outer",5000,0,5000,5000,0,5000);
  
  TGraph* g = new TGraph();
  TGraph* g2 = new TGraph();
  TGraph* g3 = new TGraph();

  Long64_t nEntries = tree->GetEntries();
  std::cout << nEntries << std::endl;
  for (Long64_t i = 0; i < nEntries; i++) {
  // for (Long64_t i = 0; i < 10000; i++) {
  // for (Long64_t i = 0; i < 100000; i++) {
    if ( i%100000 == 0 ) { std::cout << i << std::endl; }

    tree->GetEntry(i);  
    if ( i == 0 ) {
      previous_event_interval_from_previous = interval_from_previous;
      previous_event_n_intt_raw_hits = n_intt_raw_hits;
      previous_event_is_MBDNS_tight_vtx10cm = is_MBDNS_tight_vtx10cm;
      continue;
    } 


    // std::cout << previous_event_is_MBDNS_tight_vtx10cm << std::endl;

    if ( previous_event_is_MBDNS_tight_vtx10cm == false ) { 
      // std::cout << previous_event_is_MBDNS_tight_vtx10cm << std::endl;
      previous_event_interval_from_previous = interval_from_previous;
      previous_event_n_intt_raw_hits = n_intt_raw_hits;
      previous_event_is_MBDNS_tight_vtx10cm = is_MBDNS_tight_vtx10cm;
      continue; 
    }

    h_temp_all->Fill(interval_from_previous);
    if ( previous_event_n_intt_raw_hits > 0 && n_intt_raw_hits > 0 ) {
      h_temp_at_least_1_hit->Fill(interval_from_previous);
    }

    if ( num_of_carried_hits > 0 ){
      h_temp_with_carryover->Fill(interval_from_previous);
      if ( previous_event_n_intt_raw_hits > 0 ) {
        h_temp_with_carryover_at_least_1_hit->Fill(interval_from_previous);
      }


      if ( 16 < interval_from_previous && interval_from_previous < 22 ){
        h_misidentified_num_of_carried_hits_short_interval->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
      if ( 30 < interval_from_previous && interval_from_previous <= 35 ){
        h_misidentified_num_of_carried_hits_short_interval_1->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
      if ( 35 < interval_from_previous && interval_from_previous <= 40 ){
        h_misidentified_num_of_carried_hits_short_interval_2->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
      if ( 40 < interval_from_previous && interval_from_previous <= 45 ){
        h_misidentified_num_of_carried_hits_short_interval_3->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
      if ( 45 < interval_from_previous && interval_from_previous <= 50 ){
        h_misidentified_num_of_carried_hits_short_interval_4->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
      if ( 50 < interval_from_previous && interval_from_previous <= 55 ){
        h_misidentified_num_of_carried_hits_short_interval_5->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
      if ( 55 < interval_from_previous && interval_from_previous <= 60 ){
        h_misidentified_num_of_carried_hits_short_interval_6->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
      if ( interval_from_previous > 80 ){
        h_misidentified_num_of_carried_hits->Fill(num_of_carried_hits_inner,num_of_carried_hits_outer);
      }
    }

    previous_event_interval_from_previous = interval_from_previous;
    previous_event_n_intt_raw_hits = n_intt_raw_hits;
    previous_event_is_MBDNS_tight_vtx10cm = is_MBDNS_tight_vtx10cm;
  }

  TCanvas* c1 = new TCanvas("c1", "Title", 900, 600);
  h_temp_all->SetLineColor(kBlack);
  h_temp_all->SetLineWidth(2);
  h_temp_all->Draw();
  h_temp_at_least_1_hit->SetLineColor(kBlack);
  h_temp_at_least_1_hit->SetLineWidth(2);
  h_temp_at_least_1_hit->SetLineStyle(2);
  h_temp_at_least_1_hit->Draw("same");
  h_temp_with_carryover->SetLineColor(kBlue);
  // h_temp_with_carryover->SetLineWidth(5);
  h_temp_with_carryover->Draw("same");
  h_temp_with_carryover_at_least_1_hit->SetLineColor(kBlue);
  h_temp_with_carryover_at_least_1_hit->SetLineStyle(2);
  h_temp_with_carryover_at_least_1_hit->Draw("same");

  TLegend* l = new TLegend(0.3,0.5,0.9,0.7);
  l->AddEntry(h_temp_all,"not cut");
  l->AddEntry(h_temp_at_least_1_hit,"#it{N}_{hit}#geq 1 in original event");
  l->AddEntry(h_temp_with_carryover,"carryover found (not cut)");
  l->AddEntry(h_temp_with_carryover_at_least_1_hit,"carryover found (#it{N}_{hit}#geq 1 in original event)");
  l->Draw("same");

  c1->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/temp_at_least_1_hit.pdf");

  TCanvas* c2 = new TCanvas("c2", "Title", 900, 600);
  int idx=1;
  for (int i=17; i < 127; i++) {
    g->SetPoint(idx, i+0.4, h_temp_with_carryover->GetBinContent(i)/h_temp_all->GetBinContent(i));
    g2->SetPoint(idx, i, h_temp_with_carryover_at_least_1_hit->GetBinContent(i)/h_temp_at_least_1_hit->GetBinContent(i));
    g3->SetPoint(idx, i-0.4, h_temp_with_carryover_at_least_1_hit->GetBinContent(i)/h_temp_all->GetBinContent(i));
    idx++;
  }


  g2->SetMarkerStyle(21);
  g2->SetMarkerSize(1.0);
  g2->SetMarkerColor(kBlue);
  g2->Draw("ap");

  g3->SetMarkerStyle(21);
  g3->SetMarkerSize(1.0);
  g3->SetMarkerColor(kRed);
  g3->Draw("p same");

  g->SetMarkerStyle(20);    
  g->SetMarkerSize(1.0);    
  g->SetMarkerColor(kGreen);
  g->Draw("p same");

  TLegend* l2 = new TLegend(0.3,0.5,0.9,0.7);
  l2->AddEntry(g,"carryover / all");
  l2->AddEntry(g2,"carrover (#it{N}_{hit}#geq 1) /#it{N}_{hit}#geq 1 ");
  l2->AddEntry(g3,"carrover (#it{N}_{hit}#geq 1) / all");
  l2->Draw("same");

  c2->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/hiritu.pdf");


  TCanvas* c3 = new TCanvas("c3", "Title", 900, 600);
  h_misidentified_num_of_carried_hits->Draw();
  c3->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits.pdf");
  
  TCanvas* c4 = new TCanvas("c4", "Title", 900, 600);
  h_misidentified_num_of_carried_hits_short_interval->Draw();
  c4->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits_short_interval.pdf");

  TCanvas* c5 = new TCanvas("c5", "Title", 900, 600);
  h_misidentified_num_of_carried_hits_short_interval_1->Draw();
  c5->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits_short_interval_1.pdf");

  TCanvas* c6 = new TCanvas("c6", "Title", 900, 600);
  h_misidentified_num_of_carried_hits_short_interval_2->Draw();
  c6->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits_short_interval_2.pdf");

  TCanvas* c7 = new TCanvas("c7", "Title", 900, 600);
  h_misidentified_num_of_carried_hits_short_interval_3->Draw();
  c7->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits_short_interval_3.pdf");

  TCanvas* c8 = new TCanvas("c8", "Title", 900, 600);
  h_misidentified_num_of_carried_hits_short_interval_4->Draw();
  c8->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits_short_interval_4.pdf");

  TCanvas* c9 = new TCanvas("c9", "Title", 900, 600);
  h_misidentified_num_of_carried_hits_short_interval_5->Draw();
  c9->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits_short_interval_5.pdf");

  TCanvas* c10 = new TCanvas("c10", "Title", 900, 600);
  h_misidentified_num_of_carried_hits_short_interval_6->Draw();
  c10->Print("/sphenix/tg/tg01/commissioning/INTT/work/ryotaro/commissioning25/CarriedOverHits_v2/output/75519/h_misidentified_num_of_carried_hits_short_interval_6.pdf");


  // int event_id = 2009;
  // tree->GetEntry();



  file->Close();
};


