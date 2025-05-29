#include "MyTimer.h"

/// Cluster/Calorimeter includes

/// Fun4All includes
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>


#include <ffarawobjects/Gl1RawHit.h>
#include <ffarawobjects/Gl1Packet.h>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <filesystem>

using namespace std;


/**
 * Constructor of module
 */
MyTimer::MyTimer(const std::string &name, const std::string& outfile)
  : SubsysReco(name)
  , m_outfile(outfile)
  , m_timer(name)
  , m_nevt(1)
{
}

/**
 * Destructor of module
 */
MyTimer::~MyTimer()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int MyTimer::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in MyTimer" << std::endl;
  }

  return 0;
}

int MyTimer::InitRun(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in MyTimer" << std::endl;
  }


  std::cout<<"MyTimer:: nevt = "<<m_nevt<<std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int MyTimer::process_event(PHCompositeNode* /*topNode*/)
{
  static int event=0;

  if((event%m_nevt)==0) {
    if(m_timer.get_state()==PHTimer::RUN){
      m_timer.stop();
    }

  
    std::cout<<"MyTimer evt :"<<event<<" ";
    if (m_timer.get_ncycle())
    {
      std::cout << ": accumulated time (ms):  " << m_timer.get_accumulated_time() << std::endl;
      std::cout << ": per event time (ms):    " << m_timer.get_accumulated_time() / m_timer.get_ncycle() << std::endl;
    }
    else {
      std::cout<<std::endl;
    }

  //m_timer.print_stat();

    m_timer.restart();

  }

  event++;


  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int MyTimer::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending MyTimer analysis package" << std::endl;
  }

  //m_froot->Write();
  //m_froot->Close();

  return 0;
}

