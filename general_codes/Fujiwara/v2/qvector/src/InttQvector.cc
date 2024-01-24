#include "InttQvector.h"

#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainerv1.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv3.h>
#include <trackbase/ActsTrackingGeometry.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/ActsSurfaceMaps.h>
#include <trackbase/InttDefs.h>
#include <Acts/Surfaces/Surface.hpp>
#include <Acts/Surfaces/CylinderSurface.hpp>
#include <trackreco/PHActsSiliconSeeding.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

/// C++ includes
#include <cassert>
#include <sstream>
#include <string>
#include <typeinfo>

/// ROOT includes
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>

// Geometory includes
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <intt/CylinderGeomIntt.h>

#include "InttOfflineCluster.h"

using namespace std;

InttQvector::InttQvector(const std::string &name,const std::string &filename)
  :SubsysReco(name)
  , fname_(filename)
  , anafile_(nullptr)
  , tree1(nullptr)
  , tree2(nullptr)
{
  cout<<"InttQvector::InttQvector is calling ctor"<<endl;
}

InttQvector::~InttQvector()
{
  cout<<"InttQvector::~InttQvector is calling dtor"<<endl;
}

int InttQvector::Init(PHCompositeNode * /*topNode*/)
{
  cout << "Beginning Init in InttQvector" << endl;

  anafile_ = new TFile(fname_.c_str(), "recreate");
  
  tree1 = new TTree("Qvectortree", "Qvector tree");
  tree2 = new TTree("InttOfflinetree", "Offline Cluster List");
  inttOfflineCls_ = new InttOfflineClusterList();

  tree1->Branch("nclus",&nclus);
  tree1->Branch("Qx",&qx);
  tree1->Branch("Qy",&qy);
  tree1->Branch("psi",&psi);

  tree2->Branch("offcluster", "InttOfflineClusterList", &inttOfflineCls_, 8000, 99);
  
  psidis = new TH1F("psidis","psi all",120,-3,3);
  qvecdis = new TH2S("qvecdis","q all",1000,-5,5,1000,-5,5);

  adcdis = new TH1F("adcdis","adc all",4000,0,20000);

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttQvector::InitRun(PHCompositeNode * /*topNode*/)
{
  cout << "Beginning InitRun in InttQvector" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttQvector::process_event(PHCompositeNode *topNode)
{
  //cout << "Beginning process_event in InttQvector " << endl;
  NodeInitialize(topNode);
  
  double qvec[3]={0.,0.,0.};
  cal_q(qvec);
  qx = qvec[0];
  qy = qvec[1];
  nclus = qvec[2];

  psi = 0.5*atan2(qx,qy);
  tree1->Fill();
  tree2->Fill();
  psidis->Fill(psi);
  qvecdis->Fill(qx,qy);
  //cout <<"nclus = "<<nclus<<endl;
  //cout <<"(qx , qy )= ("<<qx<<" , "<<qy<<")"<<endl;
  //cout <<"psi = "<<psi<<endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttQvector::Reset(PHCompositeNode */*topNode*/)
{
  //cout << "Beginning Reset in InttQvector " << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttQvector::ResetEvent(PHCompositeNode */*topNode*/)
{
  //cout << "Beginning ResetEvent in InttQvector " << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttQvector::End(PHCompositeNode * /*topNode*/)
{
  cout << "Beginning End in InttQvector" << endl;
  /*
    c1 = new TCanvas("","",1200,1200);
    c1->Divide(2,1);
    c1->cd(1);
    psidis->Draw();
    c1->cd(2);
    qvecdis->Draw();
  */
  
  if(anafile_!=nullptr){
    anafile_->Write();
    anafile_->Close();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttQvector::EndRun(const int runnumber)
{
  std::cout << "tutorial::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttQvector::NodeInitialize(PHCompositeNode *topNode)
{
  geom_container = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");

  geom = dynamic_cast<CylinderGeomIntt *>(geom_container->GetLayerGeom(3));
  if (geom == NULL)
    cout << "No CylinderGeomIntt" << endl;
  
  m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_tGeometry)
    {
      std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing."
		<< std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  else
    //cout << "ActGeometry on node tree is successfully loaded" << endl;
  
  m_clusterMap = findNode::getClass<TrkrClusterContainer>(topNode,
							  "TRKR_CLUSTER");
  if (!m_clusterMap)
    {
      cout << PHWHERE
	   << "TrkrClusterContainer node is missing."
	   << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttQvector::cal_q(double qvec[])
{
  double sumx=0;
  double sumy=0;
  double count=0;
  double phi;

  inttOfflineCls_->clear();

  for (unsigned int inttlayer = 0; inttlayer < m_nInttLayers; inttlayer++)
    {
      for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
	{
	  auto range = m_clusterMap->getClusters(hitsetkey);
	  
	  for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
	    {
	      const auto cluskey = clusIter->first;
	      const auto cluster = clusIter->second;
	      const auto globalPos = m_tGeometry->getGlobalPosition(cluskey, cluster);

	      phi = atan2(globalPos(1),globalPos(0));
	      sumx = sumx + cos(2*phi);
	      sumy = sumy + sin(2*phi);
	      count++;

	      {
		InttOfflineCluster *cls = inttOfflineCls_->addCluster();
		
		//const auto cluskey = clusIter->first;
		//const auto cluster = clusIter->second;
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

		cls->global[0]  = globalPos.x();
		cls->global[1]  = globalPos.y();
		cls->global[2]  = globalPos.z();
		
		adcdis->Fill(cls->adc);
	      }
	    }
	}
    }
  
  qvec[0] = sumx/count;
  qvec[1] = sumy/count;
  qvec[2] = count;
  
  return;
}
