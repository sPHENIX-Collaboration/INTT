#include "InttEventSync.h"
#include "InttEvent.h"
#include <TTree.h>
#include <iostream>
#include <iomanip>

using namespace std;

void EventBuf::showStatus(){
    std::cout<<"   ";
    for(int i=0; i<8; i++){
        std::cout<<std::setw(3)<<status[i];
    }
    std::cout<<std::endl;
    std::cout<<"   ";
    for(int i=0; i<8; i++){
        int evtseq=-1;
        //if(inttEvtBuf[i])!=NULL) { evtseq = inttEvtBuf[i]->evtSeq; }
        if(status[i]) { evtseq = inttEvtBuf[i]->evtSeq; }
        std::cout<<std::setw(7)<<evtseq;
    }
    std::cout<<std::endl;
    std::cout<<"   ";
    for(int i=0; i<8; i++){
        int nhit=0;
        if(status[i]) { nhit = inttEvtBuf[i]->getNHits(); }
        std::cout<<std::setw(7)<<nhit;
    }
    std::cout<<std::endl;
};

//////////////////////////////

InttEventSync::InttEventSync() :
    initDone(false)
    //,inttEvt(NULL)
    //,iEvent(NULL)
{
}

void InttEventSync::initBranch()
{
    cout<<"initBranch"<<endl;
    for(int itree=0; itree<vTree.size(); itree++){
        inttEvt[itree]=NULL;
        vTree[itree]->SetBranchAddress("event", &(inttEvt[itree]));

        iEvent[itree]=0;
    }

    // check tree
    cout<<"Nentries: ";
    for(int itree=0; itree<vTree.size(); itree++){
        int N = vTree[itree]->GetEntries();
        cout<<N<<" ";
    }
    cout<<endl;

    initDone=true;
}

InttEvent* InttEventSync::getNextEvent()
{
    if(!initDone)
        initBranch();

    
    //read one event from files
    for(int itree=0; itree<vTree.size(); itree++){
        cout<<iEvent[itree]<<flush;

        if(iEvent[itree]>=vTree[itree]->GetEntries()){
            // all event in this file runout
            cout<<endl;
            continue;
        }

        vTree[itree]->GetEntry(iEvent[itree]);

        auto evtItr = vEvtBuf.find(inttEvt[itree]->bco);
        if(evtItr==vEvtBuf.end()){ // not found
            EventBuf *evtBuf = new EventBuf(inttEvt[itree]->bco, inttEvt[itree]->evtSeq);
            evtBuf->addInttEvent(itree, inttEvt[itree]);
            vEvtBuf.insert(make_pair(inttEvt[itree]->bco, evtBuf));
        }
        else {
            evtItr->second->addInttEvent(itree, inttEvt[itree]);
        }

        cout<<" evtSeq="<<setw(8)<<inttEvt[itree]->evtSeq
            <<" bco=0x"<<setfill('0')<<setw(10)<<right<<hex<<inttEvt[itree]->bco<<dec<<setfill(' ')<<endl;
    }
    cout<<endl;

    if(vEvtBuf.size()==0){
        bool fileRunOut=true;
        for(int itree=0; itree<vTree.size(); itree++){

            fileRunOut &= (iEvent[itree]>=vTree[itree]->GetEntries());
        }
        if(fileRunOut) { cout<<"end of file: "<<endl; return NULL;}
    }

    // debug
    //--for(auto evtItr=vEvtBuf.begin(); evtItr!=vEvtBuf.end(); ++evtItr){
    //--    evtItr->second->showStatus();
    //--}

    // get first event (smaller BCO)
    EventBuf* firstEvt = (vEvtBuf.size()>0) ? vEvtBuf.begin()->second : NULL ;
    if(firstEvt==NULL) { cout<<"first event=NULL"<<endl; return NULL; }

    InttEvent* mergedEvt=NULL;

    int ngood=0;
    for(int itree=0; itree<8; itree++){
        if(firstEvt->status[itree]){
            iEvent[itree]++;
            ngood++;

            if(mergedEvt==NULL) mergedEvt = new InttEvent();

            InttEvent * thisEvt = firstEvt->inttEvtBuf[itree];

            mergedEvt->evtSeq = thisEvt->evtSeq;
            mergedEvt->bco    = thisEvt->bco;

            //cout<<itree<<", bco : "<<hex<<thisEvt->bco<<" "<<mergedEvt->bco<<dec<<endl;
            for(int ilad=0; ilad<14; ilad++){
              mergedEvt->bcoArray[itree*14 + ilad]   = thisEvt->bcoArray[ilad];
              //cout<<"   "<<itree<<" "<<ilad<<" , bco : "<<hex<<thisEvt->bcoArray[ilad]<<" "<<mergedEvt->bcoArray[itree*14+ilad]<<dec<<endl;
            }

            for(int ihit=0; ihit<thisEvt->getNHits(); ihit++){
                InttHit* hitnew = mergedEvt->addHit();
                InttHit* hit    = thisEvt->getHit(ihit);
                hitnew->copy(*hit);
                //cout<<"debug hit: "<<endl;
                //hitnew->show();
                //hit->show();
            }
        }
    }
    //mergedEvt->show();
    //cout<<"ngood : "<<ngood<<endl;

    // clear eventbuf
    {
        for(auto evtItr = vEvtBuf.begin(); evtItr!=vEvtBuf.end(); ++evtItr){
            delete evtItr->second;
        }
        vEvtBuf.clear();
    }

    return mergedEvt;
}
