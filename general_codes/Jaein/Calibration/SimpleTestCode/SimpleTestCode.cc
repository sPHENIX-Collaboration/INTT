#include "InttEvent.h"
#include "InttEvent.cc"
#include "InttBCOMap.h"
#include "InttBCOMap.cc"
#include "TH2D.h"
#include <cdbobjects/CDBTTree.h>
#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libintt.so)

std::string inputFilePath = "/sphenix/tg/tg01/commissioning/INTT/data/root_files/2023/"; 


TH2D *h2_bco_ladder[8];
TH2D *h2_bco_ladder_cut[8];

int n_event = 10000;

void SimpleTestCode()
{

    gROOT->SetBatch(kTRUE);
    recoConsts *rc = recoConsts::instance();
    rc->set_StringFlag("CDB_GLOBALTAG", "jaein213");
    rc->set_uint64Flag("TIMESTAMP", 20869);
    for (int j = 0; j < 8; j++)
    {
        h2_bco_ladder[j] = new TH2D(Form("h2_bco_felix_%d", j), Form("h2_bco_felix_%d", j), 14, 0, 14, 128, 0, 128);
        h2_bco_ladder_cut[j] = new TH2D(Form("h2_bco_felix_cut%d", j), Form("h2_bco_felix_cut%d", j), 14, 0, 14, 128, 0, 128);
    }

    InttBCOMap bcomap;
//    bcomap.LoadFromFile("/sphenix/tg/tg01/commissioning/INTT/QA/bco_bcofull_difference/CDB/2023/cdb_bco_20869.root");
    bcomap.LoadFromCDB("INTT_BCOMAP");

    TFile *f;
    std::string inputfile = inputFilePath + "beam_intt0-00020869-0000_event_base.root";

    f = TFile::Open(inputfile.c_str());
    if (!f)
    {
        std::cout<<"ROOT file are not found!"<<std::endl;
        return;
    }
    TTree *tree = (TTree *)f->Get("tree");

    int felixserver = 0;
    InttEvent *evt = new InttEvent();
    tree->SetBranchAddress("event", &evt);
    for (int j = 0; j < n_event; j++)
    {
        tree->GetEvent(j);
        for (int k = 0; k < evt->getNHits(); k++)
        {
            auto hit = evt->getHit(k);
            int in_chip = hit->chip_id;
            int in_chan = hit->chan_id;
            int in_module = hit->module;
            int in_bco = hit->bco;
            int in_adc = hit->adc;
            Long64_t lo_bco_full = hit->bco_full;
            int bco_diff = (lo_bco_full & 0x7F) - in_bco;
            if (bco_diff < 0)
            {
                h2_bco_ladder[felixserver]->Fill(in_module, bco_diff + 128);
                if (!bcomap.IsBad(felixserver, in_module, lo_bco_full, in_bco))
                {
                    h2_bco_ladder[felixserver]->Fill(in_module, bco_diff + 128);
                }
            }
            else
            {
                h2_bco_ladder[felixserver]->Fill(in_module, bco_diff);
                if (!bcomap.IsBad(felixserver, in_module, lo_bco_full, in_bco))
                {
                    h2_bco_ladder_cut[felixserver]->Fill(in_module, bco_diff);
                }
            }
        }
    }
    TCanvas* can = new TCanvas("can","can",800,800);
    can->Divide(2);
    can->cd(1);
    h2_bco_ladder[0]->Draw("colz");
    can->cd(2);
    h2_bco_ladder_cut[0]->Draw("colz");
    can->Update();
    can->SaveAs("plot.pdf");
    return;
}

