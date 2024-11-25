
#include "InttCluster.h"

#include <iostream>
#include <iomanip>

using namespace std;


void InttCluster::show(bool crflag) { 
  cout<<"  mod, chip, ch, adc, nhits= "<<module<<" "<<chip_id<<" "<<ch<<" "<<adc<<" "<<nhits;
  if(crflag)  cout <<endl;
}

void InttClusterList::Clustering(vector<InttHit*>& vHit)
{
  if(vHit.size()==0) {
    return ;
  }

  vector< vector<InttHit*> > vlist;

  vector<InttHit*> vgroup;
  InttHit* hit_prev = nullptr;
  for( auto itr=vHit.begin(); itr!=vHit.end(); ++itr)
  {
    InttHit* hit = *itr;
    //debug cout<<"hitprev : ";
    //debug if(hit_prev!=nullptr) cout<<hit_prev->module<<" "<<hit_prev->chip_id<<" "<<hit_prev->chan_id<<" ";
    //debug cout<<"hit : "<<hit->module<<" "<<hit->chip_id<<" "<<hit->chan_id<<endl;

    if( hit_prev==nullptr || 
       (hit->chan_id - hit_prev->chan_id)==1 // neighbouring hit
    ){
      //vgroup.push_back(hit);
    }
    else {
      vlist.push_back(vgroup);
      vgroup.clear();
    }
    
    vgroup.push_back(hit);

    hit_prev=hit;
  }
  if(vgroup.size()>0) vlist.push_back(vgroup);

  //static const int dac[8]={23, 30, 60, 90, 120, 150, 180, 210};
  static const int dac[8]={15, 30, 50, 70, 90, 110, 130, 150};

  //debug cout<<"Ngroup "<<vlist.size()<<endl;
  for(unsigned int igroup=0; igroup<vlist.size(); igroup++)
  {
     vector<InttHit*>& vgroup1 = vlist[igroup];

     if(vgroup1.size()>0){
       //debug cout<<"    ";
       //debug for(auto itr1=vgroup1.begin(); itr1!=vgroup1.end(); ++itr1){
       //debug    InttHit* hit = *itr1;
       //debug    cout<<hit->chan_id<<" ";
       //debug }
       //debug cout<<endl;
       
       float pos_sum=0.0;
       int   adc_sum=0;
       int   module=0, chip_id=0;

       int      pid, ampl, bco;
       Long64_t bco_full;
       
       //int      evt;
       int      roc, barrel, layer, ladder, arm;

       InttHit *savehit=nullptr;
       for(auto itr1=vgroup1.begin(); itr1!=vgroup1.end(); ++itr1){
          InttHit* hit = *itr1;

          pos_sum += (hit->chan_id*dac[hit->adc]);
          adc_sum += dac[hit->adc];

          module  = hit->module;
          chip_id = hit->chip_id;

          ///////////////
          savehit = hit;
       }
       if(adc_sum>0) pos_sum/=adc_sum;

       InttCluster* clus=new InttCluster(module, chip_id, pos_sum, adc_sum, vgroup1.size());

       clus->pid      = savehit->pid;
       clus->ampl     = savehit->ampl;
       clus->bco      = savehit->bco;
       clus->bco_full = savehit->bco_full;
       clus->evt      = savehit->evt;
       clus->roc      = savehit->roc;
       clus->barrel   = savehit->barrel;
       clus->layer    = savehit->layer;
       clus->ladder   = savehit->ladder;
       clus->arm      = savehit->arm;
       //clus->show();

       fvCluster.push_back(clus);
     }
  }
  fNhits=fvCluster.size();
}

/////////////////////////
void Show(InttHit* hit, bool crflag=true){
  if(hit==nullptr) return;

  cout<<"    "<<setw(6)<<hit->pid
      <<" "   <<setw(3)<<hit->module
      <<" "   <<setw(3)<<hit->chip_id
      <<" "   <<setw(3)<<hit->chan_id
      <<" "   <<setw(2)<<hit->adc;
  cout<<" "   <<setw(4)<<hit->bco
      <<" "   <<setw(12)<<hit->bco_full
      <<"("   <<setw(3)<<(hit->bco_full&0x7F)<<")";
  if(crflag) cout<<endl;
}

void Show(vector<InttHit*>& vHit){
  for( auto itr=vHit.begin(); itr!=vHit.end(); ++itr)
  {
    InttHit* hit = *itr;
    Show(hit);
  }
}

bool SkipBadHit(InttHit* hit)
{
    bool isBadHit = (
            (hit->bco==0&&hit->chip_id== 0&&hit->chan_id==  0&&hit->adc==0) 
         || (hit->bco==0&&hit->chip_id==21&&hit->chan_id==126&&hit->adc==6) 
            );
    return isBadHit;
}


void Clustering (InttEvent* inttEvent, InttClusterList* clusterList)
{

  vector<InttHit*> vHit[8][14][26];

  int N = inttEvent->getNHits();
  //if(N)
    std::cout << "Num hits: " << N << "  "<<std::endl;
  for(int ihit = 0; ihit < N; ++ihit)
  {
    InttHit* hit = inttEvent->getHit(ihit);
    
    if(hit->pid < 3001 || 3008 < hit->pid ) {cout<<"pid out of range : "<<hit->pid<<endl; continue;}
    if(hit->module  > 14) {cout<<"module out of range : "<<hit->module<<endl; continue;}
    if(hit->chip_id > 26) {cout<<"chipid out of range : "<<hit->chip_id<<endl; continue;}

    //Show(hit,false); 
    //if(SkipBadHit(hit)){ /*cout<<"   skip : "<<endl;*/ continue;}
    //cout<<endl;

    vHit[hit->pid-3001][hit->module][hit->chip_id].push_back(hit);
  }

  // clustering
  for(int ipid = 0; ipid < 8; ++ipid)
  {
      for(int imodule = 0; imodule < 14; ++imodule)
      {
          for(int ichip = 0; ichip < 26; ++ichip)
          {
              //if(vHit[ipid][imodule][ichip].size()>0){
              //    //cout<<"module/chip = "<<imodule<<" "<<ichip<<endl;
              //    Show(vHit[ipid][imodule][ichip]);
              //}
              //}
              //for(int ichip = 0; ichip < 26; ++ichip)
              //{
              //cout<<"imo "<<imodule<<" "<<ichip<<endl;
              clusterList->Clustering(vHit[ipid][imodule][ichip]);
          }
      }
  }
}

