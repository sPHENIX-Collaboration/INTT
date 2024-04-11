#include <TFile.h>
#include <cdbobjects/CDBTTree.h>
#include <iostream>
#include <TFile.h>
#include <TDirectoryFile.h>
#include <TH1.h>
#include <TH2D.h>
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"




void drawBCOHisto(int runnumber = 20869)
{
    SetsPhenixStyle();
 //   gStyle->SetOptStat(0); // Turn off the statistics box
    TFile *of = new TFile(Form("bco_%d_test_fun4all.root",runnumber));
    TCanvas *can = new TCanvas("can", "can", 800, 800);
    can->Divide(4, 2);

    TH1D *h1_histo[8];
    TH2D *h2_histo[8];
    double max_value = 0;
    for (int i = 0; i < 8; i++)
    {
        h2_histo[i] = (TH2D *)of->Get(Form("h2_bco_felix_%d", i));
        h1_histo[i] = (TH1D *)h2_histo[i]->ProjectionY();
        h1_histo[i]->SetLineColor(i/4+i + 1);
        h1_histo[i]->SetLineWidth(3);
        can->cd(i + 1);
        h1_histo[i]->Draw();
        cout << max_value << endl;
        max_value = (max_value > h1_histo[i]->GetMaximum()) ? max_value : h1_histo[i]->GetMaximum();
        cout << max_value << endl;
        //        h1_histo[i] = new TH1D(Form("BCO_Felix_%d", i), "INTT BCO", 128, 0, 128);
    }

   TCanvas can2; 
    can2.cd();
    TLegend leg(.55, .75, .85, .93);
    leg.AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg.AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "");
    leg.AddEntry("", Form("Run %d",20869), "");
    TLegend leg2(.20, .70, .40, .93);
    TLegend leg3(.40, .70, .60, .93);
    for (int i = 0; i < 8; i++)
    {
        if (i < 4)
            leg2.AddEntry(h1_histo[i], Form("FELIX %d", i),"l");
        else
            leg3.AddEntry(h1_histo[i], Form("FELIX %d", i),"l");
        if (i != 0)
            h1_histo[i]->Draw("same");
        else
        {
            h1_histo[i]->SetMaximum(max_value * 1.3);
            h1_histo[i]->Draw();
        }
    }
    leg.Draw();
    leg2.Draw();
    leg3.Draw();
    can2.RedrawAxis();    
    can2.Print("plot2.png");    
    can->SaveAs("plot.png");
}