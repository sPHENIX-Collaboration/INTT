#ifndef __INTTOFFLINECLUSTER_H_
#define __INTTOFFLINECLUSTER_H_

#include <iostream>
#include <TClonesArray.h>

using namespace std;

class InttOfflineCluster : public TObject {
  public:
    InttOfflineCluster(){};
    virtual ~InttOfflineCluster(){};

    void copy(InttOfflineCluster& hit );
    void show(bool crflag=true);

  public:
    int  hitsetkey;
    int  clusterid;

    float local[2];          //< 2D local position [cm] 2 * 32 64bit  - cumul 1*64
    float global[3];          //< 2D local position [cm] 2 * 32 64bit  - cumul 1*64
    unsigned short int adc;  //< cluster sum adc 16
    char phisize; // 8bit
    char zsize;   // 8bit
    char overlap; // 8bit 
    char edge;    // 8bit - cumul 2*64

  ClassDef(InttOfflineCluster, 2)

};

class InttOfflineClusterList : public TObject {
  public:
    InttOfflineClusterList();
    virtual ~InttOfflineClusterList(){ };

    InttOfflineCluster* addCluster(); 

    void            clear();
    void            copy(InttOfflineClusterList* org);
    //void show() {}

    int                 getNClusters();
    InttOfflineCluster* getCluster(const int iclus);

  public:
    int   fNhits;
    TClonesArray* fhitArray;

  ClassDef(InttOfflineClusterList, 2)
};


#endif  // __INTTCLUSTER_H_
