#include "InttOfflineEvent.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>

ClassImp(InttOfflineHit)
ClassImp(InttOfflineEvent)


using namespace std;

static const int NHITS_INIT2 = 500;

/////////////////////
InttOfflineHit::InttOfflineHit() {
  //cout<<"ctor InttHit"<<endl;
}

void InttOfflineHit::copy(InttOfflineHit& hit){
  hitsetkey = hit.hitsetkey;
  hitkey    = hit.hitkey;
  adc       = hit.adc;
}

void InttOfflineHit::show(bool /*explanation_flag*/){
/*
  if(explanation_flag){
    cout<<"   module chip_id chan_id adc ampl";
    cout<<endl;
  }
  cout<<"   ";
  cout<<setw(6)<<module<<" ";
  cout<<setw(6)<<chip_id<<" ";
  cout<<setw(6)<<chan_id<<" ";
  cout<<setw(3)<<adc<<" ";
  cout<<setw(4)<<ampl<<" ";
  cout<<endl;
*/
}


Bool_t InttOfflineHit::IsEqual(const TObject *obj) const
{
    const InttOfflineHit* objcp = dynamic_cast<const InttOfflineHit*>(obj);
    bool ishitsetkey = (hitsetkey == (objcp->hitsetkey));
    bool ishitkey    = (hitkey    == (objcp->hitkey));
    bool isadc       = (adc       == (objcp->adc));

    return ishitsetkey&&ishitkey&&isadc;
}

Bool_t InttOfflineHit::IsSortable() const { return kTRUE;}

Int_t  InttOfflineHit::Compare(const TObject* obj) const {
    const InttOfflineHit* objcp = dynamic_cast<const InttOfflineHit*>(obj);
    if(     hitsetkey < objcp->hitsetkey) return -1;
    else if(hitsetkey > objcp->hitsetkey) return  1;
    else {
      if(     hitkey < objcp->hitkey) return -1;
      else if(hitkey > objcp->hitkey) return  1;
      else {
        return 0;
      }
    }
}

/////////////////////

InttOfflineEvent::InttOfflineEvent(): fNhits(0), fhitArray(NULL) {
  fhitArray = new TClonesArray("InttOfflineHit", NHITS_INIT2);
  cout<<"ctor InttOfflineEvent"<<endl;
}

InttOfflineEvent::~InttOfflineEvent(){
  if(fhitArray!=NULL) delete fhitArray;
  cout<<"dtor InttOfflineEvent"<<endl;
}

InttOfflineHit* InttOfflineEvent::addHit(){
  //cout<<"nhits: "<<fNhits<<endl;
  TClonesArray& hitArray = *fhitArray;
  InttOfflineHit* hitnew = new(hitArray[fNhits++]) InttOfflineHit();

  //cout<<"nhits: before return "<<fNhits<<endl;
  return hitnew;
}

int InttOfflineEvent::getNHits(){
  return fNhits;//m_hitArray->GetEntries();
}

InttOfflineHit* InttOfflineEvent::getHit(const int ihit){
  return (ihit<getNHits()) ? (InttOfflineHit*)fhitArray->UncheckedAt(ihit) : NULL;
}

void InttOfflineEvent::clear() {
  fhitArray->Clear(); 
  fNhits=0;
};

void InttOfflineEvent::copy(InttOfflineEvent* org) {
  if(org==NULL) return;

  clear();

  for(int ihit=0; ihit<org->getNHits(); ihit++){
      InttOfflineHit* hitnew = addHit();
      InttOfflineHit* hit    = org->getHit(ihit);
      hitnew->copy(*hit);
      //cout<<"debug hit: "<<endl;
      //hitnew->show();
      //hit->show();
  }
};

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
