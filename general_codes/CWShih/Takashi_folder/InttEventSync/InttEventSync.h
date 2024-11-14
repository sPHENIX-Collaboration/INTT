#ifndef __INTTEVENTSYNC_H__
#define __INTTEVENTSYNC_H__

#include <vector>
#include <map>
#include <stdint.h>
#include <cstddef>

class InttEvent;
class TTree;

struct EventBuf{
    EventBuf(uint64_t BCO, int EVT): bco(BCO), evtSeq(EVT){
        for(int i=0; i<8; i++){
            inttEvtBuf[i] = NULL;
            status[i]    = false;
        }
    };

    void addInttEvent(int idata, InttEvent* inttEvt){
      inttEvtBuf[idata] = inttEvt;
      status[idata]    = true;
    };

    bool checkStatus(){
        bool allStatus=true;
        for(int i=0; i<8; i++){
            allStatus &= status[i];
        }
        return allStatus;
    };

    void showStatus();

    int        evtSeq;
    uint64_t   bco;
    InttEvent* inttEvtBuf[8];
    bool       status[8];
};

class InttEventSync{
public:
    InttEventSync();
    virtual ~InttEventSync(){};

    void addData(TTree* tree){ vTree.push_back(tree); }

    InttEvent* getNextEvent();

protected:
    void initBranch();

    std::vector<TTree*> vTree;

    bool initDone;
    
    InttEvent* inttEvt[8];
    int        iEvent[8];

    std::map<uint64_t, EventBuf*> vEvtBuf;
};

#endif // __INTTEVENTSYNC_H__
