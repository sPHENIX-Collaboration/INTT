#ifndef __INTTCLUSTER_H_
#define __INTTCLUSTER_H_

#include "InttEvent.h"

#include <iostream>

using namespace std;

class InttClusterList;

void Clustering (InttEvent* inttEvent, InttClusterList* clusterList);

class InttCluster {
  public:
    InttCluster(int Module, int Chip_id, float Ch, int Adc, int Nhits)
        :module(Module), chip_id(Chip_id), ch(Ch), adc(Adc), nhits(Nhits){};
    virtual ~InttCluster(){};

    void show(bool crflag=true);

  public:
    int      module;
    int      chip_id;
    float    ch;
    int      adc;
    int      nhits;

  public:   
    int      pid;
    int      ampl;
    //fpga

    int      bco;
    Long64_t bco_full;

    int      evt;

    int      roc;
    int      barrel;
    int      layer;
    int      ladder;
    int      arm;
};

class InttClusterList {
  public:
    InttClusterList():fNhits(0) {};
    virtual ~InttClusterList(){ clear(); };

    void show() { /*cout<<"  mod, chip, ch, adc, nhits= "<<module<<" "<<chip_id<<" "<<ch<<" "<<adc<<" "<<nhits<<endl;*/}

    void clear() {
      for(auto itrClus=fvCluster.begin(); itrClus!=fvCluster.end(); ++itrClus)
      {
          InttCluster* clus = *itrClus;
          delete clus;
      }
      fvCluster.clear();
    }

    void Clustering(vector<InttHit*>& vHit);

    int          getNhits() { return fNhits; }
    InttCluster* getCluster(const int iclus) { 
        if( 0<=iclus && iclus < fvCluster.size() ) return fvCluster[iclus];
        else                                       return nullptr; 
    }

  public:
    int   fNhits;
    vector<InttCluster*> fvCluster;

};


#endif  // __INTTCLUSTER_H_
