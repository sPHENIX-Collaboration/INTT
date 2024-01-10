
#include <iostream>
#include <fstream>
#include <iomanip>
#include <pmonitor/pmonitor.h>
#include "myfirstproject.h"

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>

int init_done = 0;

using namespace std;

struct LadderInfo
{
    int barrel;
    int layer;
    int ladder;
};

struct LadderInfo felix_map[8][14];

int pid;

int adc;
int amp;
int chp;

//fpga
int module;

int chn;
//fem

int bco;
Long64_t bco_full;

int evt;

int roc;
int barrel;
int layer;
int ladder;
int direction;

int full_fphx;
int full_roc;

std::fstream map_file;
TFile* file = nullptr;
TTree* tree = nullptr;

int setup(std::string const& filename)
{
    module_map();

    file = TFile::Open(filename.c_str(), "RECREATE");
    if(!file)
    {
        printf("Could not (re)create ROOT file:\n\t%s\nExiting\n", filename.c_str()); 
        return 1;
    }

    file->cd();

    if(tree)return 0;
    tree = new TTree("tree", "tree");
    
    tree->Branch("pid", &pid);
    
    tree->Branch("adc", &adc);
    tree->Branch("ampl", &amp);
    tree->Branch("chip_id", &chp);
    
    tree->Branch("module", &module);
    tree->Branch("chan_id", &chn);
    
    tree->Branch("bco", &bco);
    tree->Branch("bco_full", &bco_full);
    
    tree->Branch("event", &evt);
    
    tree->Branch("roc", &roc);
    tree->Branch("barrel", &barrel);
    tree->Branch("layer", &layer);
    tree->Branch("ladder", &ladder);
    tree->Branch("arm", &direction);
    
    tree->Branch("full_fphx", &full_fphx);
    tree->Branch("full_roc", &full_roc);

    return 0;
}

int pinit()
{
    if (init_done) return 1;
    init_done = 1;
    
    return 0;
}

int process_event (Event * e)
{
    Packet *p = nullptr;
    for(int id = 3001; id < 3009; ++id)
    {
          p = e->getPacket(id);
          if (p)
          {
            int N = p->iValue(0, "NR_HITS");
            //if(N)std::cout << "Num hits: " << N << std::endl;
    	    //if(N)std::cout << "Num hits: " << std::setw(10) << N << "\r";
            for(int i = 0; i < N; ++i)
            {
	      
                //if((p->iValue(i, "DATAWORD") >> 0xf) & 0x1)continue;

                pid = id;

                adc = p->iValue(i, "ADC");
                amp = p->iValue(i, "AMPLITUDE");
                chp = p->iValue(i, "CHIP_ID");
		//chp = (chp-1) % 26 + 1; // p->iValue(i, "CHIP_ID") starts from 0, but our definition of chip_id starts from 1 and ends at 26. I added +1 to convert it.
		chp = (chp<=26) ? chp : chp - 26;
                module = p->iValue(i, "FEE");    //felix port, martin might change the name
                chn = p->iValue(i, "CHANNEL_ID");
                //fem
                bco = p->iValue(i, "FPHX_BCO");
                bco_full = p->lValue(i, "BCO");
                ++evt;
 
                //felix = id % 100
                roc = 2 * (id - 3001);
                if(module > 6)++roc;
                roc %= 8;

                barrel = felix_map[id - 3001][module].barrel;
                layer = felix_map[id - 3001][module].layer;
                ladder = felix_map[id - 3001][module].ladder;
                direction = (id - 3001) / 4;
                //direction = (id / 100 + 1) % 2;

                full_fphx = p->iValue(i, "FULL_FPHX");
                full_roc = p->iValue(i, "FULL_ROC");
    
                tree->Fill();
            }
    
            delete p;
          }
    }

return 0;
}

int module_map(std::string path)
{
    std::string filename;
    std::string line;

    for(int felix = 0; felix < 8; ++felix)
    {
        filename = path;
        filename += "intt";
        filename += std::to_string(felix);
        filename += "_map.txt";

        map_file.open(filename, std::ios::in);
        if(!map_file.is_open())
        {
            std::cout << "Could not open file:" << std::endl;
            std::cout << "\t" << filename << std::endl;
            std::cout << "\t(You can specify the path to the map files)" << std::endl;
            return 1;
        }

        int felix_channel;
        char ldr_name[16];
        struct LadderInfo ladder_info;

        //std::cout << felix << std::endl;
        while(map_file)
        {
            std::getline(map_file, line);

            while(line.find("#") != std::string::npos)
            {
                line = line.substr(0, line.find("#"));
            }

            while(std::isspace(line[0]))
            {
                line = line.substr(1); 
            }

            if(line.empty())continue;

            sscanf(line.c_str(), "%d %*s %s", &felix_channel, ldr_name);
            sscanf(ldr_name, "B%dL%d", &(ladder_info.barrel), &(ladder_info.ladder));
            ladder_info.layer = ladder_info.ladder / 100;
            ladder_info.ladder %= 100;

            felix_map[felix][felix_channel] = ladder_info;

            //std::cout << "\t" << felix_channel << " -> " << ladder_info.barrel << " " << ladder_info.layer << " " << ladder_info.ladder << std::endl;
        }

        map_file.close();
    }

    return 0;
}


