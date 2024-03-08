#include "InttDeadMapTest.h"
#include "InttDeadMapTest.cc"
#include "InttBCOFilterv1.h"
#include "InttBCOFilterv1.cc"

void testrun()
{
    InttDeadMapTest test;
    InttBCOFilterv1 test2;

    test.LoadCDBFile("/sphenix/tg/tg01/commissioning/INTT/work/jaein/CDBTest/macros/CDBTest/InttHotDeadMap_cdb-00020869_30.root");
    test2.LoadCDBFile("/sphenix/tg/tg01/commissioning/INTT/QA/bco_bcofull_difference/CDB/2023/cdb_bco_20869.root");
    TH1D* hist = new TH1D("hist","hist",10,0,10);
    TH1D* hist2 = new TH1D("hist2","hist2",10,0,10);
    TH2D* hist3[8];
    for(int felix_server=0;felix_server<8;felix_server++)
    {
        hist3[felix_server] = new TH2D(Form("hist3_%d",felix_server),Form("hist3_%d",felix_server),14,0,14,128,0,128);
        for(int felix_channel=0;felix_channel<14;felix_channel++)
        {
            for(int chip=0;chip<26;chip++)
            {
                for(int channel=0;channel<128;channel++)
                {
                    int flag = -1;
                    flag = test.isBad(felix_server,felix_channel,chip,channel);
                    hist->Fill(flag);
                    if(flag)
                    {
                        hist2->Fill(flag);
                    }
                    if(test2.isBCOPeakTest(felix_server,felix_channel,channel))
                    {
                        hist3[felix_server]->Fill(felix_channel,channel);
                    }
                }
            }
        }
    } 
    TFile* sfile = new TFile("test.root","RECREATE");
    sfile->cd();
    hist->Write();
    hist2->Write();
    for(int i=0;i<8;i++)
    {
        hist3[i]->Write();
    }
    sfile->Close();
    return;
}
