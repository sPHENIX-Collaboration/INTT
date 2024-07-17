#include "InttCompareRawHit.h"

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

//#include <trackbase/InttDefs.h>
//#include <trackbase/TrkrHitv2.h>
//#include <trackbase/TrkrHitSet.h>
//#include <trackbase/TrkrHitSetContainerv1.h>

/// ROOT includes
//#include <TFile.h>
//#include <TH1.h>
//#include <TH2.h>
//#include <TNtuple.h>
//#include <TTree.h>

/// C++ includes
//#include <cassert>
//#include <cmath>
//#include <fstream>
//#include <sstream>
//#include <string>

//#include <intt/InttMapping.h>

//#include "InttEvent.h"

using namespace std;


/**
 * Constructor of module
 */
InttCompareRawHit::InttCompareRawHit(const std::string &name)
  : SubsysReco(name)
{
}

/**
 * Destructor of module
 */
InttCompareRawHit::~InttCompareRawHit()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttCompareRawHit::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttCompareRawHit" << std::endl;
  }

  return 0;
}

int InttCompareRawHit::InitRun(PHCompositeNode * topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttCompareRawHit" << std::endl;
  }

  if(!topNode)
  {
	  std::cout << "InttCompareRawHit::InitRun(PHCompositeNode* topNode)" << std::endl;
	  std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

	  return -1;
  }

/*

  TrkrHitSetContainer* trkr_hit_set_container = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
*/

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttCompareRawHit::process_event(PHCompositeNode* /*topNode*/)
{
  static int event=0;
  static int nskip=0;

  Fun4AllServer *se = Fun4AllServer::instance();
  PHCompositeNode *topnode1 = se->topNode("TOP");  // default INTT DST
  PHCompositeNode *topnode2 = se->topNode("TOP2"); // special INTT DST

  //cout<<"top"<<endl;
  //topNode->print();
  //cout<<"top1"<<endl;
  //topnode1->print();
  //cout<<"top2"<<endl;
  //topnode2->print();

  Gl1RawHit     *gl1     = findNode::getClass<Gl1RawHit>(    topnode1, "GL1RAWHIT");
  InttEventInfo *intthdr = findNode::getClass<InttEventInfo>(topnode2, "INTTEVENTHEADER");

  InttRawHitContainer* rawhitcont1 = findNode::getClass<InttRawHitContainer>(topnode1, "INTTRAWHIT");
  InttRawHitContainer* rawhitcont2 = findNode::getClass<InttRawHitContainer>(topnode2, "INTTRAWHIT");

  uint64_t bcogl1  = (gl1    !=nullptr) ? gl1->get_bco()          : std::numeric_limits<uint64_t>::max();
  uint64_t bcointt = (intthdr!=nullptr) ? intthdr->get_bco_full() : std::numeric_limits<uint64_t>::max();


  if(bcogl1  == std::numeric_limits<uint64_t>::max() ||
     bcointt == std::numeric_limits<uint64_t>::max() ){
    cout<<"bco is max. not valid"<<endl;
  }
  // to 40bit
  bcogl1  &= 0xFFFFFFFFFF;
  bcointt &= 0xFFFFFFFFFF;

  uint64_t bcointt1 = (rawhitcont1!=nullptr&&rawhitcont1->get_nhits()>0) 
                           ? rawhitcont1->get_hit(0)->get_bco() 
                           : std::numeric_limits<uint64_t>::max();
  uint64_t bcointt2 = (rawhitcont2!=nullptr&&rawhitcont2->get_nhits()>0) 
                           ? rawhitcont2->get_hit(0)->get_bco() 
                           : std::numeric_limits<uint64_t>::max();

  if(bcogl1 != (bcointt-1)){ // inttbco is always 1 higher than gl1bco
    cout<<"   bco is different. skip inttdst nth="<<nskip<<" : gl1:0x"<<hex<<bcogl1<<", intt:0x"<<bcointt
        <<", intt1:0x"<<bcointt1<<", intt2:0x"<<bcointt2<<dec<<endl;
    nskip++;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


  bool result = Compare(rawhitcont1, rawhitcont2);
  string s_result = (result) ? "rawhit is OK" : "rawhit is different";
  //  cout<<""<<endl;
  //}
  //if(!result){
  //  cout<<"rawhit is different"<<endl;
  //}

  cout<<"InttCompareRawHit::process_event : "<<event;
  cout<<" gl1:0x"<<hex<<bcogl1<<", intt:0x"<<bcointt<<dec<<" "<<s_result<<endl;

  event++;
  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttCompareRawHit::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttCompareRawHit analysis package" << std::endl;
  }


  return 0;
}

bool InttCompareRawHit::Compare(InttRawHitContainer* rawmap1, InttRawHitContainer* rawmap2)
{
  if(rawmap1==nullptr || rawmap2==nullptr) {
    cout<<"pointer is null"<<endl;
    return false;
  }

  
  uint nhit1 = rawmap1->get_nhits();
  uint nhit2 = rawmap2->get_nhits();
  if(nhit1!=nhit2) {
    cout<<"nhits is different : "<<nhit1<<" "<<nhit2<<endl;
    return false;
  }

  bool resultall=true;
  for(uint ihit1=0; ihit1<nhit1; ihit1++){
    InttRawHit *hit1 = rawmap1->get_hit(ihit1);
    InttRawHit *hit2 = rawmap2->get_hit(ihit1);

    bool result = Compare(hit1, hit2);
    resultall &= result;
    if(!result) cout<<"failed ihit: "<<ihit1<<endl;
  }

  return resultall;
}

bool InttCompareRawHit::Compare(InttRawHit* raw1, InttRawHit* raw2)
{

  bool result = true;
  //result &= (raw1->get_bco()           == raw2-> get_bco()          );
  //result &= (raw1->get_packetid()      == raw2-> get_packetid()     );
  //result &= (raw1->get_word()          == raw2-> get_word()         );
  //result &= (raw1->get_fee()           == raw2-> get_fee()          );
  //result &= (raw1->get_channel_id()    == raw2-> get_channel_id()   );
  //result &= (raw1->get_chip_id()       == raw2-> get_chip_id()      );
  //result &= (raw1->get_adc()           == raw2-> get_adc()          );
  //result &= (raw1->get_FPHX_BCO()      == raw2-> get_FPHX_BCO()     );
  //result &= (raw1->get_full_FPHX()     == raw2-> get_full_FPHX()    );
  //result &= (raw1->get_full_ROC()      == raw2-> get_full_ROC()     );
  //result &= (raw1->get_amplitude()     == raw2-> get_amplitude()    );
  //result &= (raw1->get_event_counter() == raw2-> get_event_counter());
  
  result &= compare(raw1->get_bco()          , raw2-> get_bco()          , "bcofull");
  result &= compare(raw1->get_packetid()     , raw2-> get_packetid()     , "packetid");
  result &= compare(raw1->get_word()         , raw2-> get_word()         , "word");
  result &= compare(raw1->get_fee()          , raw2-> get_fee()          , "fee");
  result &= compare(raw1->get_channel_id()   , raw2-> get_channel_id()   , "channel_id");
  result &= compare(raw1->get_chip_id()      , raw2-> get_chip_id()      , "chip_id");
  result &= compare(raw1->get_adc()          , raw2-> get_adc()          , "adc");
  result &= compare(raw1->get_FPHX_BCO()     , raw2-> get_FPHX_BCO()     , "bco");
  result &= compare(raw1->get_full_FPHX()    , raw2-> get_full_FPHX()    , "full_fphx");
  result &= compare(raw1->get_full_ROC()     , raw2-> get_full_ROC()     , "full_roc");
  result &= compare(raw1->get_amplitude()    , raw2-> get_amplitude()    , "amplitude");
  result &= compare(raw1->get_event_counter(), raw2-> get_event_counter(), "event_counter");
  
  return result;
}

template<typename T> 
bool InttCompareRawHit::compare(const T& val1, const T& val2, std::string label)
{
  if(val1!=val2) {
    cout<<"failed : "<<label<<" : "<<val1<<" "<<val2<<endl;
    return false;
  }
  return true;
}
