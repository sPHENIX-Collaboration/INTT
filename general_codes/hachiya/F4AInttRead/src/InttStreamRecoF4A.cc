#include "InttStreamRecoF4A.h"

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
#include <ffarawobjects/Gl1Packet.h>
#include <trackbase/InttEventInfo.h>

#include <intt/InttBadChannelMap.h>
#include <intt/InttBCOMap.h>
#include <intt/InttMap.h>
#include <intt/InttFeeMap.h>



#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TLine.h>
#include <TFile.h>

#include <filesystem>
#include <map>

using namespace std;


/**
 * Constructor of module
 */
InttStreamRecoF4A::InttStreamRecoF4A(const std::string &name)
  : SubsysReco(name)
{
//  m_badmap = new InttBadChannelMapv1();
  m_bcomap = new InttBCOMap();
//  m_feemap = new InttFeeMapv1();
}

/**
 * Destructor of module
 */
InttStreamRecoF4A::~InttStreamRecoF4A()
{
//  delete m_badmap;
//  delete m_bcomap;
}

/**
 * Initialize the module and prepare looping over events
 */
int InttStreamRecoF4A::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttStreamRecoF4A" << std::endl;
  }

  return 0;
}

int InttStreamRecoF4A::InitRun(PHCompositeNode * topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttStreamRecoF4A" << std::endl;
  }

  if(!topNode)
  {
	  std::cout << "InttStreamRecoF4A::InitRun(PHCompositeNode* topNode)" << std::endl;
	  std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

	  return -1;
  }

  // init calib
  //m_feemap->LoadFromCDB();

  for(int i=0; i<8; i++){
    h_bcodiff[i] = new TH2D(Form("h_bcodiff_%d", i), Form("bcodiff_%d", i), 26*14, 0, 26*14, 140, -7, 133);
    h_bco[i]     = new TH2D(Form("h_bco_%d", i),     Form("bco_%d", i),     26*14, 0, 26*14, 140, -7, 133);
    h_hit[i]     = new TH2D(Form("h_hit_%d", i),     Form("hit_%d", i),     26*14, 0, 26*14, 128,  0, 128);

   
    // RecoBCO - GL1BCO
    h_bcoreco_diff[i]     = new TH1D(Form("h_bcoreco_diff_%d", i), Form("bcoreco diff_%d", i), 540, -270, 270);
    h_bcoreco_evt_diff[i] = new TH1D(Form("h_bcoreco_evt_diff_%d", i), Form("bcoreco evt diff_%d", i), 540, -270, 270);

    // RecoBCO - StrobeBCO, same as FPHX BCO
    h_bcorecointt_diff[i] = new TH1D(Form("h_bcorecointt_diff_%d", i), Form("bcoreco intt diff_%d", i), 540, -270, 270);

    // ChipBCO
    h_bco_felix[i]= new TH1D(Form("h_bco_felix_%d", i), Form("bcofelix_%d", i), 128,  0, 128);

    // StrobeBCO - GL1BCO
    h_bcogl1diff_felix[i]= new TH1D(Form("h_bcogl1diff_felix_%d", i), Form("bcogl1diff_felix_%d", i), 1024, -512, 512);


    h_bunch[i] = new TH1D(Form("h_bunch_%d", i), Form("bunch @ trigger_%d", i), 150, -15, 135);

    h_bunch_strb[i] = new TH1D(Form("h_bunch_strb_%d", i), Form("bunch @ strobe_%d", i), 150, -15, 135);

    // RecoBCO - Gl1BCO, vs  bunch, to see the peak
    h_bunch_evt_bcodiff[i] = new TH2F(Form("h_bunch_evt_bcodiff_%d", i), Form("bunch @ strobe_%d", i),
                                      750, -250, 500, 150, -15, 135);

    //ChipBCO vs Bunch to check the linear correlation
    h_bunch_bco[i] = new TH2D(Form("h_bunch_bco_%d", i), Form("bunch vs BCO %d", i),
                                      150, 0, 150, 150, -15, 135);

    // StrobeBCO - Prev StrobeBCO
    h_bcoprediff[i] = new TH1D(Form("h_bcoprediff_%d", i), Form("BCO - PreBCO %d", i), 1000, 0, 1000);
    //
    // StrobeBCO variation in an event
    h_bco_variation[i] = new TH1D(Form("h_bco_variation%d", i), Form("BCO BCOref %d", i), 1000, -500, 500);
  }

  h_bco_all= new TH1D("h_bco_all", "bcoall", 128,  0, 128);
  h_bunch_all = new TH1D("h_bunch_all", "bunch @ evt all felix", 150, -15, 135);
  h_bunch_gl1 = new TH1D("h_bunch_gl1", "bunch @ gl1", 150, -15, 135);
  h_bunch_all_trig = new TH1D("h_bunch_all_trig", "bunch @ evt all felix trig", 150, -15, 135);
  h_bunch_gl1_trig = new TH1D("h_bunch_gl1_trig", "bunch @ gl1 trig", 150, -15, 135);

  h_bcoreco_evt_diff_all = new TH1D("h_bcoreco_evt_diff_all", "bcoreco evt diff_all", 540, -270, 270);
  h_bcointtgl1_diff = new TH1D("h_bcointtgl1_diff", "bco intt gl1 diff_", 540, -270, 270);
  h_bcogl1pre_diff = new TH1D("h_bcogl1pre_diff", "bco gl1 pre diff_", 1000, 0, 1000);

  return Fun4AllReturnCodes::EVENT_OK;



}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttStreamRecoF4A::process_event(PHCompositeNode* topNode)
{
  static int event=0;
  static int nskip=0;

  //Gl1RawHit* gl1  = findNode::getClass<Gl1RawHit>(topNode, "GL1RAWHIT");
  //Gl1Packet* gl1p = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  Gl1Packet* gl1  = findNode::getClass<Gl1Packet>(topNode, "GL1RAWHIT");

  uint64_t bco_gl1  = (gl1 !=nullptr) ? gl1->getBCO() : std::numeric_limits<uint64_t>::max();
  //int      evt_gl1  = (gl1 !=nullptr) ? gl1->getEvtSequence() : -1;
  int      bunch_gl1= (gl1!=nullptr) ? gl1->getBunchNumber() : -1;

  //uint64_t trig =  (gl1!=nullptr) ? gl1->getLiveVector() : 0;
  uint64_t trig =  (gl1!=nullptr) ? gl1->getScaledVector() : 0;

  // trigbit 12 MBD N&S >= 1, vtx < 10 cm 
  bool isZok = ( (trig&0x0000000000001000) == 0x0000000000001000 );


  InttEventInfo *intthdr = findNode::getClass<InttEventInfo>(topNode, "INTTEVENTHEADER");

  InttRawHitContainer* rawhitmap = findNode::getClass<InttRawHitContainer>(topNode, "INTTRAWHIT");

  uint64_t bcointt1= (intthdr!=nullptr) ? intthdr->get_bco_full() : std::numeric_limits<uint64_t>::max();

  uint64_t bcointt  = (rawhitmap!=nullptr&&rawhitmap->get_nhits()>0)
                           ? rawhitmap->get_hit(0)->get_bco()
                           : std::numeric_limits<uint64_t>::max();


  static uint64_t prebcointt  = 0;
  static int      preevtcnt  = 0;
  //uint64_t prebcointt1 = 0;

  if( bco_gl1 == std::numeric_limits<uint64_t>::max() ){
    cout<<"StreamRecoF4A bco is max. not valid"<<endl;
  }
  if( bcointt == std::numeric_limits<uint64_t>::max() ){
    cout<<"StreamRecoF4A bco is max. not valid"<<endl;
  }
  if(bcointt1 == std::numeric_limits<uint64_t>::max() ) 
  {
    cout<<"StreamRecoF4A inttbco1 is max. no intt1 data valid. skip nth: "<<nskip<<endl;
    nskip++;
    //return Fun4AllReturnCodes::ABORTEVENT;
  }
  //
  // to 40bit

  bco_gl1  &= 0xFFFFFFFFFF;
  bcointt  &= 0xFFFFFFFFFF;
  bcointt1 &= 0xFFFFFFFFFF;

  uint64_t bcodiff = bcointt - prebcointt;

  int bcointt7 = bcointt&0x7F;

  int bcooffset = m_bcopeak[0][0];//48; //0x43; //0x25;

  int evtcnt= (rawhitmap!=nullptr&&rawhitmap->get_nhits()>0)
                           ? rawhitmap->get_hit(0)->get_event_counter()
                           : std::numeric_limits<uint64_t>::max();
  static bool initEvtCnt=false;
  static uint64_t bcofull_evt0=0;
  if((!initEvtCnt)&&(event==1||evtcnt==0||(preevtcnt>evtcnt))){
    bcofull_evt0 = bcointt;
    initEvtCnt=true;
  }

  uint64_t bcofull_corr=bcointt - bcofull_evt0;

  int64_t diff_inttgl1 = bcointt - bco_gl1;
  h_bcointtgl1_diff->Fill(bcointt - bco_gl1);

  //if(bcogl1 < (bcointt-1)){ // inttbco is always 1 higher than gl1bco
    cout<<event<<"   bco : intt:0x"<<hex<<bcointt<<", intt1:0x"<<bcointt1<<" diff 0x"<<bcodiff<<" : "<<bcofull_corr<<dec<<" "<<evtcnt<<" "<<diff_inttgl1<<" gl1:0x"<<hex<<bco_gl1<<dec<<endl;
    nskip++;
  //  return Fun4AllReturnCodes::ABORTEVENT;
  //}
  event++;

  if(rawhitmap==nullptr) {
    cout<<"pointer is null"<<endl;
    return false;
  }

  static TH1F * h[8]{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
  for(int i=0; i<8; i++){
    if(h[i] == nullptr) h[i] = new TH1F(Form("h_%d", i), Form("h_%d", i), 128,0,128);
    else                h[i]->Reset();
  }

  static TH2F * h2[8]{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
  for(int i=0; i<8; i++){ // 14*13*2
    if(h2[i] == nullptr) {
      h2[i] = new TH2F(Form("h2_%d", i), Form("h2_%d", i), 14*2, 0, 14, 128*13,0,128*13);
    }
    else                 h2[i]->Reset();
  }

  for(int i=0; i<8; i++){ // 14*13*2
    h_bco_felix[i]->Reset();
  }

  h_bunch_gl1->Fill(bunch_gl1);
  if(isZok) h_bunch_gl1_trig->Fill(bunch_gl1);
  
  uint nhit = rawhitmap->get_nhits();

  set<uint> vUnique[8];
  map<uint, int> vchipbco[8]; // key, ihit


  uint64_t bcointt_now[8]={0};
  static uint64_t bcointt_pre_felix[8]={0};
  static uint64_t bco_gl1_pre = 0;


  h_bcogl1pre_diff->Fill(bco_gl1 - bco_gl1_pre);



  // loop nhits with checking copy hit
  for(uint ihit=0; ihit<nhit; ihit++){
    InttRawHit *hit = rawhitmap->get_hit(ihit);

    int ifelix = hit->get_packetid() - 3001;
    int bco    = hit->get_FPHX_BCO(); // 7bit
    uint64_t bcofull= (hit->get_bco()&0xFFFFFFFFFF); // 40bit

    int ladder = hit->get_fee();        // 0-13 (4bit)
    int chip   = (hit->get_chip_id()-1)%26;    // 0-26 (5bit)
    int chan   = hit->get_channel_id(); // 0-127 (7bit)
    int adc    = hit->get_adc();        // 3bit

    InttNameSpace::RawData_s raw;
    raw.felix_server = ifelix;
    raw.felix_channel = hit->get_fee();
    raw.chip = chip;
    raw.channel = hit->get_channel_id();

    if (m_HotChannelSet.find(raw) != m_HotChannelSet.end())
    {
    //  std::cout<<"hotchan removed : "<<raw.felix_server<<" "<<raw.felix_channel<<" "<<raw.chip<<" "<<raw.channel<<std::endl;
      continue;
    }

    //InttMap::Offline_s offl = m_feemap->ToOffline(raw);

    //jif(m_badmap->IsBad(offl)){
    //j  continue;
    //j}
    //if(m_bcomap->isBad(raw)){
    //  continue;
    //}

    // lad[25-22]+chip[21-17]+chan[16-10]+adc[9-7]+bco[6-0]
    uint key = ((ladder&0xF)<<22)|((chip&0x1F)<<17)|((chan&0x7F)<<10)|((adc&0x7)<<7)|(bco&0x7F) ;

    int64_t bcogl1diff = bcofull - bco_gl1;
    //cout<<"bco-gl1diff " <<bcogl1diff<<endl;
    h_bcogl1diff_felix[ifelix]->Fill(bcogl1diff);
    
    // bco - preBCO for felix
    if(bcointt_pre_felix[ifelix]>0){
      int64_t bcoprediff = bcofull - bcointt_pre_felix[ifelix];
      h_bcoprediff[ifelix]->Fill(bcoprediff);

      //--if(bcoprediff!=120) cout<<"diff : "<<hex<<bcoprediff<<" "<<bcofull<<" "<<bcointt_pre_felix[ifelix]<<dec<<endl;
    }
    bcointt_now[ifelix] = bcofull;

    // bco variation
    h_bco_variation[ifelix]->Fill(bcofull - bcointt);

    if(vUnique[ifelix].find(key)==vUnique[ifelix].end()) {
      vUnique[ifelix].insert(key);
      uint chipbcokey = ((ladder&0xF)<<22)|((chip&0x1F)<<17)|(bco&0x7F) ;
      vchipbco[ifelix].insert(std::make_pair(chipbcokey, ihit));

      h[ifelix]->Fill(bco);
      float x = ladder + (int)((chip)/13) + 0.5;
      float y = chan   + (int)((chip)%13)*128 + 0.5;
      h2[ifelix]->Fill(x, y);
      //int bco7diff = (bco - bcointt7 + 128)%128;
      int bco7diffc = (bco - (bcointt7 + bcooffset) + 128)%128;
      //--cout<<nhit<<"    hit : "<<ihit
      //--          <<" "<<ifelix
      //--          <<" "<<ladder
      //--          <<" "<<chip
      //--          <<" "<<chan
      //--          <<" "<<adc
      //--          <<" 0x"<<hex<<bco<<" "<<bco7diff<<dec<<endl;

      h_bcodiff[ifelix]->Fill(ladder*26 + chip+0.5, bco7diffc+0.5);
      h_bco[ifelix]->Fill(ladder*26 + chip+0.5, bco+0.5);
      h_hit[ifelix]->Fill(ladder*26 + chip+0.5, chan+0.5);
    }

    //cout<<"    hit : "<<ihit<<" "<<ifelix<<" 0x"<<hex<<bco<<dec<<endl;
  }

  map<int, int> vbcodiff_felix[8];
  h_bco_all->Reset();
  for(int ifelix=0; ifelix<8; ifelix++){
   
    for(auto val : vchipbco[ifelix]){
      int bco = (val.first)&0x7F;
      h_bco_felix[ifelix]->Fill(bco);

      InttRawHit *hit = rawhitmap->get_hit(val.second);

      uint64_t bcofull= (hit->get_bco()&0xFFFFFFFFFF); // 7bit

      // trigger mode
      //uint64_t bcofull_reco = bco | (bcointt&0xFFFFFFFF80);

      // stream mode
      //uint64_t bcofull_reco = bco + bcointt;
      int bcofull_reco = bco + bcofull;

      int  bcointtgl1_diff = bcofull_reco - bco_gl1;
      int  bcointthit_diff = bcofull_reco - bcointt;
      //bcointtgl1_diff += 128;
      //bcointtgl1_diff %= 128;

      //--bcointthit_diff += 128;
      //--bcointthit_diff %= 128;
      //
      h_bcorecointt_diff[ifelix]->Fill(bcointthit_diff);
      h_bcoreco_diff[ifelix]->Fill(bcointtgl1_diff);

      h_bunch_bco[ifelix]->Fill(bco, bunch_gl1);
      h_bunch_evt_bcodiff[ifelix]->Fill(bcointtgl1_diff, bunch_gl1);

      auto bco_itr = vbcodiff_felix[ifelix].find(bcointtgl1_diff);
      if(bco_itr==vbcodiff_felix[ifelix].end()) {
        vbcodiff_felix[ifelix].insert(make_pair(bcointtgl1_diff,1));
        h_bcoreco_evt_diff[ifelix]->Fill(bcointtgl1_diff);
      } else {
        bco_itr->second += 1;
      }
    }
    h_bco_all->Add(h_bco_felix[ifelix]);
  }

  map<int, int> vbcodiff_all;

  for(int ifelix=0; ifelix<8; ifelix++){
    for(auto& val : vbcodiff_felix[ifelix]){
      int bcointtgl1_diff = val.first;
      //int count           = val.second;

      //cout<<"             recobco diff : "<<bcointtgl1_diff<<" "<<count<<" "<<ifelix<<endl;

      auto bco_all_itr = vbcodiff_all.find(bcointtgl1_diff);
      if(bco_all_itr==vbcodiff_all.end()) {
        vbcodiff_all.insert(make_pair(bcointtgl1_diff,1));
        h_bcoreco_evt_diff_all->Fill(bcointtgl1_diff);
        //
        //if(bcointtgl1_diff==20)
        //if(bcointtgl1_diff==11)
        //if(bcointtgl1_diff==12)
        if(bcointtgl1_diff==23)
        {
          h_bunch_all->Fill(bunch_gl1);
          if(isZok) h_bunch_all_trig->Fill(bunch_gl1);
        }

      } else {
        bco_all_itr->second += 1;
      }

    }
  }


  prebcointt  = bcointt;
  preevtcnt   = evtcnt;
  //prebcointt1 = bcointt1;

  for(int ifelix=0; ifelix<8; ifelix++){
    bcointt_pre_felix[ifelix] = bcointt_now[ifelix];
  }
  bco_gl1_pre = bco_gl1;


  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttStreamRecoF4A::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttStreamRecoF4A analysis package" << std::endl;
  }

  TFile *froot = TFile::Open("streamrecof4a.root","recreate");
  for(int i=0; i<8; i++){
    h_bcodiff[i]->Write();
    h_bco[i]->Write();
    h_hit[i]->Write();
    h_bco_felix[i]->Write();
    h_bunch[i]->Write();
    h_bcoreco_diff[i]->Write();
    h_bcoreco_evt_diff[i]->Write();
    h_bcorecointt_diff[i]->Write();
    h_bcogl1diff_felix[i]->Write();

    h_bunch_strb[i]->Write();
    h_bunch_evt_bcodiff[i]->Write();
    h_bunch_bco[i]->Write();
    h_bcoprediff[i]->Write();

    h_bco_variation[i]->Write();
  }
  h_bcoreco_evt_diff_all->Write();
  h_bco_all->Write();
  h_bunch_all->Write();
  h_bunch_gl1->Write();
  h_bunch_all_trig->Write();
  h_bunch_gl1_trig->Write();
  h_bcointtgl1_diff->Write();
  h_bcogl1pre_diff->Write();

  froot->Close();

  return 0;
}

//void InttStreamRecoF4A::SetBadChannelMap(const std::string& file){
//  //m_badmap->LoadFromFile(file);
//}

void InttStreamRecoF4A::SetBcoMap(const std::string& file){
  m_bcomap->LoadFromFile(file);

  for(int ifelix=0; ifelix<8; ifelix++){
    for(int iladder=0; iladder<14; iladder++){
      for(int ibco=0; ibco<128; ibco++){
        if(!m_bcomap->IsBad(ifelix, iladder, 0, ibco)){
          m_bcopeak[ifelix][iladder] = ibco + 1;
          cout<<"bcopeak "<<ifelix<<" "<<iladder<<" "<<m_bcopeak[ifelix][iladder]<<endl;
          break;
        }
      }
    }
  }
}

int InttStreamRecoF4A::LoadHotChannelMapLocal(std::string const& filename)
{
  if (filename.empty())
  {
    std::cout << "int InttCombinedRawDataDecoder::LoadHotChannelMapLocal(std::string const& filename)" << std::endl;
    std::cout << "\tArgument 'filename' is empty string" << std::endl;
    return 1;
  }

  if (!std::filesystem::exists(filename))
  {
    std::cout << "int InttCombinedRawDataDecoder::LoadHotChannelMapLocal(std::string const& filename)" << std::endl;
    std::cout << "\tFile '" << filename << "' does not exist" << std::endl;
    return 1;
  }


  CDBTTree cdbttree(filename);
  // need to checkt for error exception
  cdbttree.LoadCalibrations();

  m_HotChannelSet.clear();
  uint64_t N = cdbttree.GetSingleIntValue("size");
  cout<<"LoadHotChannelMapLocal : "<<N<<endl;
  for (uint64_t n = 0; n < N; ++n)
  {
    m_HotChannelSet.insert((struct InttNameSpace::RawData_s){
        .felix_server = cdbttree.GetIntValue(n, "felix_server"),
        .felix_channel = cdbttree.GetIntValue(n, "felix_channel"),
        .chip = cdbttree.GetIntValue(n, "chip"),
        .channel = cdbttree.GetIntValue(n, "channel")});

     //if(Verbosity() < 1)
     //{
     //   continue;
     //}
     std::cout << "Masking channel:\t" << std::endl;
     std::cout << "\t" << cdbttree.GetIntValue(n, "felix_server")
               << "\t" << cdbttree.GetIntValue(n, "felix_channel")
               << "\t" << cdbttree.GetIntValue(n, "chip")
               << "\t" << cdbttree.GetIntValue(n, "channel") << std::endl;
  }

  return 0;
}
