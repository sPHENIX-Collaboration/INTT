#include "InttClusAna.h"

/// Cluster/Calorimeter includes

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrClusterHitAssoc.h>
//#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>
#include <trackbase/MvtxDefs.h>


/// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TTree.h>

/// C++ includes
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

#include "InttEvent.h"
#include "InttRawData.h"

using namespace std;

/**
 * This class demonstrates the construction and use of an analysis module
 * within the sPHENIX Fun4All framework. It is intended to show how to
 * obtain physics objects from the analysis tree, and then write them out
 * to a ROOT tree and file for further offline analysis.
 */

/**
 * Constructor of module
 */
InttClusAna::InttClusAna(const std::string &name, const std::string& /*filename*/)
  : SubsysReco(name)
{
}

/**
 * Destructor of module
 */
InttClusAna::~InttClusAna()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttClusAna::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttClusAna" << std::endl;
  }


  return 0;
}

int InttClusAna::InitRun(PHCompositeNode * /*topNode*/)
{
//  cout<<"InttClusAna::InitRun beamcenter "<<xbeam_<<" "<<ybeam_<<endl;

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */




int InttClusAna::process_event(PHCompositeNode *topNode)
{
  static int ievt=0;
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in InttClusAna" << std::endl;
  }

  std::cout<<"event "<<ievt<<std::endl;
  ievt++;

  ActsGeometry *m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_tGeometry)
  {
    std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


  TrkrClusterContainer *m_clusterMap = 
	  findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!m_clusterMap)
  {
      cout << PHWHERE << "TrkrClusterContainer node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }


  TrkrClusterHitAssoc* clusterhitassoc = findNode::getClass<TrkrClusterHitAssoc>(topNode, "TRKR_CLUSTERHITASSOC");
  if (!clusterhitassoc)
  {
      cout << PHWHERE << "TrkrClusterHitAssoc node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }


  TrkrHitSetContainer* hits = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!hits)
  {
    std::cout << PHWHERE << "ERROR: Can't find node TRKR_HITSET" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

////////////////////
//
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range  = m_clusterMap->getClusters(hitsetkey);
      auto hitset = hits->findHitSet(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
         const auto ckey    = clusIter->first;
         const auto cluster = clusIter->second;

         const auto hitrange = clusterhitassoc->getHits(ckey);

         int ladder_phi = InttDefs::getLadderPhiId(ckey);
         int ladder_z   = InttDefs::getLadderZId(ckey);

         int phisize = cluster->getPhiSize();
         const int cluster_size = std::distance( hitrange.first, hitrange.second );

         std::cout<<inttlayer<<" "<<ladder_phi<<" "<<ladder_z<<" "<<phisize<<" "<<cluster_size<<": ";
         for (auto hitIter = hitrange.first; hitIter != hitrange.second; ++hitIter)
         {
            auto hitkey = hitIter->second;
            int col = InttDefs::getCol(hitkey);
            int row = InttDefs::getRow(hitkey);
            auto hit = hitset->getHit(hitkey);

            std::cout<<"("<<col<<" "<<row<<" "<<hit->getAdc()<<"), ";
         }
         std::cout<<std::endl;
      }
    }
  }


/////////////
//// SvtxTrack
//  SvtxTrackMap* svtxtrackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");


  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttClusAna::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttClusAna analysis package" << std::endl;
  }

  return 0;
}

  
