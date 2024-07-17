#include "InttSkipEvent.h"

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

using namespace std;


/**
 * Constructor of module
 */
InttSkipEvent::InttSkipEvent(const std::string &name)
  : SubsysReco(name)
{
}

/**
 * Destructor of module
 */
InttSkipEvent::~InttSkipEvent()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttSkipEvent::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttSkipEvent" << std::endl;
  }

  return 0;
}

int InttSkipEvent::InitRun(PHCompositeNode * topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttSkipEvent" << std::endl;
  }

  if(!topNode)
  {
	  std::cout << "InttSkipEvent::InitRun(PHCompositeNode* topNode)" << std::endl;
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
int InttSkipEvent::process_event(PHCompositeNode* /*topNode*/)
{
  static int event=0;
  static int nskip=0;

  Fun4AllServer *se = Fun4AllServer::instance();
  PHCompositeNode *topnode1 = se->topNode("TOP");  // default INTT DST
  //PHCompositeNode *topnode2 = se->topNode("TOP2"); // special INTT DST

  //cout<<"top"<<endl;
  //topNode->print();
  //cout<<"top1"<<endl;
  //topnode1->print();
  //cout<<"top2"<<endl;
  //topnode2->print();

  Gl1RawHit     *gl1     = findNode::getClass<Gl1RawHit>(    topnode1, "GL1RAWHIT");
  //InttEventInfo *intthdr = findNode::getClass<InttEventInfo>(topnode2, "INTTEVENTHEADER");

  InttRawHitContainer* rawhitcont1 = findNode::getClass<InttRawHitContainer>(topnode1, "INTTRAWHIT");

  uint64_t bcogl1  = (gl1    !=nullptr) ? gl1->get_bco()          : std::numeric_limits<uint64_t>::max();
  //uint64_t bcointt = (intthdr!=nullptr) ? intthdr->get_bco_full() : std::numeric_limits<uint64_t>::max();

  uint64_t bcointt1 = (rawhitcont1!=nullptr&&rawhitcont1->get_nhits()>0)
                           ? rawhitcont1->get_hit(0)->get_bco()
                           : std::numeric_limits<uint64_t>::max();


  if(bcogl1  == std::numeric_limits<uint64_t>::max()/* || bcointt == std::numeric_limits<uint64_t>::max()*/ ){
    cout<<"StreamSkipEvent bco is max. not valid"<<endl;
  }
  // to 40bit
  if(bcointt1 == std::numeric_limits<uint64_t>::max() ) 
  {
    cout<<"StreamSkipEvent inttbco1 is max. no intt1 data valid. skip nth: "<<nskip<<endl;
    nskip++;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  bcogl1   &= 0xFFFFFFFFFF;
  bcointt1 &= 0xFFFFFFFFFF;

  //--if(bcogl1 < (bcointt-1)){ // inttbco is always 1 higher than gl1bco
  //--  cout<<"   bco is exceed. skip inttdst nth="<<nskip<<" : gl1:0x"<<hex<<bcogl1<<", intt:0x"<<bcointt<<dec<<endl;
  //--  nskip++;
  //--  return Fun4AllReturnCodes::ABORTEVENT;
  //--}

/*
  InttRawHitContainer* rawhitcont2 = findNode::getClass<InttRawHitContainer>(topnode2, "INTTRAWHIT");

  bool result = Compare(rawhitcont1, rawhitcont2);
  string s_result = (result) ? "rawhit is OK" : "rawhit is different";
  //  cout<<""<<endl;
  //}
  //if(!result){
  //  cout<<"rawhit is different"<<endl;
  //}

  cout<<"InttStreamSkipEvent::process_event : "<<event;
  cout<<" gl1:0x"<<hex<<bcogl1<<", intt:0x"<<bcointt<<dec<<" "<<s_result<<endl;
*/

  event++;
  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttSkipEvent::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttSkipEvent analysis package" << std::endl;
  }


  return 0;
}


