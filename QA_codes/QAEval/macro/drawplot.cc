#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <iostream>
#include "sPhenixStyle.C"

void drawplot()
{
    SetsPhenixStyle();
    TFile* inputFile = TFile::Open("InttQAEval_2.root", "READ");
    if (!inputFile || !inputFile->IsOpen())
    {
        std::cerr << "Failed to open input file: InttQAEval_2.root" << std::endl;
        return;
    }

    TTree* tree = dynamic_cast<TTree*>(inputFile->Get("tree"));
    if (!tree)
    {
        std::cerr << "Failed to get TTree from file: InttQAEval_2.root" << std::endl;
        inputFile->Close();
        delete inputFile;
        return;
    }

    int runnumber;
    int runtime;
    int runmode;
    int nevents;
    double goodchanratio;
    int intt_bco_diff_qa;

    tree->SetBranchAddress("runnumber", &runnumber);
    tree->SetBranchAddress("runtime", &runtime);
    tree->SetBranchAddress("runmode", &runmode);
    tree->SetBranchAddress("nevents", &nevents);
    tree->SetBranchAddress("goodchanratio", &goodchanratio);
    tree->SetBranchAddress("intt_bco_diff_qa", &intt_bco_diff_qa);

    // 히스토그램 생성
    TH1D* hist1 = new TH1D("hist1", "All runs", 100, 0, 4000);
    TH1D* hist2 = new TH1D("hist2", "Run with bco_diff_qa == 1", 100, 0, 4000);
    TH1D* hist3 = new TH1D("hist3", "Golden Runs", 100, 0, 4000);

    std::vector<double> runnumbers_mode0;
    std::vector<double> goodchanratios_mode0;
    std::vector<double> runnumbers_mode1;
    std::vector<double> goodchanratios_mode1;
    
    std::vector<double> runnumbers_AuAu;
    std::vector<double> goodchanratios_AuAu;
    
    std::vector<double> runnumbers_all;
    std::vector<double> goodchanratios_all;
    std::vector<double> runnumbers_all_nocut;
    std::vector<double> goodchanratios_all_nocut;
    int N_totalRun = 0;
    Long64_t total_num = 0;
    int N_GoodRun = 0;
    Long64_t Good_num = 0;
    int N_BadRun = 0;
    Long64_t Bad_num = 0;
    int N_QRun = 0;
    Long64_t Q_num = 0;
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; ++i)
    {

        tree->GetEntry(i);
        if (runnumber >= 46560)
        {
            N_totalRun++;
            total_num += nevents;
            if(runtime<300)
            {
                N_QRun++;
                Q_num += nevents;
            }
            if(runtime>=300 && intt_bco_diff_qa == 1 && goodchanratio >= 80.0)
            {
                N_GoodRun++;
                Good_num += nevents;
            }
            if (runtime >= 300 && (intt_bco_diff_qa == 0 || goodchanratio < 80.0))
            {
                N_BadRun++;
                Bad_num += nevents;
            }
           
            hist1->Fill(runtime);
            if (intt_bco_diff_qa == 1 && runtime >= 300)
            {
                hist2->Fill(runtime);
                if (runtime >= 300 && goodchanratio > 80.0)
                {
                    hist3->Fill(runtime);
                }
                if (runmode == 0 && runnumber < 54000)
                {
                    runnumbers_mode0.push_back(runnumber);
                    goodchanratios_mode0.push_back(goodchanratio);
                }
                else if (runmode == 1 && runnumber < 54000)
                {
                    runnumbers_mode1.push_back(runnumber);
                    goodchanratios_mode1.push_back(goodchanratio);
                }
                else if(runnumber > 54000)
                {
                    runnumbers_AuAu.push_back(runnumber);
                    goodchanratios_AuAu.push_back(goodchanratio);

                }
                runnumbers_all.push_back(runnumber);
                goodchanratios_all.push_back(goodchanratio);
            }
            runnumbers_all_nocut.push_back(runnumber);
            goodchanratios_all_nocut.push_back(goodchanratio);
        }
    }

    TH1D* ratioHist = (TH1D*)hist2->Clone("ratioHist");
    ratioHist->SetTitle("Ratio");
    ratioHist->Divide(hist1);
    ratioHist->GetYaxis()->SetNdivisions(405);
    ratioHist->GetXaxis()->SetNdivisions(405);
    ratioHist->SetXTitle("time [sec]");
    ratioHist->SetYTitle("Ratio");

    TCanvas* canvas = new TCanvas("canvas", "Runtime Histograms", 1500, 1500);
    canvas->cd();
    auto p1 = new TPad("p1", "", 0.0, 0.50, 1.0, 1.0); // Top panel
    p1->SetBottomMargin(0);
    p1->SetTopMargin(0.04);
    p1->SetLeftMargin(0.16);
    p1->SetRightMargin(0.04);
    p1->Draw();

    auto p2 = new TPad("p2", "", 0.00, 0.0, 1.0, 0.50); // Bottom panel
    p2->SetTopMargin(0);
    p2->SetLeftMargin(0.16);
    p2->SetBottomMargin(0.7);
    p2->SetRightMargin(0.04);
    canvas->cd();
    p2->Draw();
    p1->cd();
    hist1->Draw();
    hist1->GetYaxis()->SetNdivisions(405);
    hist1->GetXaxis()->SetNdivisions(405);
    hist1->GetYaxis()->SetRangeUser(0.1, 300);
    hist1->SetXTitle("time [sec]");
    hist1->SetYTitle("Entries");
    hist2->GetYaxis()->SetNdivisions(405);
    hist2->GetXaxis()->SetNdivisions(405);
    hist2->GetYaxis()->SetRangeUser(0.1, 300);
    hist2->SetYTitle("Entries");
    hist2->SetXTitle("time [sec]");
    hist2->SetLineColor(kRed);
    hist2->Draw("SAME");
    p2->cd();
    ratioHist->SetXTitle("time [sec]");
    ratioHist->Draw("P HIST");
    canvas->SaveAs("runtime_histograms_with_ratio.png");

    auto can2 = new TCanvas("canvas2", "Runtime Histograms", 1500, 1500);
    can2->cd();
    hist1->Draw();
    can2->SaveAs("runtime_nocut.png");

    auto can3 = new TCanvas("canvas3", "Runtime Histograms", 1500, 1500);
    can3->cd();
    hist2->Draw();
    can3->SaveAs("runtime_bcocut.png");

    // Golden Runs 히스토그램 그리기
    auto can4 = new TCanvas("canvas4", "Golden Runs", 1500, 1500);
    can4->cd();
    hist3->Draw();
    can4->SaveAs("golden_runs.png");

    // TGraph 생성 및 그리기
    TGraph* graph_mode0 = new TGraph(runnumbers_mode0.size(), &runnumbers_mode0[0], &goodchanratios_mode0[0]);
    graph_mode0->SetTitle("Good Channel Ratio vs Runnumber (runmode = 0)");
    graph_mode0->GetXaxis()->SetTitle("Runnumber");
    graph_mode0->GetYaxis()->SetTitle("Good Channel Ratio [%]");
    graph_mode0->GetYaxis()->SetRangeUser(-0.1, 110);
    graph_mode0->GetXaxis()->SetRangeUser(46000, 56000);
    graph_mode0->SetMarkerStyle(20);
    graph_mode0->SetMarkerColor(kBlue);
    graph_mode0->SetLineColor(kBlack);
    TGraph* graph_mode1 = new TGraph(runnumbers_mode1.size(), &runnumbers_mode1[0], &goodchanratios_mode1[0]);
    graph_mode1->SetTitle("Good Channel Ratio vs Runnumber (runmode = 1)");
    graph_mode1->GetXaxis()->SetTitle("Runnumber");
    graph_mode1->GetYaxis()->SetTitle("Good Channel Ratio [%]");
    graph_mode1->SetMarkerStyle(20);
    graph_mode1->SetMarkerColor(kBlack);
    graph_mode1->SetLineColor(kBlack);

    TGraph* graph_AuAu = new TGraph(runnumbers_AuAu.size(), &runnumbers_AuAu[0], &goodchanratios_AuAu[0]);
    graph_AuAu->SetTitle("Good Channel Ratio vs Runnumber (runmode = 1)");
    graph_AuAu->GetXaxis()->SetTitle("Runnumber");
    graph_AuAu->GetYaxis()->SetTitle("Good Channel Ratio [%]");
    graph_AuAu->SetMarkerStyle(20);
    graph_AuAu->SetMarkerColor(kGreen+1);
    graph_AuAu->SetLineColor(kBlack);


    TGraph* graph_all = new TGraph(runnumbers_all.size(), &runnumbers_all[0], &goodchanratios_all[0]);
    graph_all->SetTitle("Good Channel Ratio vs Runnumber (All)");
    graph_all->GetXaxis()->SetTitle("Runnumber");
    graph_all->GetYaxis()->SetTitle("Good Channel Ratio [%]");
    graph_all->SetMarkerStyle(22);
    graph_all->SetMarkerColor(kWhite);

    TGraph* graph_all_nocut = new TGraph(runnumbers_all_nocut.size(), &runnumbers_all_nocut[0], &goodchanratios_all_nocut[0]);
    graph_all_nocut->SetTitle("Good Channel Ratio vs Runnumber (all_nocut)");
    graph_all_nocut->GetXaxis()->SetTitle("Runnumber");
    graph_all_nocut->GetYaxis()->SetTitle("Good Channel Ratio");
    graph_all_nocut->SetMarkerStyle(22);
    graph_all_nocut->SetMarkerColor(kGreen);
    
    TF1* fitFunc_mode0 = new TF1("fitFunc_mode0", "pol0", 46000, 56000);
    //graph_mode0->Fit(fitFunc_mode0, "RSC rob=0.95");
    
    TF1* fitFunc_mode1 = new TF1("fitFunc_mode1", "pol0", 46000, 56000);
    //graph_mode1->Fit(fitFunc_mode1, "RSC rob=0.95");
    
    TF1* fitFunc_all = new TF1("fitFunc_all", "pol0", 46000, 56000);
    fitFunc_all->SetLineColor(kRed);
    graph_all->Fit(fitFunc_all, "RSC rob=0.95");

    // TCanvas* can5 = new TCanvas("canvas5", "Good Channel Ratio vs Runnumber", 1500, 1500);
    // can5->cd();
    // graph_mode0->Draw("AP");
    // can5->SaveAs("good_channel_ratio_mode0.png");

    // TCanvas* can6 = new TCanvas("canvas6", "Good Channel Ratio vs Runnumber", 1500, 1500);
    // can6->cd();
    // graph_mode1->Draw("AP");
    // can6->SaveAs("good_channel_ratio_mode1.png");

    // TCanvas* can7 = new TCanvas("canvas7", "Good Channel Ratio vs Runnumber", 1500, 1500);
    // can7->cd();
    // graph_all->Draw("AP");
    // can7->SaveAs("good_channel_ratio_all.png");

    TCanvas* can8 = new TCanvas("canvas8", "Good Channel Ratio (BCOQA = GOOD & runtime > 300s)", 1500, 1000);
    can8->cd();
    graph_all->Draw("AP");
    graph_mode0->Draw("P SAME");
    graph_mode1->Draw("P SAME");
    graph_AuAu->Draw("P SAME");
    fitFunc_all->Draw("SAME");
    TLatex *latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.045);
    latex->SetTextAlign(31);
    latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form(" Run24 #it{p+p}, #it{Au+Au} #it{#bf{sPHENIX}} internal"));
    TLegend *leg = new TLegend(0.61, 0.55, 0.81, 0.75);
    leg->AddEntry(graph_mode0, "INTT #it{p+p Trigger Mode}", "PL");
    leg->AddEntry(graph_mode1, "INTT #it{p+p Streaming Mode}", "PL");
    leg->AddEntry(graph_AuAu, "INTT #it{Au+Au Trigger Mode}", "PL");
    leg->AddEntry(fitFunc_all, "Robust (h=0.95) / p0 = 95.7715 ", "L");
    leg->SetTextSize(0.03);
    leg->Draw("SAME");
    can8->SaveAs("good_channel_ratio_bcoqa.png");
    TFile* sfile = new TFile("golden_runs.root", "RECREATE");
    hist3->Write();
    hist2->Write();
    hist1->Write();
    graph_mode0->Write("graph_mode0");
    graph_mode1->Write("graph_mode1");
    graph_all->Write("graph_all");
    graph_all_nocut->Write("graph_all_nocut");
    fitFunc_all->Write();
    fitFunc_mode0->Write();
    fitFunc_mode1->Write();
    sfile->Close();

    std::cout<<"total Run and events : " << N_totalRun << " " << total_num << std::endl; 
    std::cout<<"GOOD Run and events : " << N_GoodRun << " " << Good_num << std::endl; 
    std::cout<<"Q Runs : " << N_QRun << " " << Q_num << std::endl;  
    std::cout<<"BAD Runs : " << N_BadRun << " " << Bad_num << std::endl; 
  
    delete hist1;
    delete hist2;
    delete hist3;
    delete ratioHist;
    delete canvas;
    delete can2;
    delete can3;
    delete can4;
    // delete can5;
    // delete can6;
    // delete can7;
    delete graph_mode0;
    delete graph_mode1;
    delete graph_all;

    inputFile->Close();
    delete inputFile;
}