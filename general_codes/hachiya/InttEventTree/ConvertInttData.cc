
#include "InttEvent.h"
#include "ConvertInttData.h"
#include "InttDecode.h"

#include <iostream>
#include <fstream>
#include "Event/fileEventiterator.h"

#include <TFile.h>
#include <TTree.h>

int init_done = 0;

using namespace std;

//const bool debug=true;
const bool debug=false;

struct LadderInfo
{
    int barrel;
    int layer;
    int ladder;
};

int module_map_flag = 1;
struct LadderInfo felix_map[8][14];


InttEvent* inttEvt = nullptr;

TFile* tree_file = nullptr;
TTree* tree = nullptr;

int Init(const char* outRootfile)
{
  if (init_done) return 1;
  init_done = 1;


  tree_file = TFile::Open(outRootfile, "recreate");


  inttEvt = new InttEvent();

  tree = new TTree("tree", "tree");
  //tree->Branch("event", &inttEvt, 8000, 99);
  tree->Branch("event", "InttEvent", &inttEvt, 8000, 99);
  //tree->Branch("event", "InttEvent", &inttEvt);
  //tree->Branch("InttEvent", &inttEvt);

  //module_map();

  return 0;
}

int Process_event (InttPacket * p)
{
    if(module_map_flag)
    {
        module_map();
        module_map_flag = 0;
    }


    int nHitTotal = 0;
    inttEvt->clear();

    for(int iladder=0; iladder<14; iladder++){
      inttEvt->evtSeq = p->getEventNum(iladder);
      inttEvt->bco    = p->getBCO(iladder);
      if(inttEvt->evtSeq>=0) break;
    }
    if(debug)
        cout<<" "<<inttEvt->evtSeq<<" "<<hex<<inttEvt->bco<<" "<<p->getBCO()<<dec<<endl;

    //for(int id = 3001; id < 3009; ++id)
    {
        //p = e->getPacket(id);
        //
        if (p)
        {
            int N = p->iValue(0, "NR_HITS");
            for(int i = 0; i < N; ++i)
            {
                //if((p->iValue(i, "DATAWORD") >> 0xf) & 0x1)continue;

                InttHit* hit = inttEvt->addHit();

                hit->pid = p->getPacketID();

                hit->adc     = p->iValue(i, "ADC");
                hit->ampl    = p->iValue(i, "AMPLITUDE");
                hit->chip_id = p->iValue(i, "CHIP_ID");
                if(hit->chip_id>=27)  hit->chip_id -= 26;

                hit->module  = p->iValue(i, "FEE");    //felix port, martin might change the name
                hit->chan_id = p->iValue(i, "CHANNEL_ID");
                ////fem
                hit->bco      = p->iValue(i, "FPHX_BCO");
                hit->bco_full = p->lValue(i, "BCO");
                //cout<<"chip : "<<hit->adc<<" "<<hit->chip_id<<" "<<chip<<" "<<hit->chip_id-chip<<" "<<hit->chan_id<<endl;

                hit->evt     = p->iValue(i, "EVTCTR");

                //hit->felix = p->getPacketID() % 100
                hit->roc = 2 * (p->getPacketID() - 3001);
                if(hit->module > 6) hit->roc++;
                hit->roc %= 8;
                hit->arm = (p->getPacketID() - 3001) / 4;

                if(0<=hit->module&&hit->module<14){
                    hit->barrel = felix_map[p->getPacketID() - 3001][hit->module].barrel;
                    hit->layer  = felix_map[p->getPacketID() - 3001][hit->module].layer;
                    hit->ladder = felix_map[p->getPacketID() - 3001][hit->module].ladder;
                } else {
                    hit->barrel = 0;
                    hit->layer  = 0;
                    hit->ladder = 0;
                    cout<<"module : "<<hit->module<<endl;
                }
                ////direction = (id / 100 + 1) % 2;

                hit->full_fphx = p->iValue(i, "FULL_FPHX");
                hit->full_roc  = p->iValue(i, "FULL_ROC");

                nHitTotal++;
            }

            delete p;
        }
    }
    //inttEvt->show();
    inttEvt->sort();
    //inttEvt->show();
    tree->Fill();

    return 0;
}

int Run(const char *evtFile){

    for(int id = 3001; id < 3009; ++id)
    //for(int id = 3002; id < 3003; ++id)
    {
        cout<<"packet = "<<id<<endl;
        fileEventiterator *evtItr = new fileEventiterator(evtFile);

        InttDecode decode(evtItr, id);
        decode.setDebug(debug, 6);

        int nevt=0;
        InttPacket *p=nullptr;
        while(p=decode.getNextPacket()){
            if((nevt%10000)==0) cout<<"nevt : "<<nevt<<endl;
            if(debug) p->dump();
            Process_event(p);
            nevt++;

            //if(nevt>=1000) {
            //  break;
            //  //decode.setDebug(false,6);
            //}
        }

        cout<<"    "<<nevt<<endl;
        if(nevt>0){
          cout<<"NBad Hits    "<<endl;
          for(int ilad=0; ilad<14; ilad++){
              cout<<"   lader="<<setw(3)<<ilad
                  <<", ngood "<<setw(20)<<decode.getNGoodHits(ilad)
                  <<", nbad " <<setw(20)<<decode.getNBadHits(ilad)<<endl;
          }
        }

        delete evtItr;
    }

    tree->Write();
    tree_file->Write();
    tree_file->Close();

    return 0;
}

int module_map(std::string path)
{
    std::string filename;
    std::string line;

    std::fstream map_file;

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


