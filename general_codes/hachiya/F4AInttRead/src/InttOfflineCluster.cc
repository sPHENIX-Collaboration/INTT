
#include "InttOfflineCluster.h"

#include <iostream>
#include <iomanip>

using namespace std;

ClassImp(InttOfflineCluster)
ClassImp(InttOfflineClusterList)


void InttOfflineCluster::copy(InttOfflineCluster& hit) { 
    hitsetkey = hit.hitsetkey; 
    clusterid = hit.clusterid;

    local[0] = hit.local[0];
    local[1] = hit.local[1];
    global[0] = hit.global[0];
    global[1] = hit.global[1];
    global[2] = hit.global[2];
    adc     = hit.adc;
    phisize = hit.phisize;
    zsize   = hit.zsize;  
    overlap = hit.overlap;
    edge    = hit.edge;   
}

void InttOfflineCluster::show(bool /*crflag*/) { 
  //cout<<"  mod, chip, ch, adc, nhits= "<<module<<" "<<chip_id<<" "<<ch<<" "<<adc<<" "<<nhits;
  //if(crflag)  cout <<endl;
}

InttOfflineClusterList::InttOfflineClusterList(): fNhits(0), fhitArray(NULL) {
  fhitArray = new TClonesArray("InttOfflineCluster", 500);
  cout<<"ctor InttOfflineClusterList"<<endl;
}

InttOfflineCluster* InttOfflineClusterList::addCluster(){
  //cout<<"nhits: "<<fNhits<<endl;
  TClonesArray& hitArray = *fhitArray;
  InttOfflineCluster* hitnew = new(hitArray[fNhits++]) InttOfflineCluster();

  //cout<<"nhits: before return "<<fNhits<<endl;
  return hitnew;
}

int InttOfflineClusterList::getNClusters(){
  return fNhits;//m_hitArray->GetEntries();
}

InttOfflineCluster* InttOfflineClusterList::getCluster(const int ihit){
  return (ihit<getNClusters()) ? (InttOfflineCluster*)fhitArray->UncheckedAt(ihit) : NULL;
}

void InttOfflineClusterList::clear() {
  fhitArray->Clear(); 
  fNhits=0;
};

void InttOfflineClusterList::copy(InttOfflineClusterList* org) {
  if(org==NULL) return;

  clear();

  for(int ihit=0; ihit<org->getNClusters(); ihit++){
      InttOfflineCluster* hitnew = addCluster();
      InttOfflineCluster* hit    = org->getCluster(ihit);
      hitnew->copy(*hit);
      //cout<<"debug hit: "<<endl;
      //hitnew->show();
      //hit->show();
  }
};

/*
void InttOfflineEvent::show() {
  cout<<"  Nhits : "<<fNhits<<endl;

  for(int ihit=0; ihit<fNhits; ihit++){
    InttOfflineHit* hit = getHit(ihit);
    hit->show((ihit==0));
  }
};

void InttOfflineEvent::sort() {
  fhitArray->Sort();
};
*/
