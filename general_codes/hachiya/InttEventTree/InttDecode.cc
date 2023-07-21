#include "InttDecode.h"
#include "string.h"

using namespace std;

void fillBcoHitBuf(deque<int>& vHitBuf, deque<BcoHitBuf*>* vBcoHitBuf, bool debug=false)
{
    BcoHitBuf* bcoHitBuf = (vBcoHitBuf->size()>0 ? vBcoHitBuf->back() : NULL);

    int nNewBcoHitBuf=0;
    if(bcoHitBuf!=NULL&&!bcoHitBuf->done) nNewBcoHitBuf++; //  to print "not done hit in previous event"

    while(vHitBuf.size()>0){
        int w = vHitBuf[0]; vHitBuf.pop_front();
        //--if(debug) cout<<" w : 0x"<<hex<<w<<dec;

        uint64_t ltmp;
        if((w&0xFF00FFFF) == 0xAD00CADE) {   // find hit header
            //--if(debug) cout<<"  : hit header"<<endl;

            if(bcoHitBuf!=NULL){
                bcoHitBuf->done=true;
            }

            // prepare new BcoHit
            if(vHitBuf.size()<=1){ // size is not enough to reconstruct BCO, then break
                vHitBuf.push_front(w);
                break;
            }

            // reconstruct new BCO
            uint64_t ltmp = ((w>>16)&0xFF);
            uint64_t BCO  = (ltmp<<32);

            w = vHitBuf[0]; vHitBuf.pop_front(); // next word is also BCO
            ltmp = w;
            BCO |= ( (ltmp&0xFFFF) <<16); 
            BCO |= ( (ltmp>>16) &0xFFFF);

            bcoHitBuf = new BcoHitBuf();
            bcoHitBuf->bco = BCO;
            vBcoHitBuf->push_back(bcoHitBuf);
            nNewBcoHitBuf++; // for debug

            //--if(debug) cout<<" w : 0x"<<hex<<w<<dec<<"  : BCO: 0x"<<hex<<BCO<<dec;
        }
        else {
            if(bcoHitBuf!=NULL) bcoHitBuf->vHit.push_back(w);
        }
        //--if(debug) cout<<endl;
    }
    // debug
    if(debug)
    {
        for(int inew=vBcoHitBuf->size()-nNewBcoHitBuf; inew<vBcoHitBuf->size(); inew++){
            vBcoHitBuf->at(inew)->print();
        }
    }
    if(debug) cout<<"-- hitbuf size ="<<vHitBuf.size()<<" "<<vBcoHitBuf->size()<<endl;
}

bool checkReadyToDecode(map<uint64_t, EventBcoHitBuf*>& vEvtBcoHitBuf){
    // check if first evtBuf is ready to decode
    //   condition: 
    //       evtBuf AllStatus = 1 or 
    //       there is an evtBuf within next 10 evtBuf
    //       if not ready if next 10 evt buf, then the evtBuf is forced to process
    if(vEvtBcoHitBuf.size()==0) return false;
 
    auto evtBufItr = vEvtBcoHitBuf.begin();

    EventBcoHitBuf* firstEvtBuf = evtBufItr->second;
    bool readyToDecode = firstEvtBuf->checkStatus();
    if(!readyToDecode){
        static const int MAXEVENTS=10;
        evtBufItr++;
        for(int ievt=0; evtBufItr!=vEvtBcoHitBuf.end(); evtBufItr++, ievt++){
            readyToDecode = evtBufItr->second->checkStatus();

            if(ievt>=MAXEVENTS) readyToDecode=true;

            if(readyToDecode) break;
        }
    }
    return readyToDecode;
}
/////////////

InttPacket::~InttPacket(){
    vector<intt_hit *>::iterator hit_itr;
    for(hit_itr=intt_hits.begin(); hit_itr!=intt_hits.end(); ++hit_itr)
    {
        delete (*hit_itr);
    }
    intt_hits.clear();
}

enum ITEM
{
    F_BCO = 1,
    F_EVTCTR,
    F_FEE,
    F_CHANNEL_ID,
    F_CHIP_ID,
    F_ADC,
    F_FPHX_BCO,
    F_FULL_FPHX,
    F_FULL_ROC,
    F_AMPLITUDE,
    F_DATAWORD
};


void  InttPacket::dump ( ostream& os )
{
    map<int, int> vEvtMap;
    for ( int iladder = 0; iladder < 14; iladder++){
        auto itr = vEvtMap.find(eventNum[iladder]);
        if(itr==vEvtMap.end()){
            vEvtMap.insert(make_pair(eventNum[iladder], 1));
        }
        else {
            itr->second++;
        }
    }
    os<<" Nevt : ";
    for(auto itr=vEvtMap.begin(); itr!=vEvtMap.end(); ++itr){
    os<<itr->first<<"("<<itr->second<<") ";
    }
    os<<endl;


    os << "  Number of hits: " << iValue(0, "NR_HITS") << endl;

    //  std::vector<intt_hit*>::const_iterator hit_itr;

    os << "   #    FEE    BCO      chip_BCO  chip_id channel_id    ADC  full_phx full_ROC Ampl." << endl;

    for ( int i = 0; i < iValue(0, "NR_HITS"); i++)
    {
        os << setw(4) << i << " "
            << setw(5) <<             iValue(i, F_FEE)     << " "
            <<  hex <<  setw(11) <<   lValue(i, F_BCO)  << dec << "   "
            <<  hex <<  setw(2)  << "0x" << iValue(i,F_FPHX_BCO)  << dec  << "   "
            <<          setw(5)  <<    iValue(i,F_CHIP_ID)    << " "
            <<          setw(9)  <<    iValue(i,F_CHANNEL_ID) << "     "
            <<          setw(5)  <<    iValue(i,F_ADC) << " "
            <<          setw(5)  <<    iValue(i,F_FULL_FPHX) << " "
            <<          setw(9)  <<    iValue(i,F_FULL_ROC)
            <<          setw(8)  <<    iValue(i,F_AMPLITUDE)
            << " "
            << "0x" << setw(8) << hex << setfill('0') << iValue(i,F_DATAWORD)
            << setfill(' ') << dec << endl;

    }

}
    

int InttPacket::decode(EventBcoHitBuf* evtBcoHitBuf)
{
    if(evtBcoHitBuf==NULL) {
        cout<<"evtBcoHitBuf close"<<endl;
        return -1;
    }

    for(int iladder=0; iladder<14; iladder++){
        if(!evtBcoHitBuf->status[iladder]){
            continue;
        }

        BcoHitBuf* bcoHitBuf = evtBcoHitBuf->bcoHitBuf[iladder]; 

        unsigned int size = bcoHitBuf->vHit.size();
        if(size<2) {
            cout<<" buf size is too small. skip to decode"<<endl;
            continue;
        }

        // footer check
        int footer = bcoHitBuf->vHit[size-1];
        if(footer != 0xcafeff80){
            cout<<" footer is not valid"<<endl;
        }

        int x = bcoHitBuf->vHit[0];
        eventNum[iladder] =  ((x>>16)&0xFFFF);
        eventNum[iladder] |= ((x&0xFFFF)<<16);

        bco[iladder] = bcoHitBuf->bco; // 40bits
        
        for(int ihit=1; ihit<size-1; ihit++){
            x = bcoHitBuf->vHit[ihit];

            intt_hit *hit = new intt_hit;

            hit->bco       = bcoHitBuf->bco; // 40bits
            hit->fee       = iladder;        // 0-13
            hit->evt       = eventNum[iladder];

            hit->channel_id  = ( x >> 16) & 0x7f; // 7bits
            hit->chip_id   = ( x >> 23) & 0x3f; // 6
            hit->adc       = ( x >> 29) & 0x7; // 3

            hit->FPHX_BCO  =   x       & 0x7f;
            hit->full_FPHX = ( x >> 7) & 0x1; // 1
            hit->full_ROC  = ( x >> 8) & 0x1; // 1
            hit->amplitude = ( x >> 9) & 0x3f; // 1
            hit->word      =  x;

            intt_hits.push_back(hit);
        }

    }

    return 0;
}

int InttPacket::iValue(const int hit, const int field)
{
    if ( hit < 0 || hit >= (int) intt_hits.size()) return 0;

    switch (field)
    {
        case F_FEE:
            return intt_hits[hit]->fee;
            break;

        case F_EVTCTR:
            return intt_hits[hit]->evt;
            break;

        case F_CHANNEL_ID:
            return intt_hits[hit]->channel_id;
            break;

        case F_CHIP_ID:
            return intt_hits[hit]->chip_id;
            break;

        case F_ADC:
            return intt_hits[hit]->adc;
            break;

        case F_FPHX_BCO:
            return intt_hits[hit]->FPHX_BCO;
            break;

        case F_FULL_FPHX:
            return intt_hits[hit]->full_FPHX;
            break;

        case F_FULL_ROC:
            return intt_hits[hit]->full_ROC;
            break;

        case F_AMPLITUDE:
            return intt_hits[hit]->amplitude;
            break;

        case F_DATAWORD:
            return intt_hits[hit]->word;
            break;

    }

    return 0;
}

uint64_t InttPacket::lValue(const int hit, const int field)
{
    if ( hit < 0 || hit >= (int) intt_hits.size()) return 0;

    switch (field)
    {
        case F_BCO:
            return intt_hits[hit]->bco;
            break;
    }

    return 0;
}

int InttPacket::iValue(const int hit, const char *what)
{
    if ( strcmp(what,"NR_HITS") == 0)
    {
        return intt_hits.size();
    }


    else if ( strcmp(what,"ADC") == 0)
    {
        return iValue(hit,F_ADC);
    }

    else if ( strcmp(what,"AMPLITUDE") == 0)
    {
        return iValue(hit,F_AMPLITUDE);
    }

    if ( strcmp(what,"CHIP_ID") == 0)
    {
        return iValue(hit,F_CHIP_ID);
    }

    if ( strcmp(what,"CHANNEL_ID") == 0)
    {
        return iValue(hit,F_CHANNEL_ID);
    }

    if ( strcmp(what,"FPHX_BCO") == 0)
    {
        return iValue(hit,F_FPHX_BCO);
    }

    if ( strcmp(what,"FULL_FPHX") == 0)
    {
        return iValue(hit,F_FULL_FPHX);
    }

    if ( strcmp(what,"FEE") == 0)
    {
        return iValue(hit,F_FEE);
    }

    if ( strcmp(what,"EVTCTR") == 0)
    {
        return iValue(hit,F_EVTCTR);
    }


    if ( strcmp(what,"FULL_ROC") == 0)
    {
        return iValue(hit,F_FULL_ROC);
    }

    if ( strcmp(what,"DATAWORD") == 0)
    {
        return iValue(hit,F_DATAWORD);
    }

    return 0;
}

uint64_t  InttPacket::lValue(const int hit, const char *what)
{
    if ( strcmp(what,"BCO") == 0)
    {
        return lValue(hit,F_BCO);
    }
    return 0;
}



/////////////
InttDecode::InttDecode(fileEventiterator* eventItr, int pID) :
         packetID(pID), evtItr(eventItr), 
         debugMode(false), endOfFile(false)
{ 
   int target_lad = 6; // for debug
};

InttPacket* InttDecode::getNextPacket()
{
  if(evtItr==NULL) return NULL;

  InttPacket* packet = NULL;

  bool isPrevEvent=false;

  static int evtIndex=0;
  while(1){// infinite loop
      if(debugMode)
          cout<<"evt : "<<evtIndex<<endl;
      evtIndex++;
     

      // check if first evtBuf is ready to decode
      bool readyToDecode = checkReadyToDecode(vEvtBcoHitBuf);
      if(endOfFile) {
          readyToDecode=true; // force to decode when end of file
      }

      if(readyToDecode){
          if(vEvtBcoHitBuf.size()==0) {
              if(debugMode)
                  cout<<"end of process : size="<<vEvtBcoHitBuf.size()<<endl;
              // return NULL
              break;
          }

          // remove firstEvtBuf from EventBcoHitBuf
          auto evtBufItr  = vEvtBcoHitBuf.begin();
          EventBcoHitBuf* firstEvtBuf = evtBufItr->second;

          vEvtBcoHitBuf.erase(vEvtBcoHitBuf.begin());

          // decode
          if(debugMode){
              cout<<"ready to decode : evtBuf : 0x"<<hex<<evtBufItr->first<<dec<<endl;
              firstEvtBuf->showStatus();
          }

          packet = new InttPacket(packetID, firstEvtBuf);
          //packet->dump();
         
          // debug 
          if(debugMode){
              for(auto evtBufItr=vEvtBcoHitBuf.begin(); evtBufItr!=vEvtBcoHitBuf.end(); evtBufItr++){
                  cout<<"evtBuf : 0x"<<hex<<evtBufItr->first<<dec<<endl;
                  evtBufItr->second->showStatus();
              }
          }

          delete firstEvtBuf;

          break;
      } 
      
      // debug 
      if(debugMode){
          if(isPrevEvent){
              for(auto evtBufItr=vEvtBcoHitBuf.begin(); evtBufItr!=vEvtBcoHitBuf.end(); evtBufItr++){
                  cout<<"evtBuf : 0x"<<hex<<evtBufItr->first<<dec<<endl;
                  evtBufItr->second->showStatus();
              }
          }
      }

      //////////////////////////////
      // check nextEvent;
      Event *e = evtItr->getNextEvent();
      if(e==NULL){
        endOfFile=true;
        if(debugMode)
            cout<<"end of FILE:"<<endl;
        continue;
      }

      oncsSubevent *p = (oncsSubevent*)e->getPacket(packetID);
      if(p){

        int  nwout;
        int  length = p->getLength();

        //p->dump(cout);

        int* array = new int[length];
        for(int i=0; i<length+1; i++) array[i] = 0;
        int state = p->fillIntArray(array, length, &nwout, "RAW");
        if(debugMode)
            cout<<"  Evt:"<<e->getEvtSequence()<<" "<<length<<" "<<nwout<<" "<<state<<endl;

        //bool hitheader_found=false;
        //int hitheader_cnt=0;

        int i=0;
        while(i<length)
        {
           if( (array[i]&0xFF00FFFF)==0xF000CAF0 ) // header
           {
              int len= (((array[i]>>16)&0xF)>>1);
              int lad=  ((array[i]>>20)&0xF);

              if(lad==target_lad){
                //--cout<<setw(5)<<i<<"  0x"<<hex<<setw(8)<<array[i]<<dec<<"  :  ";
                //--cout<<" packet header : len, lad="<<len<<" "<<lad<<endl;
              }

              i++;
              
              for(int ipkt=0; ipkt<len; ipkt++,i++){ 
                vHitBuf[lad].push_back(array[i]); 
                //--if(lad==target_lad) cout<<setw(5)<<i<<"  0x"<<hex<<setw(8)<<array[i]<<dec<<"  :  "<<lad<<endl;
              }

           }
           else { 
             //--cout<<setw(5)<<i<<"  0x"<<hex<<setw(8)<<array[i]<<dec<<"  :  ";
             //--cout<<endl;
             i++;
           }
        }

        ///////////////////
        // post packet process
        //  create BcoHitBuffer for ladder by ladder 
        for(int ilad=0; ilad<16; ilad++)
        {
          fillBcoHitBuf(vHitBuf[ilad], &(vBcoHitBuf[ilad]), debugMode&(ilad==target_lad));
        }

        // fill EventBcoHitBuffer using vBcoHitBuf (BcoHitBuf array)
        for(int ilad=0; ilad<14; ilad++)
        {
            int bufsize = vBcoHitBuf[ilad].size();
            for(int i=0; i<bufsize; i++){
                //BcoHitBuf* bcoHitBuf = vBcoHitBuf[ilad][i];
                //cout<<"ilad : "<<ilad<<"  "<<vBcoHitBuf[ilad].size()<<endl;

               // if(ilad==target_lad) 
               //     cout<<"size : "<<vBcoHitBuf[ilad].size()<<endl;

                BcoHitBuf* bcoHitBuf = vBcoHitBuf[ilad].front();
                if(bcoHitBuf->done){
                    // this bcoHitBuf should be removed from vBcoHitBuf
                    vBcoHitBuf[ilad].pop_front();

                    
                    map<uint64_t, EventBcoHitBuf*>::iterator itr = vEvtBcoHitBuf.find(bcoHitBuf->bco);
                    if(itr==vEvtBcoHitBuf.end()){ // first BCO
                        EventBcoHitBuf* evtBcoHitBuf = new EventBcoHitBuf(bcoHitBuf->bco);
                        evtBcoHitBuf->addBcoHitBuf(ilad, bcoHitBuf);

                        vEvtBcoHitBuf.insert(make_pair(bcoHitBuf->bco, evtBcoHitBuf));
                    }
                    else {
                        EventBcoHitBuf* evtBcoHitBuf = itr->second;
                        evtBcoHitBuf->addBcoHitBuf(ilad, bcoHitBuf);
                    }
                }
            }
            //cout<<"ilad : "<<ilad<<"  "<<bufsize<<" "<<vBcoHitBuf[ilad].size()<<endl;
        }


        
        ///////////////////
        delete [] array;
        delete p;

        isPrevEvent=true;
      }
      else{
          if(debugMode)
              cout<<"Skip : "<<e->getEvtType()<<endl;
          isPrevEvent=false;
      }

      delete e;
  }

  return packet;
}

