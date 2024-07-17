#include "InttStreamAna.h"

/// Cluster/Calorimeter includes

/// Fun4All includes
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <ffarawobjects/InttRawHitContainer.h>
#include <ffarawobjects/InttRawHit.h>

#include <ffarawobjects/Gl1RawHit.h>
#include <trackbase/InttEventInfo.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>

using namespace std;


/**
 * Constructor of module
 */
InttStreamAna::InttStreamAna(const std::string &name)
  : SubsysReco(name)
{
}

/**
 * Destructor of module
 */
InttStreamAna::~InttStreamAna()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttStreamAna::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttStreamAna" << std::endl;
  }

  return 0;
}

int InttStreamAna::InitRun(PHCompositeNode * topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttStreamAna" << std::endl;
  }

  if(!topNode)
  {
	  std::cout << "InttStreamAna::InitRun(PHCompositeNode* topNode)" << std::endl;
	  std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

	  return -1;
  }

/*

  TrkrHitSetContainer* trkr_hit_set_container = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
*/
  // define histograms
  for(int i=0; i<8; i++){
    h_bco[i] = new TH2F(Form("h_bco_%d", i),     Form("bco_%d", i),     26*14, 0, 26*14, 140, -7, 133);
    h_bcoreco_diff[i] = new TH1F(Form("h_bcoreco_diff_%d", i), Form("bcoreco diff_%d", i), 540, -270, 270);
    h_bcoreco_evt_diff[i] = new TH1F(Form("h_bcoreco_evt_diff_%d", i), Form("bcoreco evt diff_%d", i), 540, -270, 270);

    h_bunch[i] = new TH1F(Form("h_bunch_%d", i), Form("bunch @ trigger_%d", i), 150, -15, 135);
    h_bunch_strb[i] = new TH1F(Form("h_bunch_strb_%d", i), Form("bunch @ strobe_%d", i), 150, -15, 135);
    h_bunch_evt_bcodiff[i] = new TH2F(Form("h_bunch_evt_bcodiff_%d", i), Form("bunch @ strobe_%d", i), 
                                      500, -250, 250, 150, -15, 135);
    h_bunch_bco[i] = new TH2F(Form("h_bunch_bco_%d", i), Form("bunch vs BCO %d", i), 
                                      150, 0, 150, 150, -15, 135);

    h_bcoprediff[i] = new TH1F(Form("h_bcoprediff_%d", i), Form("BCO - PreBCO %d", i), 1000, 0, 1000);
  }
  h_bunch_all = new TH1F("h_bunch_all", "bunch @ evt all felix", 150, -15, 135);
  h_bunch_gl1 = new TH1F("h_bunch_gl1", "bunch @ gl1", 150, -15, 135);
  h_bco_all = new TH1F("h_bco_all", "bcoall", 128,  0, 128);


  cout<<"gl1file : "<<m_gl1_fname.c_str()<<endl;
  if(m_gl1_fname.length()>0){
    m_gl1file  = TFile::Open(m_gl1_fname.c_str());
    m_t_evtgl1 = (TTree*)m_gl1file->Get("t_evtgl1");

    m_t_evtgl1->SetBranchAddress("evt_gl1",   &m_evt_gl1);
    m_t_evtgl1->SetBranchAddress("bco_gl1",   &m_bco_gl1);
    m_t_evtgl1->SetBranchAddress("bunch_gl1", &m_bunch_gl1);

    m_gl1_ievent = 0;
  }




  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttStreamAna::process_event(PHCompositeNode* topNode)
{
  static int event=0;
  static int nskip=0;


  InttEventInfo* intthdr = findNode::getClass<InttEventInfo>(topNode, "INTTEVENTHEADER");
  InttRawHitContainer* rawhitcont 
                         = findNode::getClass<InttRawHitContainer>(topNode, "INTTRAWHIT");

  uint64_t bcointt = (intthdr!=nullptr) ? intthdr->get_bco_full() : std::numeric_limits<uint64_t>::max();

  uint64_t bcointt1= (rawhitcont!=nullptr&&rawhitcont->get_nhits()>0)
                           ? rawhitcont->get_hit(0)->get_bco()
                           : std::numeric_limits<uint64_t>::max();

  static uint64_t bcointt_pre{0};
  static uint64_t bcointt_pre_felix[8]{0};
  static uint64_t bcogl1_pre{0};


  if(   bcointt   == std::numeric_limits<uint64_t>::max() )
  {
    cout<<"StreamAna bcointt is max. not valid. Abort"<<endl;
    nskip++;
    //return Fun4AllReturnCodes::ABORTEVENT;
    //return Fun4AllReturnCodes::EVENT_OK;
  }
  if(   bcointt1 == std::numeric_limits<uint64_t>::max() )
  {
    cout<<"StreamAna bcointt1 is max. not valid. Abort"<<endl;
    nskip++;
    //return Fun4AllReturnCodes::ABORTEVENT;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  bcointt  &= 0xFFFFFFFFFF;
  bcointt1 &= 0xFFFFFFFFFF;

  //--static const int gl1bco_offset=  19;
  //static const int strobebco_offset= -19;
  static const int strobebco_offset= -23;
  //static const int strobebco_offset= 0;
  bcointt  += strobebco_offset;
  bcointt1 += strobebco_offset;

  uint64_t bcoprediff = bcointt1 - bcointt_pre;


  cout<<"StreamAna :"<<hex
                  <<" 0x"<<bcointt1
                  <<" 0x"<<bcointt <<dec
                  <<" diff: "<<bcoprediff<<endl;

  //h_bcoprediff[0]->Fill(bcoprediff);


  static bool isFirstFound=false;

  int64_t bcodiff_gl1intt = 0;
  if(m_t_evtgl1!=nullptr){
    int itrig_in_strobe = 0;
    while(itrig_in_strobe<120) 
    //while(itrig_in_strobe<1) 
    {
      int iloop=0;
      while(m_gl1_ievent<m_t_evtgl1->GetEntries())
      {
        m_t_evtgl1->GetEntry(m_gl1_ievent);
        //--m_bco_gl1 += gl1bco_offset;
        m_bco_gl1 &= 0xFFFFFFFFFF;

        bcodiff_gl1intt = m_bco_gl1 - bcointt1;
        if( fabsl(bcodiff_gl1intt)<1000 || (iloop%10000)==0 || iloop<100) {
          cout<<"   gl1 : "<<m_evt_gl1<<" 0x"<<hex<<m_bco_gl1<<" 0x"<<bcointt1<<dec<<endl;
        }

        if(bcointt1<m_bco_gl1+120) // double periods
        //if(bcointt1<m_bco_gl1) // single period
        {
          //cout<<"   gl1 found : "<<m_evt_gl1<<" 0x"<<hex<<m_bco_gl1<<" 0x"<<bcointt1<<dec<<endl;
          break;
        }


        m_gl1_ievent++;
        iloop++;

        if(isFirstFound){
          cout<<"   gl1 increment : "<<m_evt_gl1<<" 0x"<<hex<<m_bco_gl1<<dec<<" "<<m_bunch_gl1<<endl;
          h_bunch_gl1->Fill(m_bunch_gl1);
        }
      }

      uint64_t gl1bcoprediff = m_bco_gl1 - bcogl1_pre;

      bcointt_pre = bcointt1;
      bcogl1_pre  = m_bco_gl1;
      cout<<"-------- found ------------- "<<bcodiff_gl1intt<<" "<<gl1bcoprediff<<" "<<itrig_in_strobe
          <<" : "<<m_evt_gl1<<" 0x"<<hex<<m_bco_gl1<<" 0x"<<bcointt1<<dec<<endl;
      
      // gl1 trigger is within strobe BCO. 120 is strobe interval
      if(0 <= bcodiff_gl1intt && bcodiff_gl1intt < 120)
      //if(-120 <= bcodiff_gl1intt && bcodiff_gl1intt < 120)
      {
        cout<<"-------- within window -------------"<<endl;
        isFirstFound=true;

        ////////////////////////////

        set<uint> vUnique[8];  // bco after copy hit removal
        set<uint> vchipbco[8]; // bco for felix

        uint nhit = rawhitcont->get_nhits();
        for(uint ihit=0; ihit<nhit; ihit++){
          InttRawHit *hit = rawhitcont->get_hit(ihit);

          int ifelix = hit->get_packetid() - 3001;
          int bco    = hit->get_FPHX_BCO(); // 7bit
          //--uint64_t bcofull= hit->get_bco()&0xFFFFFFFFFF; // 40bit

          int ladder = hit->get_fee();            // 0-13 (4bit)
          int chip   = (hit->get_chip_id()-1)%26; // 0-26 (5bit)
          int chan   = hit->get_channel_id();     // 0-127 (7bit)
          int adc    = hit->get_adc();            // 3bit

          //InttNameSpace::RawData_s raw;
          //raw.felix_server = ifelix;
          //raw.felix_channel = hit->get_fee();
          //raw.chip = chip;
          //raw.channel = hit->get_channel_id();

          //if (m_HotChannelSet.find(raw) != m_HotChannelSet.end())
          //{
          ////  std::cout<<"hotchan removed : "<<raw.felix_server<<" "<<raw.felix_channel<<" "<<raw.chip<<" "<<raw.channel<<std::endl;
          //  continue;
          //}

          /////////////////////////////
          // remove copy hit
          // lad[25-22]+chip[21-17]+chan[16-10]+adc[9-7]+bco[6-0]
          uint key = ((ladder&0xF)<<22)|((chip&0x1F)<<17)|((chan&0x7F)<<10)|((adc&0x7)<<7)|(bco&0x7F) ;

          if(vUnique[ifelix].find(key)==vUnique[ifelix].end()) {
            vUnique[ifelix].insert(key);
            uint chipbcokey = ((ladder&0xF)<<22)|((chip&0x1F)<<17)|(bco&0x7F) ;
            vchipbco[ifelix].insert(chipbcokey);

            //h[ifelix]->Fill(bco);
            //float x = ladder + (int)((chip)/13) + 0.5;
            //float y = chan   + (int)((chip)%13)*128 + 0.5;
            //h2[ifelix]->Fill(x, y);
            //int bco7diff = (bco - bcointt7 + 128)%128;
            //int bco7diffc = (bco - (bcointt7 + bcooffset) + 128)%128;
            //--cout<<nhit<<"    hit : "<<ihit
            //--          <<" "<<ifelix
            //--          <<" "<<ladder
            //--          <<" "<<chip
            //--          <<" "<<chan
            //--          <<" "<<adc
            //--          <<" 0x"<<hex<<bco<<" "<<bco7diff<<dec<<endl;

            //h_bcodiff[ifelix]->Fill(ladder*26 + chip+0.5, bco7diffc+0.5);
            h_bco[ifelix]->Fill(ladder*26 + chip+0.5, bco+0.5);
            //h_hit[ifelix]->Fill(ladder*26 + chip+0.5, chan+0.5);
          }
          /////////////////////////////

          //cout<<"    hit : "<<ihit<<" "<<ifelix<<" 0x"<<hex<<bco<<dec<<endl;
        }


        h_bco_all->Reset();

        map<int, int> vbco_evt[8];
        for(int ifelix=0; ifelix<8; ifelix++){
         
          for(uint val : vchipbco[ifelix]){
            int bco = (val)&0x7F;
            //h_bco_felix[ifelix]->Fill(bco);

            uint64_t bcofull_reco  = bco + bcointt1;
            //uint64_t bcofull_reco8 = bcofull_reco&0xFF;
            //uint64_t gl1bco8       = m_bco_gl1&0xFF;
            int      diff = bcofull_reco - m_bco_gl1;
            h_bcoreco_diff[ifelix]->Fill(diff);

            h_bunch_bco[ifelix]->Fill(bco, m_bunch_gl1);

            //if(abs(diff)>130) {cout<<"exceed hit : "<<diff<<endl;}
            if(abs(diff)>250) {cout<<"exceed hit : "<<diff<<endl;}

            /////////////////////
            auto bco_itr = vbco_evt[ifelix].find(bco);
            if(bco_itr==vbco_evt[ifelix].end()) {
              vbco_evt[ifelix].insert(make_pair(bco,1));
            } else {
              bco_itr->second += 1;
            }
            
          }
          //h_bco_all->Add(h_bco_felix[ifelix]);
        }

        map<int, int> vbco_evt_all;
        for(int ifelix=0; ifelix<8; ifelix++){
          for(auto& val : vbco_evt[ifelix]){
            int bco = val.first;
            //int count = val.second;
            uint64_t bcofull_reco  = bco + bcointt1;
            //uint64_t bcofull_reco8 = bcofull_reco&0xFF;
            //uint64_t gl1bco8       = m_bco_gl1&0xFF;
            int      diff = bcofull_reco - m_bco_gl1;

            //if(abs(diff)>130) {cout<<"exceed : "<<diff<<endl;}
            if(abs(diff)>250) {cout<<"exceed : "<<diff<<endl;}

            h_bcoreco_evt_diff[ifelix]->Fill(diff);
            h_bunch_evt_bcodiff[ifelix]->Fill(diff, m_bunch_gl1);

            /////////////////////
            auto bcoall_itr = vbco_evt_all.find(bco);
            if(bcoall_itr==vbco_evt_all.end()) {
              vbco_evt_all.insert(make_pair(bco,1));
            } else {
              bcoall_itr->second += 1;
            }

            //if(diff==19)
            if(diff==0)
            {
              h_bunch[ifelix]->Fill(m_bunch_gl1);
              if(bcoall_itr==vbco_evt_all.end()) {
                h_bunch_all->Fill(m_bunch_gl1);
              }


              int bunch_strb = m_bunch_gl1 - bco;
              bunch_strb += 120;
              bunch_strb %= 120;

              h_bunch_strb[ifelix]->Fill(bunch_strb);
              //cout<<"             recobco 0x"<<hex<<bcofull_reco<<dec<<" "<<bco<<" "<<count<<endl;
            }
          }
        }
        
        ////////////////////////////
      }
      else {
        cout<<"-------- out of window ------------- "<<endl;
        break; // gl1bco is not in this strobe. break this loop
      }
      itrig_in_strobe++;

      m_gl1_ievent++; // increment gl1 event here to check if the second gl1 in this strobe
      if(isFirstFound){
        h_bunch_gl1->Fill(m_bunch_gl1);
      }
    }

  }

  {
    uint64_t bcointt_now[8]={0};
    uint nhit = rawhitcont->get_nhits();
    for(uint ihit=0; ihit<nhit; ihit++){
      InttRawHit *hit = rawhitcont->get_hit(ihit);

      int ifelix = hit->get_packetid() - 3001;
      uint64_t bcofull= hit->get_bco()&0xFFFFFFFFFF; // 40bit

      /////////////////////////////

      h_bcoprediff[ifelix]->Fill(bcofull - bcointt_pre_felix[ifelix]);

      bcointt_now[ifelix] = bcofull;
      //cout<<"    hit : "<<ihit<<" "<<ifelix<<" 0x"<<hex<<bco<<dec<<endl;
    }

    for(int ifelix=0; ifelix<8; ifelix++){
      bcointt_pre_felix[ifelix] = bcointt_now[ifelix];
    }
  }

  event++;
  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttStreamAna::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttStreamAna analysis package" << std::endl;
  }

  delete m_gl1file;

  TFile *fhistfile = new TFile("bcohist.root", "recreate");
  for(int ifelix=0; ifelix<8; ifelix++){
    h_bco[ifelix]->Write();
    h_bcoreco_diff[ifelix]->Write();
    h_bcoreco_evt_diff[ifelix]->Write();

    h_bunch[ifelix]->Write();
    h_bunch_strb[ifelix]->Write();
    h_bunch_evt_bcodiff[ifelix]->Write();
    h_bunch_bco[ifelix]->Write();
    h_bcoprediff[ifelix]->Write();
  }
  h_bunch_gl1->Write();
  h_bunch_all->Write();
  fhistfile->Close();


  return 0;
}


