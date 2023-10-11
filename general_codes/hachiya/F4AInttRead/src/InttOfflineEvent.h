#ifndef __INTTOFFLINEEVENT_H__
#define __INTTOFFLINEEVENT_H__

//#include "TTree.h"
//#include "Riostream.h"
#include "TClonesArray.h"
//#include "TObject.h"

class InttOfflineHit : public TObject {
  public:
    InttOfflineHit();
    virtual ~InttOfflineHit(){};

    void copy(InttOfflineHit& hit);
    void show(bool explanation_flag=false);

    virtual Bool_t IsEqual(const TObject *obj) const;
    virtual Bool_t IsSortable() const;
    virtual Int_t  Compare(const TObject* obj) const;

  public:
    int hitsetkey;
    int hitkey;
    int adc;
    
    ClassDef(InttOfflineHit, 1)
};

class InttOfflineEvent : public TObject {
  public:
    InttOfflineEvent();
    virtual ~InttOfflineEvent();

    InttOfflineHit* addHit();
    void            clear();
    void            copy(InttOfflineEvent* org);

    int             getNHits();
    InttOfflineHit* getHit(const int ihit);

    void     show();
    void     sort();

  //protected:
    int           fNhits;
    TClonesArray* fhitArray;
 
    ClassDef(InttOfflineEvent, 1)

};

#endif
