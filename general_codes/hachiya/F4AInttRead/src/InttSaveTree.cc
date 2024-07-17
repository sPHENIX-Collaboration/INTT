#include "InttSaveTree.h"

/// Cluster/Calorimeter includes

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//#include <trackbase/TrkrClusterv4.h>
//#include <trackbase/TrkrClusterContainerv3.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>

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
#include "InttOfflineEvent.h"
#include "InttOfflineCluster.h"

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
InttSaveTree::InttSaveTree(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , _rawModule(nullptr)
  , fname_(filename)
  , anafile_(nullptr)
  , tree_(nullptr)
  , inttEvtCp_(nullptr)
  , inttOfflineEvt_(nullptr)
  , inttOfflineCls_(nullptr)
{
}

/**
 * Destructor of module
 */
InttSaveTree::~InttSaveTree()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttSaveTree::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttSaveTree" << std::endl;
  }

  inttEvtCp_      = new InttEvent();
  inttOfflineEvt_ = new InttOfflineEvent();
  inttOfflineCls_ = new InttOfflineClusterList();
  emcOfflineCls_  = new InttOfflineClusterList();
  mvtxOfflineCls_ = new InttOfflineClusterList();

  anafile_ = new TFile(fname_.c_str(), "recreate");

  tree_ = new TTree("tree", "Raw_Cluster tree");

  tree_->Branch("event",      "InttEvent",              &inttEvtCp_,      8000, 99);
  tree_->Branch("offevent",   "InttOfflineEvent",       &inttOfflineEvt_, 8000, 99);
  tree_->Branch("offcluster", "InttOfflineClusterList", &inttOfflineCls_, 8000, 99);
  tree_->Branch("emccluster", "InttOfflineClusterList", &emcOfflineCls_, 8000, 99);
  tree_->Branch("mvtxcluster","InttOfflineClusterList", &mvtxOfflineCls_, 8000, 99);


  return 0;
}

int InttSaveTree::InitRun(PHCompositeNode * /*topNode*/)
{
  cout<<"InttSaveTree::InitRun " <<endl;

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttSaveTree::process_event(PHCompositeNode *topNode)
{
  static int ievt=0;
  cout<<"InttEvt::process evt : "<<ievt++<<endl;

  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in AnaTutorial" << std::endl;
  }


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

/////////////
//  InttEvent from RawModule (not standard way)
  inttEvtCp_->clear();

  InttEvent *inttEvt = nullptr;
  if(_rawModule){
    inttEvt = _rawModule->getInttEvent();
  }

  if(inttEvt!=NULL) {
    inttEvtCp_->copy(inttEvt);
  }

  
  inttOfflineEvt_->clear();
  readRawHit(topNode);

  // fill cluster
  inttOfflineCls_->clear();
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range = m_clusterMap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
        InttOfflineCluster *cls = inttOfflineCls_->addCluster();

        const auto cluskey = clusIter->first;
        const auto cluster = clusIter->second;
        int hskey      = (cluskey>>32)&0xFFFFFFFF;
        int clusterid  = (cluskey    )&0xFFFFFFFF;

        cls->hitsetkey = hskey;
        cls->clusterid = clusterid;

        cls->local[0]  = cluster->getPosition(0);
        cls->local[1]  = cluster->getPosition(1);
        cls->adc       = cluster->getAdc();
        cls->phisize   = cluster->getPhiSize();
        cls->zsize     = cluster->getZSize();
        cls->overlap   = cluster->getOverlap();
        cls->edge      = cluster->getEdge();

        const auto globalPos = m_tGeometry->getGlobalPosition(cluskey, cluster);
        cls->global[0]  = globalPos.x();
        cls->global[1]  = globalPos.y();
        cls->global[2]  = globalPos.z();

        //cout<<"cluster pos: ";
        //cout<<globalPos.x()<<" ";
        //cout<<globalPos.y()<<" ";
        //cout<<globalPos.z()<<endl;


      }
    }
  }

  static int evtCount=0;

/*
/////////////
  int nclusadd=0, nclusadd2=0;
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range = m_clusterMap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
         nclusadd ++;
         const auto cluster = clusIter->second;
         if(cluster->getAdc()>40) nclusadd2++;

      }
    }
  }

/////////////

  cout<< "evtCount : "<<evtCount<<" "<<evtseq<<" "<<hex<<bco<<dec<<endl;

  struct ClustInfo {
    int layer;
    Acts::Vector3 pos;
  };

  float ntpval[20];
  int nCluster=0;
  bool exceedNwrite=false;
  //std::vector<Acts::Vector3> clusters[2]; // inner=0; outer=1
  std::vector<ClustInfo> clusters[2]; // inner=0; outer=1
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    int layer= (inttlayer<2?0:1);
    for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range = m_clusterMap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
        const auto cluskey = clusIter->first;
        const auto cluster = clusIter->second;
        const auto globalPos = m_tGeometry->getGlobalPosition(cluskey, cluster);
        int ladder_z   = InttDefs::getLadderZId(cluskey);
        int ladder_phi = InttDefs::getLadderPhiId(cluskey);
        int size       = cluster->getSize();

        if(nCluster<5) {
            cout<<"xyz : "<<globalPos.x()<<" "<< globalPos.y()<<" "<< globalPos.z()<<" :  "
                <<cluster->getAdc()<<" "<<size<<" "<<inttlayer<<" "<<ladder_z<<" "<<ladder_phi<<endl;
        }
        else {
          if(!exceedNwrite) {
            cout<<" exceed : ncluster limit.  no more cluster xyz printed"<<endl;
            exceedNwrite=true;
          }
        }

        ClustInfo info;
        info.layer = inttlayer;
        info.pos   = globalPos;

	//clusters[layer].push_back(globalPos);
	clusters[layer].push_back(info);
        nCluster++;

        ntpval[0] = nclusadd; //bco_full
        ntpval[1] = nclusadd2; //bco_full
        ntpval[2] = bco; //bco_full
        ntpval[3] = evtseq; //evtCount;
        ntpval[4] = size; // size
        ntpval[5] = cluster->getAdc(); // size
        ntpval[6] = globalPos.x();
        ntpval[7] = globalPos.y();
        ntpval[8] = globalPos.z();
        ntpval[9] = inttlayer;
        ntpval[10] = ladder_phi;
        ntpval[11] = ladder_z;
        h_ntp_clus->Fill(ntpval);
        // = new TNtuple("ntp_clus", "Cluster Ntuple", "bco_full:evt:size:adc:x:y:z:lay:lad:sen");
      }
    }
  }
  cout<<"nCluster : "<<nCluster<<endl;
*/

  tree_->Fill();

  evtCount++;

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttSaveTree::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttSaveTree analysis package" << std::endl;
  }

  if(anafile_!=nullptr){
    anafile_->Write();
    anafile_->Close();
  }

  return 0;
}

void InttSaveTree::readRawHit(PHCompositeNode* topNode)
{
  TrkrHitSetContainer* m_hits = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!m_hits)
  {
    cout << PHWHERE << "ERROR: Can't find node TRKR_HITSET" << endl;
    return ;
  }

  // loop over the InttHitSet objects
  TrkrHitSetContainer::ConstRange hitsetrange =
          m_hits->getHitSets(TrkrDefs::TrkrId::inttId);

  for (TrkrHitSetContainer::ConstIterator hitsetitr = hitsetrange.first;
      hitsetitr != hitsetrange.second;
      ++hitsetitr)
  {
    // Each hitset contains only hits that are clusterizable - i.e. belong to a single sensor
    int         hitsetkey = hitsetitr->first;
    TrkrHitSet *hitset    = hitsetitr->second;
    
    TrkrHitSet::ConstRange hitrangei    = hitset->getHits();
    for (TrkrHitSet::ConstIterator hitr = hitrangei.first;
         hitr != hitrangei.second;
         ++hitr)
    {
      int hitkey = hitr->first;

      InttOfflineHit* hit = inttOfflineEvt_->addHit();
      hit->hitsetkey      = hitsetkey;
      hit->hitkey         = hitkey;
      hit->adc            = (hitr->second)->getAdc();
    }

  }
}

