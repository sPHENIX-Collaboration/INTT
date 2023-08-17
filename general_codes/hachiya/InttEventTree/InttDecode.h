#ifndef __INTTDECODE_H__
#define __INTTDECODE_H__
#include <Event/fileEventiterator.h>
#include <Event/Event.h>
#include <Event/oncsSubevent.h>
#include <iomanip>
#include <vector>
#include <deque>
#include <ostream>

struct BcoHitBuf {
  BcoHitBuf():done(false){};

  void print(){
      std::cout<<"  BCO: 0x"<<std::hex<<bco<<std::dec<<std::endl;
    for(unsigned int i=0; i<vHit.size(); i++){
        std::cout<<"    hit: 0x"<<std::hex<<vHit[i]<<std::dec<<std::endl;
    }
    std::cout<<"    status: "<<(done?"done":"continue")<<std::endl;

  }

  uint64_t    bco;
  std::vector<int> vHit;
  bool        done;
};

struct EventBcoHitBuf{
    EventBcoHitBuf(uint64_t BCO): bco(BCO){
        for(int i=0; i<14; i++){
            bcoHitBuf[i] = NULL;
            status[i]    = false;
        }
    };

    void addBcoHitBuf(int ladder, BcoHitBuf* hitBuf){
      bcoHitBuf[ladder] = hitBuf;
      status[ladder]    = true;
    };

    bool checkStatus(){
        bool allStatus=true;
        for(int i=0; i<14; i++){
            allStatus &= status[i];
        }
        return allStatus;
    };

    void showStatus(){
        std::cout<<"   ";
        for(int i=0; i<14; i++){
            std::cout<<std::setw(3)<<status[i];
        }
        std::cout<<std::endl;
        std::cout<<"   ";
        for(int i=0; i<14; i++){
            int nhits=0;
            if((bcoHitBuf[i])!=NULL) { nhits = bcoHitBuf[i]->vHit.size(); }
            std::cout<<std::setw(3)<<nhits;
        }
        std::cout<<std::endl;
    };

    uint64_t   bco;
    BcoHitBuf* bcoHitBuf[14];
    bool       status[14];
};

/////////////
class InttPacket{
  public:
    InttPacket(int pid, EventBcoHitBuf* evtBcoHitBuf) : packetID(pid) {
        for(int i=0; i<14; i++){ eventNum[i]=-1; bco[i]=0;}
    
        decode(evtBcoHitBuf);
    };

    virtual ~InttPacket();

    int       getPacketID(){return packetID;}
    int       getEventNum(int i=0){return eventNum[i];}
    uint64_t  getBCO(int i=0){return bco[i];}

    void dump(std::ostream& os=std::cout);


    int      iValue(const int hit,const char * what);
    uint64_t lValue(const int hit,const char * what);

    int      iValue(const int hit, const int field);
    uint64_t lValue(const int hit, const int field);

  protected:
    int decode(EventBcoHitBuf* evtBcoHitBuf);

    struct intt_hit
    {
        uint64_t bco;
        uint32_t evt;
        uint16_t fee;
        uint16_t channel_id;
        uint16_t chip_id;
        uint16_t adc;
        uint16_t FPHX_BCO;
        uint16_t full_FPHX;
        uint16_t full_ROC;
        uint16_t amplitude;
        //uint16_t full_fphx;
        uint32_t word;
    };


    int      packetID;
    int      eventNum[16];
    uint64_t bco[16];
    std::vector<intt_hit *> intt_hits;
};


/////////////
class InttDecode {
public:
    InttDecode(fileEventiterator* eventItr, int pID=3001);
    virtual ~InttDecode(){ };
    
    InttPacket* getNextPacket();

    void setDebug(bool flag, int target=6) { 
        debugMode=flag;
        target_lad = target;
    }

    unsigned int getNBadHits(const int ladder) { return nBadHits[ladder]; }
    unsigned int getNGoodHits(const int ladder) { return nGoodHits[ladder]; }
    
protected:
  int                packetID;
  fileEventiterator* evtItr;
  bool               debugMode;

  int target_lad;

  std::deque<int>                     vHitBuf[16];
  std::deque<BcoHitBuf*>              vBcoHitBuf[16];

  std::map<uint64_t, EventBcoHitBuf*> vEvtBcoHitBuf;

  bool endOfFile=false;

  unsigned int nBadHits[16]; // data not belong to anyBCOFULL
  unsigned int nGoodHits[16];// data belong to BCOFULL
};

#endif // __INTTDECODE_H__
