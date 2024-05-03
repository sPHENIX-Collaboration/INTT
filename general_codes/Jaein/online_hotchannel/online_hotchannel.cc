#include "Mapping.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "TFile.h"
#include "TH2.h"
using namespace std;

std::map<std::string, int> port_to_id_map = {
    {"A1", 0}, {"A2", 1}, {"A3", 2}, {"B1", 3}, {"B2", 4}, {"B3", 5}, {"C1", 6}, {"C2", 7}, {"C3", 8}, {"D1", 9}, {"D2", 10}, {"D3", 11}};

//////Constant values////////
const int felix = 8;
const int chip = 26;
const int module = 14;
const int channel = 128;
const int nchannel = 372736;
///////// Input file & hitmap //////
TFile* hitmapfile;
TH2D* hitmap[felix][module];

////////// Output hot chan list //////
std::ofstream outfile_South; 
std::ofstream outfile_North;

////// Global variables /////////////
uint64_t average_hit = 0;
std::string hitmap_path = "/sphenix/tg/tg01/commissioning/INTT/QA/hitmap/2024/";

void ReadHitMap(string inputfile)
{
    hitmapfile = new TFile(inputfile.c_str(), "READ");
    for (int i = 0; i < felix; i++)
    {
        for (int j = 0; j < module; j++)
        {
            hitmap[i][j] = (TH2D *)hitmapfile->Get(Form("HitMap_%d_%d", i, j));
        }
    }
}

uint64_t CalculateAverageHit()
{
    uint64_t total_hit = 0;
    for (int i = 0; i < felix; i++)
    {
        for (int j = 0; j < module; j++)
        {
            total_hit += hitmap[i][j]->GetEntries();
        }
    }
    return total_hit/nchannel; 
}

int MakeHotChannelListFromHitMap(int felix_, int module_, double hot_channel_cut)
{
    int nhot = 0;
    uint64_t nhits = 0;
    std::string server_name = "intt"+std::to_string(felix_)+"_"+std::to_string(module_);
    uint64_t total_hits = hitmap[felix_][module_]->GetEntries();
    double hot_chan_cut = hot_channel_cut * total_hits;
    for (int i = 0; i < chip; i++)
    //NOTE : CHIP ID in InttRawHit : 0~25;
    // For the convention of online masking, CHIP ID is 1~26
    {
        for (int j = 0; j < channel; j++)
        {
            nhits = hitmap[felix_][module_]->GetBinContent(i + 1, j + 1);
            if (nhits > hot_chan_cut && nhits > average_hit)
            {
                nhot++;
                int ROC_ID = serverFC_toinfo_map.at(server_name).ROC;
                std::string port = serverFC_toinfo_map.at(server_name).Port;
                int port_ID = port_to_id_map[port];
                // ROC_ID ; Felix_Channel(module) ; Port_ID ; CHIP_ID ; Chan_ID
                if (felix_ < 4)
                    outfile_South << ROC_ID << " " << module_ << " " << port_ID << " " << i + 1 << " " << j << " "
                                  << "0 0 0 0 0" << std::endl;
                else
                    outfile_North << ROC_ID << " " << module_ << " " << port_ID << " " << i + 1 << " " << j << " "
                                  << "0 0 0 0 0" << std::endl;
            }
        }
    }
    return nhot;
}

void online_hotchannel(int runnumber = 38994)
{
    ostringstream oss;
    oss << "hitmap_run" << setw(8) << setfill('0') << runnumber << ".root";
    std::string filename = hitmap_path + oss.str();
    
    std::cout << filename << std::endl;
    ReadHitMap(filename);
    average_hit = CalculateAverageHit(); 
    std::string outname_south = "hotlist_south_"+std::to_string(runnumber)+".txt"; 
    std::string outname_north = "hotch_north_"+std::to_string(runnumber)+".txt";
    outfile_South.open(outname_south);
    outfile_North.open(outname_north); 
    
    // If the # of hits from the channel occupiled more than 30% of total hits on one half-ladder, will be classified as hot
    double hot_channel_cut = 0.3;

    for (int i = 0; i < felix; i++)
    {
        for (int j = 0; j < module; j++)
        {
            MakeHotChannelListFromHitMap(i, j, hot_channel_cut);
        }
    }
    outfile_South.close();
    outfile_North.close();
    return 0;
}
