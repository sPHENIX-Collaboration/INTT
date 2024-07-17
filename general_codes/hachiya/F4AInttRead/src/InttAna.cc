#include "InttAna.h"

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
//#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>
#include <trackbase/MvtxDefs.h>

#include <trackbase/InttEventInfo.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

//#include <globalvertex/MbdVertex.h>
//#include <globalvertex/MbdVertexMap.h>
#include <mbd/MbdOut.h>
#include <ffarawobjects/Gl1RawHit.h>

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>


#include "inttxyvertexfinder/InttVertex3D.h"
#include "inttxyvertexfinder/InttVertex3DMap.h"


#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>


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
InttAna::InttAna(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , _rawModule(nullptr)
  , fname_(filename)
  , anafile_(nullptr)
  , h_zvtxseed_(nullptr)
{
  xbeam_ = ybeam_ = 0.0;
}

/**
 * Destructor of module
 */
InttAna::~InttAna()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttAna::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttAna" << std::endl;
  }

  anafile_ = new TFile(fname_.c_str(), "recreate");
  h_dca2d_zero  = new TH1F("h_dca2d_zero", "DCA2D to 0", 500, -10, 10);
  h2_dca2d_zero = new TH2F("h2_dca2d_zero", "DCA2D to 0 vs nclus", 500, -10, 10, 100, 0, 10000);
  h2_dca2d_len  = new TH2F("h2_dca2d_len", "DCA2D to 0 vs len", 500, -10, 10, 500, -10, 10);
  h_zvtx        = new TH1F("h_zvtx", "Zvertex", 400, -40, 40);

  h_ntp_clus = new TNtuple("ntp_clus", "Cluster Ntuple", "nclus:nclus2:bco_full:evt:size:adc:x:y:z:lay:lad:sen:lx:ly:phisize:zsize:zv");
  h_ntp_emcclus = new TNtuple("ntp_emcclus", "EMC Cluster Ntuple", "nclus:evt:x:y:z:e:ecore:size");
  h_ntp_mvtxclus = new TNtuple("ntp_mvtxclus", "MVTX Cluster Ntuple", "nclus:evt:size:x:y:z:lay:lad:phisize:zsize:zv");

  h_ntp_cluspair = new TNtuple("ntp_cluspair", "Cluster Pair Ntuple", "nclus:nclus2:bco_full:evt:ang1:ang2:x1:x2:y1:y2:z1:z2:the1:the2:dca2d:len:unorm:l1:l2:vx:vy:vz:zvtx");
  h_ntp_emccluspair = new TNtuple("ntp_emccluspair", "INTT and EMC Cluster Pair Ntuple", "nclus:evt:ang1:ang2:x1:x2:y1:y2:z1:z2:dca2d:len:vx:vy:vz:eang:ez:ecore:esize:the1:the2:ethe:prjx:prjy:prjz:pang:pthe:px:py:pz");
  h_ntp_mvtxcluspair = new TNtuple("ntp_mvtxcluspair", "INTT and MVTX Cluster Pair Ntuple", "nclus:evt:ang1:ang2:x1:x2:y1:y2:z1:z2:dca2d:len:vx:vy:vz:m0ang:m0z:m0phisize:m0zsize:the1:the2:m0the");
  h_ntp_mvtxpair = new TNtuple("ntp_mvtxpair", "MVTX Cluster Pair Ntuple", "nclus:evt:mang0:mang1:mz0:mz1:vx:vy:vz:mthe0:mthe1");

  h_ntp_intttrk = new TNtuple("ntp_intttrk", "Intt Track Ntuple", "nclus:nclus2:bco_full:evt:ang1:ang2:x1:x2:y1:y2:z1:z2:the1:the2:dca2d:len:unorm:zvtx:pt:px:py:pz:c");
  h_ntp_trkpair = new TNtuple("ntp_trkpair", "Intt Track PairNtuple", 
                              "mee:px:py:pz:pt:pxp:pyp:pzp:pxm:pym:pyz");

  h_ntp_evt = new TNtuple("ntp_evt", "Event Ntuple", "nclus:nclus2:bco_full:evt:zv:zvs:zvm:zvc:bz:bqn:bqs:bfemclk:xvtx:yvtx:zvtx:nclusin:nclusout:nclszv:ntrkzv:chi2ndfzv:widthzv:ngroupzv:goodzv:peakratiozv:xvsim:yvsim:zvsim:xvsvtx:yvsvtx:zvsvtx:nmvtx0:nmvtx1:nmvtx2:ntrksvtx:nemc:nemc1:nemc2");


  h_zvtxseed_ = new TH1F("h_zvtxseed", "Zvertex Seed histogram", 200, -50, 50);

  h_t_evt_bco = new TTree("t_evt_bco", "Event Tree for BCO");
  h_t_evt_bco->Branch("evt_gl1",   &(m_evtbcoinfo.evt_gl1),  "evt_gl1/I");
  h_t_evt_bco->Branch("evt_intt",  &(m_evtbcoinfo.evt_intt), "evt_intt/i");
  h_t_evt_bco->Branch("evt_mbd",   &(m_evtbcoinfo.evt_mbd),  "evt_mbd/i");
  h_t_evt_bco->Branch("bco_gl1",   &(m_evtbcoinfo.bco_gl1),  "bco_gl1/l");
  h_t_evt_bco->Branch("bco_intt",  &(m_evtbcoinfo.bco_intt), "bco_intt/l");
  h_t_evt_bco->Branch("bco_mbd",   &(m_evtbcoinfo.bco_mbd),  "bco_mbd/l");
  h_t_evt_bco->Branch("pevt_gl1",  &(m_evtbcoinfo_prev.evt_gl1),  "pevt_gl1/I");
  h_t_evt_bco->Branch("pevt_intt", &(m_evtbcoinfo_prev.evt_intt), "pevt_intt/i");
  h_t_evt_bco->Branch("pevt_mbd",  &(m_evtbcoinfo_prev.evt_mbd),  "pevt_mbd/i");
  h_t_evt_bco->Branch("pbco_gl1",  &(m_evtbcoinfo_prev.bco_gl1),  "pbco_gl1/l");
  h_t_evt_bco->Branch("pbco_intt", &(m_evtbcoinfo_prev.bco_intt), "pbco_intt/l");
  h_t_evt_bco->Branch("pbco_mbd",  &(m_evtbcoinfo_prev.bco_mbd),  "pbco_mbd/l");



  return 0;
}

int InttAna::InitRun(PHCompositeNode * /*topNode*/)
{
  cout<<"InttAna::InitRun beamcenter "<<xbeam_<<" "<<ybeam_<<endl;

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttAna::process_event(PHCompositeNode *topNode)
{
  static int ievt=0;
  cout<<"InttEvt::process evt : "<<ievt++<<endl;

  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in AnaTutorial" << std::endl;
  }

  //readRawHit(topNode);

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

  InttEventInfo* inttevthead = findNode::getClass<InttEventInfo>(topNode, "INTTEVENTHEADER");

  InttVertex3DMap* inttvertexmap = findNode::getClass<InttVertex3DMap>(topNode, "InttVertexMap");

  double vtx_sim[3]{-9999,-9999,-9999};

  std::map<GlobalVertex::VTXTYPE, std::string> sourcemap{
               {GlobalVertex::VTXTYPE::UNDEFINED, "UNDEFINED"},
               {GlobalVertex::VTXTYPE::TRUTH,     "TRUTH"},
               {GlobalVertex::VTXTYPE::SMEARED,   "SMEARED"},
               {GlobalVertex::VTXTYPE::MBD,       "MBD"},
               {GlobalVertex::VTXTYPE::SVTX,      "SVTX"},
               {GlobalVertex::VTXTYPE::SVTX_MBD,  "SVTX_MBD"}
            };
	//   enum VTXTYPE
  std::string s_vtxid = "NULL";

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (vertexmap){
    if (!vertexmap->empty()){

      for(auto vertex : *vertexmap)
      {
        std::cout<<"map entry"<<std::endl;
        std::map<GlobalVertex::VTXTYPE, std::string>::iterator itr_src;
        itr_src = sourcemap.find((GlobalVertex::VTXTYPE)vertex.first);
        if( itr_src != sourcemap.end()) 
        {
           s_vtxid = itr_src->second;
        }

        if(vertex.first == GlobalVertex::VTXTYPE::TRUTH){
          vtx_sim[0] = vertex.second->get_x();
          vtx_sim[1] = vertex.second->get_y();
          vtx_sim[2] = vertex.second->get_z();
        }

        std::cout<<std::endl<<"A "<<vertex.second->get_x()
                            <<" " <<vertex.second->get_y()
                            <<" " <<vertex.second->get_z()
                            <<" " <<vertex.second->get_id()
                            <<" " <<s_vtxid<<std::endl;
      }
    }
  }

/*
  MbdVertexMap *mbdvertexmap = findNode::getClass<MbdVertexMap>(topNode, "MbdVertexMap");
  MbdVertex* mbdvtx = nullptr;
  if (mbdvertexmap){
    cout<<"MbdVertexMap"<<endl;
    if (!mbdvertexmap->empty()){

      for(auto mbdvertex : *mbdvertexmap)
      {
        std::cout<<"mbdmap entry"<<std::endl;

        std::cout<<std::endl<<"Mbdz"
                            <<" " <<mbdvertex.second->get_z()
                            <<" " <<mbdvertex.second->get_id()<<endl;
        mbdvtx = mbdvertex;
      }
    }
    else {
      cout<<"MbdVertexMap empty"<<endl;
    }
  }
*/

  SvtxVertexMap *svtxvertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  SvtxVertex* svtxvertex = nullptr;
  if(svtxvertexmap){
    cout<<"svtxvertex: size : "<<svtxvertexmap->size()<<endl;
    for (SvtxVertexMap::ConstIter iter = svtxvertexmap->begin(); iter != svtxvertexmap->end(); ++iter)
    {
      svtxvertex = iter->second;
      std::cout<<std::endl<<"SvtxVertex"
                          <<" " <<svtxvertex->get_x()
                          <<" " <<svtxvertex->get_y()
                          <<" " <<svtxvertex->get_z()
                          <<" " <<svtxvertex->get_id()<<endl;
    }
  }
  else {
    cout<<"SvtxVertexMap is not available"<<endl;
  }




  MbdOut *mbdout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if(!mbdout){
    cout<<"No MbdOut"<<endl;
  }
  double mbdqs = (mbdout!=nullptr) ?  mbdout->get_q(0) : -9999;
  double mbdqn = (mbdout!=nullptr) ?  mbdout->get_q(1) : -9999;
//  double mbdt0 = (mbdout!=nullptr) ?  mbdout->get_t0() : -9999;
  double mbdz  = (mbdout!=nullptr) ?  mbdout->get_zvtx() : -9999;

  Gl1RawHit *gl1raw = findNode::getClass<Gl1RawHit>(topNode, "GL1RAWHIT");
  if(!gl1raw){
    cout<<"No Gl1Raw"<<endl;
  }

  InttRawHitContainer* inttrawmap = findNode::getClass<InttRawHitContainer>(topNode, "INTTRAWHIT");
  InttRawHit*          inttraw    = (inttrawmap!=nullptr && inttrawmap->get_nhits()>0) ? inttrawmap->get_hit(0) : nullptr;
/////////////
//  InttEvent from RawModule (not standard way)
  InttEvent *inttEvt = nullptr;
  if(_rawModule){
    inttEvt = _rawModule->getInttEvent();
  }
  uint64_t bco = 0;
  int      evtseq = -1;
  if(inttEvt!=NULL) {
    bco    = inttEvt->bco;
    evtseq = inttEvt->evtSeq;
  }
  if(inttevthead){
    bco = inttevthead->get_bco_full();
    //evtseq = inttEvt->evtSeq;
  }
  cout<<"bco 0x"<<hex<<bco<<dec<<endl;

  if(gl1raw){
    bco    = gl1raw->get_bco();
    evtseq = gl1raw->getEvtSequence();
  }

  if(evtseq==-1) {
    evtseq = ievt;
  }


  double vertex[10][3]{};

  InttVertex3D* zvtxobj=NULL;
  if(inttvertexmap){
    int ivtx=0;
    cout<<"vertex map size : "<<inttvertexmap->size()<<endl;
    InttVertex3DMap::ConstIter biter_beg = inttvertexmap->begin();
    InttVertex3DMap::ConstIter biter_end = inttvertexmap->end();
    //cout<<"vertex map size : after size "<<endl;
    //inttvertexmap->identify();

    for(InttVertex3DMap::ConstIter biter = biter_beg; biter != biter_end; ++biter) {
      InttVertex3D* vtxobj = biter->second;
      if(vtxobj){
        cout<<"vtxobj"<<ivtx<<endl;
        vtxobj->identify();
        vertex[ivtx][0] = vtxobj->get_x();
        vertex[ivtx][1] = vtxobj->get_y();
        vertex[ivtx][2] = vtxobj->get_z();
        if(ivtx==2){ zvtxobj = vtxobj; }
      }
      else {
        cout<<"no vtx object : "<<ivtx<<endl;
      }
      ivtx++;
      if(ivtx>=10) {
        cout<<"n_vertex exceeded : "<<ivtx<<endl;
        break;
      }
    }
  }

///////////////////////////////////////
double zvertex = (m_useSimVtx) ? vtx_sim[2] : vertex[2][2];

m_xvertex = (m_useSimVtx) ? vtx_sim[0] : vertex[2][0];
m_yvertex = (m_useSimVtx) ? vtx_sim[1] : vertex[2][1];
m_zvertex = zvertex;

///////////////////////////////////////
// fill evtbco_info
  m_evtbcoinfo.clear();
  m_evtbcoinfo.evt_gl1  = (gl1raw     !=nullptr) ? gl1raw->getEvtSequence()     : -1;
  m_evtbcoinfo.evt_intt = (inttraw    !=nullptr) ? inttraw->get_event_counter() : -1;
  m_evtbcoinfo.evt_mbd  = (mbdout     !=nullptr) ? mbdout->get_evt()            : -1;
  m_evtbcoinfo.bco_gl1  = (gl1raw     !=nullptr) ? gl1raw->get_bco()            :  0;
  m_evtbcoinfo.bco_intt = (inttevthead!=nullptr) ? inttevthead->get_bco_full()  :  0;
  m_evtbcoinfo.bco_mbd  = (mbdout     !=nullptr) ? mbdout->get_femclock()       :  0;

  cout<<"event : "<<m_evtbcoinfo.evt_gl1<<" "
                  <<m_evtbcoinfo.evt_intt<<" "
                  <<m_evtbcoinfo.evt_mbd<<" :  "
             <<hex<<m_evtbcoinfo.bco_gl1<<dec<<" "
             <<hex<<m_evtbcoinfo.bco_intt<<dec<<" "
             <<hex<<m_evtbcoinfo.bco_mbd<<dec<< " : " 
             <<hex<< m_evtbcoinfo.bco_gl1 -m_evtbcoinfo_prev.bco_gl1 <<dec<<" "
             <<hex<< m_evtbcoinfo.bco_intt-m_evtbcoinfo_prev.bco_intt<<dec<<" "
             <<hex<< m_evtbcoinfo.bco_mbd -m_evtbcoinfo_prev.bco_mbd <<dec<<endl;

/////////////
  int nclusadd=0, nclusadd2=0;
  int nclus_inner=0, nclus_outer=0;
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

         if(inttlayer<2) nclus_inner++;
         else            nclus_outer++;

      }
    }
  }


///////////////////////////////////////

/////////////
  struct ClustInfo {
    int layer;
    Acts::Vector3 pos;
    TrkrCluster* cl;
  };

/////////////
// MVTX cluster
  std::vector<ClustInfo> mvtxclusters[3]; // inner=0; outer=1
  std::set<TrkrDefs::TrkrId> detectors;
  detectors.insert(TrkrDefs::TrkrId::mvtxId);
  int nclusmvtx[3] = {0,0,0};
  float ntpval_mvtx[20];
  for (const auto &det : detectors)
  {
    for (const auto &layer : {0, 1, 2})
    {
      for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(det, layer))
      {
        auto range = m_clusterMap->getClusters(hitsetkey);
        for (auto citer = range.first; citer != range.second; ++citer)
        {
          const auto ckey = citer->first;
          const auto cluster = citer->second;
          const auto global = m_tGeometry->getGlobalPosition(ckey, cluster);

          int phisize = cluster->getPhiSize();
          int zsize   = cluster->getZSize();

          ClustInfo info;
          info.layer= layer;
          info.pos  = global;
          info.cl   = citer->second;
          if(phisize<4&&zsize<4)
            mvtxclusters[layer].push_back(info);

          ntpval_mvtx[ 0] = 0; //evt;
          ntpval_mvtx[ 1] = evtseq; //evtCount;
          ntpval_mvtx[ 2] = cluster->getSize();
          ntpval_mvtx[ 3] = global.x();
          ntpval_mvtx[ 4] = global.y();
          ntpval_mvtx[ 5] = global.z();
          ntpval_mvtx[ 6] = layer;
          ntpval_mvtx[ 7] = MvtxDefs::getStaveId(ckey);
          ntpval_mvtx[ 8] = MvtxDefs::getChipId(ckey);
          ntpval_mvtx[ 9] = cluster->getPhiSize();
          ntpval_mvtx[10] = cluster->getZSize();
          ntpval_mvtx[11] = zvertex; //zvtx;
          h_ntp_mvtxclus->Fill(ntpval_mvtx);
  //h_ntp_mvtxclus = new TNtuple("ntp_mvtxclus", "MVTX Cluster Ntuple", "nclus:evt:size:x:y:z:lay:lad:sen:phisize:zsize:zv");
  
          nclusmvtx[layer]++;
        }
      }
    }
  }

/////////////
// SvtxTrack
  SvtxTrackMap* svtxtrackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");


/////////////
// EMC Cluster
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_POS_COR_CEMC");
  if(!clusterContainer)
  {
    std::cout << PHWHERE << "InttAna::process_event - Fatal Error - CLUSTERINFO_POS_COR_CEMC node is missing. Try next" << std::endl;
    clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");

    if(!clusterContainer)
    {
      std::cout << PHWHERE << "InttAna::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;
    }
  }
  //RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
 
  int nemc = -9999, nemc1 = -9999, nemc2=-9999;
  if(!clusterContainer)
  {
    std::cout << PHWHERE << "InttAna::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;
  }
  else {
    float ntpval_emccls[20];
    //This is how we iterate over items in the container.
    RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
    RawClusterContainer::ConstIterator clusterIter;

    
    nemc = clusterContainer->size();
    nemc1=0;
    nemc2=0;

    for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
    {
      RawCluster *recoCluster = clusterIter -> second;

      ntpval_emccls[0] = nemc;
      ntpval_emccls[1] = evtseq;
      ntpval_emccls[2] = recoCluster->get_x();
      ntpval_emccls[3] = recoCluster->get_y();
      ntpval_emccls[4] = recoCluster->get_z();
      ntpval_emccls[5] = recoCluster->get_energy();
      ntpval_emccls[6] = recoCluster->get_ecore();
      ntpval_emccls[7] = recoCluster->getNTowers();


      h_ntp_emcclus->Fill(ntpval_emccls);

      if(recoCluster->get_ecore()>0.15) nemc1++;
      if(recoCluster->get_ecore()>0.15 && recoCluster->getNTowers()>1) nemc2++;
    }
  }

/////////////
  static int evtCount=0;

  cout<< "evtCount : "<<evtCount<<" "<<evtseq<<" "<<hex<<bco<<dec<<endl;

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

        int ladder_z   = InttDefs::getLadderZId(cluskey);
        int ladder_phi = InttDefs::getLadderPhiId(cluskey);
        int size       = cluster->getSize();

        //cout    <<cluster->getPosition(0)<<" "
        //        <<cluster->getPosition(1)<<" : "
        //        <<cluster->getAdc()<<" "<<size<<" "<<inttlayer<<" "<<ladder_z<<" "<<ladder_phi<<endl;

        const auto globalPos = m_tGeometry->getGlobalPosition(cluskey, cluster);

        if(nCluster<5) {
            cout<<"xyz : "<<globalPos.x()<<" "<< globalPos.y()<<" "<< globalPos.z()<<" :  "
                <<cluster->getPosition(0)<<" "
                <<cluster->getPosition(1)<<" : "
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
        info.cl    = clusIter->second;

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
        ntpval[12] = cluster->getPosition(0);
        ntpval[13] = cluster->getPosition(1);
        ntpval[14] = cluster->getPhiSize();
        ntpval[15] = cluster->getZSize();
        ntpval[16] = zvertex; //zvtx;
        h_ntp_clus->Fill(ntpval);
        // = new TNtuple("ntp_clus", "Cluster Ntuple", "bco_full:evt:size:adc:x:y:z:lay:lad:sen");
      }
    }
  }
  cout<<"nCluster : "<<nCluster<<endl;


  double zvtx=-9999.;

/*
  struct cluspair {
    float p1_ang;
    float p2_ang;
    float p1_r;
    float p2_r;
    float p1_x;
    float p2_x;
    float p1_y;
    float p2_y;
    float p1_z;
    float p2_z;
    float p1_the;
    float p2_the;
    float dca_p0;
    float len_p0;
    TrkrCluster* p1;
    TrkrCluster* p2;
  };
*/

  //vector<cluspair> vcluspair;
  vcluspair.clear();

  Acts::Vector3 beamspot   = Acts::Vector3(xbeam_,ybeam_,0);
  Acts::Vector3 mvtxoffset = Acts::Vector3(xmvtx_,ymvtx_,0);

  if(nCluster<300)
  //if(nCluster<500)
  {
    float ntpval2[25];
    vector<double> vz_array;
    for(auto c1=clusters[0].begin(); c1!=clusters[0].end(); ++c1) // inner
    {
      for(auto c2=clusters[1].begin(); c2!=clusters[1].end(); ++c2) // outer
      {
          Acts::Vector3 p1 = c1->pos - beamspot;
          Acts::Vector3 p2 = c2->pos - beamspot;
          Acts::Vector3 u = p2 - p1;
          double unorm = sqrt(u.x()*u.x()+u.y()*u.y());
          if(unorm<0.00001) continue;

          // skip bad compbination
          double p1_ang = atan2(p1.y(), p1.x());
          double p2_ang = atan2(p2.y(), p2.x());
          double d_ang = p2_ang - p1_ang;


          // unit vector in 2D
          double ux = u.x()/unorm;
          double uy = u.y()/unorm;
          double uz = u.z()/unorm; // same normalization factor(2D) is multiplied

          //Acts::Vector3 p0   = beamspot - p1;
          Acts::Vector3 p0   = Acts::Vector3(0,0,0) - p1; // p1, p2 are already shifted by beam center

          double dca_p0 = p0.x()*uy - p0.y()*ux; // cross product of p0 x u
          double len_p0 = p0.x()*ux + p0.y()*uy; // dot product of p0 . u

         
          // beam center in X-Y plane
          double vx = len_p0*ux + p1.x();
          double vy = len_p0*uy + p1.y();

          double vz = len_p0*uz + p1.z();

          double p1_r = sqrt(p1.y()*p1.y() + p1.x()*p1.x());
          double p2_r = sqrt(p2.y()*p2.y() + p2.x()*p2.x());

          float p1_the = atan2(p1_r, (p1.z() - zvertex));
          float p2_the = atan2(p2_r, (p2.z() - zvertex));

          //if(unorm>4.5||d_ang<-0.005*3.1415||0.005*3.1415<d_ang) 
          //if(unorm>4.5||d_ang<-0.25*3.1415||0.25*3.1415<d_ang) 
          if(fabs(d_ang)<0.05 /*&& fabs(dca_p0) < 0.5*/) // theta cut should be introduced
          {
            h_zvtxseed_->Fill(vz);
            vz_array.push_back(vz);

            cluspair clspair;
            clspair.p1_ang = p1_ang;                
            clspair.p2_ang = p2_ang;
            clspair.p1_r   = p1_r;                
            clspair.p2_r   = p2_r;
            clspair.p1_x   = p1.x();
            clspair.p2_x   = p2.x();
            clspair.p1_y   = p1.y();
            clspair.p2_y   = p2.y();
            clspair.p1_z   = p1.z();
            clspair.p2_z   = p2.z();
            clspair.p1_the = p1_the;
            clspair.p2_the = p2_the;
            clspair.dca_p0 = dca_p0;
            clspair.len_p0 = len_p0;
            clspair.p1 = c1->cl;
            clspair.p2 = c2->cl;
            vcluspair.push_back(clspair);
          }


          h_dca2d_zero->Fill(dca_p0);
          h2_dca2d_zero->Fill(dca_p0, nCluster);
          h2_dca2d_len->Fill(dca_p0, len_p0);
          //cout<<"dca : "<<dca_p0<<endl;
          //
          ntpval2[ 0]  = nclusadd;
          ntpval2[ 1]  = nclusadd2;
          ntpval2[ 2]  = 0;
          ntpval2[ 3]  = evtCount;
          ntpval2[ 4]  = p1_ang;
          ntpval2[ 5]  = p2_ang;
          ntpval2[ 6]  = p1.x();
          ntpval2[ 7]  = p2.x();
          ntpval2[ 8]  = p1.y();
          ntpval2[ 9]  = p2.y();
          ntpval2[10]  = p1.z();
          ntpval2[11]  = p2.z();
          ntpval2[12]  = p1_the;
          ntpval2[13]  = p2_the;
          ntpval2[14]  = dca_p0;
          ntpval2[15]  = len_p0;
          ntpval2[16] = unorm;
          ntpval2[17] = c1->layer;
          ntpval2[18] = c2->layer;
          ntpval2[19] = vx;
          ntpval2[20] = vy;
          ntpval2[21] = vz;
          ntpval2[22] = zvertex;//zvtx;

          if(evtCount<50000)
            h_ntp_cluspair->Fill(ntpval2);
          //h_ntp_cluspair = new TNtuple("ntp_cluspair", "Cluster Pair Ntuple", "nclus:bco_full:evt:ang1:ang2:dca2d:len:unorm");
          //h_ntp_cluspair "nclus:nclus2:bco_full:evt:ang1:ang2:x1:x2:y1:y2:z1:z2:the1:the2:dca2d:len:unorm:l1:l2:vx:vy:vz:zvtx");
      }
    }

    ////////////////////////////////////
    // calculate trancated mean of DCA~Z histogram as Z-vertex position
    if(vz_array.size()>3){
      double zbin    = h_zvtxseed_->GetMaximumBin();
      double zcenter = h_zvtxseed_->GetBinCenter(zbin);
      double zmean   = h_zvtxseed_->GetMean();
      double zrms    = h_zvtxseed_->GetRMS();
      if(zrms<20) zrms = 20;

      double zmax = zcenter + zrms; // 1 sigma
      double zmin = zcenter - zrms; // 1 sigma

      double zsum=0.;
      int    zcount=0;
      for(auto iz=vz_array.begin(); iz!=vz_array.end(); ++iz){
        double vz = (*iz);
        if(zmin<vz&&vz<zmax){
          zsum+=vz;
          zcount++;
        }
      }
      if(zcount>0) zvtx = zsum/zcount;

      cout<<"ZVTX: "<<zvtx<<" "<<zcenter<<" "<<zmean<<" "<<zrms<<" "<<zbin<<endl;
    }
    h_zvtx->Fill(zvtx);


    float ntpval5[25];
    ////////////////////////////////////
    // calculate momentum for B-ON data
    Acts::Vector3 beamspot_z = beamspot;
    beamspot_z[2] = zvertex;
    for(vector<cluspair>::iterator itrcp=vcluspair.begin(); itrcp!=vcluspair.end(); ++itrcp){
      cluspair& clspair = *itrcp;

      // fit xyplane with straight line
      linefitXYplane(clspair, beamspot_z);

      // fit rzplane with straight line
      linefitRZplane(clspair, beamspot_z);

      // pT and circle
      calculateMomentum(clspair, beamspot);


      ntpval5[ 0]  = nclusadd;
      ntpval5[ 1]  = nclusadd2;
      ntpval5[ 2]  = 0;
      ntpval5[ 3]  = evtCount;
      ntpval5[ 4]  = clspair.p1_ang;
      ntpval5[ 5]  = clspair.p2_ang;
      ntpval5[ 6]  = clspair.p1_x;
      ntpval5[ 7]  = clspair.p2_x;
      ntpval5[ 8]  = clspair.p1_y;
      ntpval5[ 9]  = clspair.p2_y;
      ntpval5[10]  = clspair.p1_z;
      ntpval5[11]  = clspair.p2_z;
      ntpval5[12]  = clspair.p1_the;
      ntpval5[13]  = clspair.p2_the;
      ntpval5[14]  = clspair.dca_p0;
      ntpval5[15]  = clspair.len_p0;
      ntpval5[16]  = 0; //clspair.unorm;
      ntpval5[17]  = zvertex;
      ntpval5[18]  = clspair.pt;
      ntpval5[19]  = clspair.px;
      ntpval5[20]  = clspair.py;
      ntpval5[21]  = clspair.pz;
      ntpval5[22]  = clspair.charge;

      // "nclus:nclus2:bco_full:evt:ang1:ang2:x1:x2:y1:y2:z1:z2:the1:the2:dca2d:len:unorm:zvtx:pt:px:py:pz:c");

      h_ntp_intttrk->Fill(ntpval5);
    }
   

    ////////////////////////////////////

    float ntpval_emcpair[30];
    if(clusterContainer!=nullptr) {
      RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
      RawClusterContainer::ConstIterator clusterIter;
      
      ///////////// 
      //  association to EMCAL
      for(vector<cluspair>::iterator itrcp=vcluspair.begin(); itrcp!=vcluspair.end(); ++itrcp){
        cluspair& clspair = *itrcp;

        projectTrack(clspair, beamspot_z, 93.5, // EMC radius is 93.5cm
                     clspair.prj_emc[0], clspair.prj_emc[1], clspair.prj_emc[2]);

        //float prj_phi = atan2(clspair.prj_emc[1]-beamspot.y(), clspair.prj_emc[0]-beamspot.x());
        float prj_phi = atan2(clspair.prj_emc[1], clspair.prj_emc[0]);
        float prj_the = atan2(1., clspair.slope_rz);


        float pt = sqrt(clspair.px*clspair.px + clspair.py*clspair.py);
        cout<<"clspair : "<<prj_the<<" "<<clspair.slope_rz<<" "<<clspair.pz<<" "<<pt<<endl;


        float small_dphi = 9999;
        float small_dthe = 9999;
        float small_dphithe = 9999;
        RawCluster *closeCluster = nullptr;

        for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
        {
          RawCluster *recoCluster = clusterIter -> second;

          if(recoCluster->get_ecore()<0.10) continue; // ignore Ecore<0.1GeV

          Acts::Vector3 emcpos   = Acts::Vector3(recoCluster->get_x(), recoCluster->get_y(), recoCluster->get_z());
          Acts::Vector3 emcposc  = emcpos - beamspot;
          float ephi = atan2(emcposc.y(), emcposc.x());
          float er   = sqrt(emcposc.x()*emcposc.x() + emcposc.y()*emcposc.y());
          float ethe = atan2(er, emcposc.z()-vertex[2][2]);

          //--ntpval_emcpair[ 0] = nclusadd;
          //--ntpval_emcpair[ 1] = evtseq;
          //--ntpval_emcpair[ 2] = clspair.p1_ang;
          //--ntpval_emcpair[ 3] = clspair.p2_ang;
          //--ntpval_emcpair[ 4] = clspair.p1_x;
          //--ntpval_emcpair[ 5] = clspair.p2_x;
          //--ntpval_emcpair[ 6] = clspair.p1_y;
          //--ntpval_emcpair[ 7] = clspair.p2_y;
          //--ntpval_emcpair[ 8] = clspair.p1_z;
          //--ntpval_emcpair[ 9] = clspair.p2_z;
          //--ntpval_emcpair[10] = clspair.dca_p0;
          //--ntpval_emcpair[11] = clspair.len_p0;
          //--ntpval_emcpair[12] = (vtx_sim[0]>-995) ? vtx_sim[0] : vertex[1][0]; //x-vertex
          //--ntpval_emcpair[13] = (vtx_sim[0]>-995) ? vtx_sim[1] : vertex[1][1]; //y-vertex
          //--ntpval_emcpair[14] = (vtx_sim[0]>-995) ? vtx_sim[2] : vertex[2][2]; //y-vertex
          //--ntpval_emcpair[15] = ephi;
          //--ntpval_emcpair[16] = recoCluster->get_z();
          //--ntpval_emcpair[17] = recoCluster->get_ecore();
          //--ntpval_emcpair[18] = recoCluster->getNTowers();
          //--ntpval_emcpair[19] = clspair.p1_the;
          //--ntpval_emcpair[20] = clspair.p2_the;
          //--ntpval_emcpair[21] = ethe;
          //--ntpval_emcpair[22] = clspair.prj_emc[0];
          //--ntpval_emcpair[23] = clspair.prj_emc[1];
          //--ntpval_emcpair[24] = clspair.prj_emc[2];
          //--ntpval_emcpair[25] = prj_phi;
          //--ntpval_emcpair[26] = prj_the;
          //--ntpval_emcpair[27] = clspair.px;
          //--ntpval_emcpair[28] = clspair.py;
          //--ntpval_emcpair[29] = clspair.pz;

          //--h_ntp_emccluspair->Fill(ntpval_emcpair);

          // look for closest cluster with the projection
          float dphi_abs = fabs(ephi - prj_phi);
          float dthe_abs = fabs(ethe - prj_the);
          float dphithe = sqrt(dphi_abs*dphi_abs + dthe_abs*dthe_abs*0.25); // dthe is weighted
          if(dphithe < small_dphithe) { small_dphithe = dphithe; closeCluster = recoCluster;}
          if(dphi_abs < small_dphi) { small_dphi = dphi_abs; closeCluster = recoCluster;}
          if(dthe_abs < small_dthe) { small_dthe = dthe_abs; }
        }
        { //fill with the closest EMC cluster
          Acts::Vector3 emcpos   = Acts::Vector3(closeCluster->get_x(), closeCluster->get_y(), closeCluster->get_z());
          Acts::Vector3 emcposc  = emcpos - beamspot;
          float ephi = atan2(emcposc.y(), emcposc.x());
          float er   = sqrt(emcposc.x()*emcposc.x() + emcposc.y()*emcposc.y());
          float ethe = atan2(er, emcposc.z()-vertex[2][2]);

          clspair.pos_emc[0] = emcpos.x();
          clspair.pos_emc[1] = emcpos.y();
          clspair.pos_emc[2] = emcpos.z();

          ////////////////////////////////////////////////
          // INTT-EMC fitting to update pT and pZ
          //calculateMomentum(clspair, emcpos, 2);
          
          ////////////////////////////////////////////////

          ntpval_emcpair[ 0] = nclusadd;
          ntpval_emcpair[ 1] = evtseq;
          ntpval_emcpair[ 2] = clspair.p1_ang;
          ntpval_emcpair[ 3] = clspair.p2_ang;
          ntpval_emcpair[ 4] = clspair.p1_x;
          ntpval_emcpair[ 5] = clspair.p2_x;
          ntpval_emcpair[ 6] = clspair.p1_y;
          ntpval_emcpair[ 7] = clspair.p2_y;
          ntpval_emcpair[ 8] = clspair.p1_z;
          ntpval_emcpair[ 9] = clspair.p2_z;
          ntpval_emcpair[10] = clspair.dca_p0;
          ntpval_emcpair[11] = clspair.len_p0;
          ntpval_emcpair[12] = (vtx_sim[0]>-995) ? vtx_sim[0] : vertex[1][0]; //x-vertex
          ntpval_emcpair[13] = (vtx_sim[0]>-995) ? vtx_sim[1] : vertex[1][1]; //y-vertex
          ntpval_emcpair[14] = (vtx_sim[0]>-995) ? vtx_sim[2] : vertex[2][2]; //y-vertex
          ntpval_emcpair[15] = ephi;
          ntpval_emcpair[16] = closeCluster->get_z();
          ntpval_emcpair[17] = closeCluster->get_ecore();
          ntpval_emcpair[18] = closeCluster->getNTowers();
          ntpval_emcpair[19] = clspair.p1_the;
          ntpval_emcpair[20] = clspair.p2_the;
          ntpval_emcpair[21] = ethe;
          ntpval_emcpair[22] = clspair.prj_emc[0];
          ntpval_emcpair[23] = clspair.prj_emc[1];
          ntpval_emcpair[24] = clspair.prj_emc[2];
          ntpval_emcpair[25] = prj_phi;
          ntpval_emcpair[26] = prj_the;
          ntpval_emcpair[27] = clspair.px;
          ntpval_emcpair[28] = clspair.py;
          ntpval_emcpair[29] = clspair.pz;

          h_ntp_emccluspair->Fill(ntpval_emcpair);
        }
      }
    }
      
    //  association to MVTX
    float ntpval_mcluspair[20];
    for(vector<cluspair>::iterator itrcp=vcluspair.begin(); itrcp!=vcluspair.end(); ++itrcp){
      cluspair& clspair = *itrcp;

      for (const auto &det : detectors)
      {
        for (const auto &layer : {2})
        {
          for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(det, layer))
          {
            auto range = m_clusterMap->getClusters(hitsetkey);
            for (auto citer = range.first; citer != range.second; ++citer)
            {
              const auto ckey    = citer->first;
              const auto cluster = citer->second;
              int mzsize   = cluster->getPhiSize();
              int mphisize = cluster->getZSize();
              if(mzsize<4 && mphisize<4) {
                const auto global = m_tGeometry->getGlobalPosition(ckey, cluster);

                Acts::Vector3 mpos   = Acts::Vector3(global.x(), global.y(), global.z());
                Acts::Vector3 mposc  = mpos - beamspot - mvtxoffset;
                float mphi = atan2(mposc.y(), mposc.x());
                float mr   = sqrt(mposc.x()*mposc.x() + mposc.y()*mposc.y());
                float mthe = atan2(mr, mposc.z()-zvertex);

                ntpval_mcluspair[ 0] = nclusadd; //evt;
                ntpval_mcluspair[ 1] = evtseq; //evtCount;
                ntpval_mcluspair[ 2] = clspair.p1_ang;
                ntpval_mcluspair[ 3] = clspair.p2_ang;
                ntpval_mcluspair[ 4] = clspair.p1_z;
                ntpval_mcluspair[ 5] = clspair.p2_z;
                ntpval_mcluspair[ 6] = clspair.dca_p0;
                ntpval_mcluspair[ 7] = clspair.len_p0;
                ntpval_mcluspair[ 8] = vertex[1][0]; //x-vertex
                ntpval_mcluspair[ 9] = vertex[1][1]; //y-vertex
                ntpval_mcluspair[10] = vertex[2][2]; //y-vertex
                ntpval_mcluspair[11] = mphi;
                ntpval_mcluspair[12] = global.z();
                ntpval_mcluspair[13] = mphisize;
                ntpval_mcluspair[14] = mzsize;
                ntpval_mcluspair[15] = clspair.p1_the;
                ntpval_mcluspair[16] = clspair.p2_the;
                ntpval_mcluspair[17] = mthe;

                h_ntp_mvtxcluspair->Fill(ntpval_mcluspair);
                //h_ntp_mvtxcluspair = new TNtuple("ntp_mvtxcluspair", "INTT and MVTX Cluster Pair Ntuple", "nclus:evt:ang1:ang2:z1:z2:dca2d:len:vx:vy:vz:m0ang:m0z:m0phisize:m0zsize:the1:the2:m0the");
              }
            }
          }
        }
      }
    }


    // tracklet pair analysis
    vector<cluspair> vcpairp, vcpairm;
    for(vector<cluspair>::iterator itrcp=vcluspair.begin(); itrcp!=vcluspair.end(); ++itrcp){
      cluspair& clspair = *itrcp;
      if(clspair.charge>0) vcpairp.push_back(clspair);
      else                 vcpairm.push_back(clspair);
    }

    const float Mpi = 0.139;
    float Mee, Px, Py, Pz, Pt, Thev, Ptep, Ptem, Phiv;

    float ntpval6[25];
    ////////////////////////////////////
    // calculate momentum for B-ON data
    for(vector<cluspair>::iterator itrcp=vcpairp.begin(); itrcp!=vcpairp.end(); ++itrcp){
      for(vector<cluspair>::iterator itrcm=vcpairm.begin(); itrcm!=vcpairm.end(); ++itrcm){
        cluspair& cpp = *itrcp;
        cluspair& cpm = *itrcm;

        calc_pair(cpp.px, cpp.py, cpp.py, Mpi,
                  cpm.px, cpm.py, cpm.py, Mpi,
                  Mee,  Px,   Py,   Pz,  Pt,
                  Thev, Ptep, Ptem, Phiv);


        ntpval5[ 0]  = Mee;
        ntpval5[ 1]  = Px;
        ntpval5[ 2]  = Py;
        ntpval5[ 3]  = Pz;
        ntpval5[ 4]  = Pt;
        ntpval5[ 5]  = cpp.px;
        ntpval5[ 6]  = cpp.py;
        ntpval5[ 7]  = cpp.pz;
        ntpval5[ 8]  = cpm.px;
        ntpval5[ 9]  = cpm.py;
        ntpval5[10]  = cpm.pz;

        h_ntp_trkpair->Fill(ntpval6);
      }
    }
  }
   
  // MVTX tracklet
  float ntpval_mvtxpair[20];
  for (ClustInfo& cluster0 : mvtxclusters[0])
  {
    Acts::Vector3& mpos0   = cluster0.pos;
    Acts::Vector3  mposc0  = mpos0 - beamspot - mvtxoffset;
    float mphi0           = atan2(mposc0.y(), mposc0.x());
    float mr0             = sqrt(mposc0.x()*mposc0.x() + mposc0.y()*mposc0.y());
    float mthe0           = atan2(mr0, mposc0.z()-zvertex);

    for (ClustInfo& cluster1 : mvtxclusters[1])
    {
          Acts::Vector3& mpos1   = cluster1.pos;
          Acts::Vector3  mposc1  = mpos1 - beamspot - mvtxoffset;
          float mphi1           = atan2(mposc1.y(), mposc1.x());
          float mr1             = sqrt(mposc1.x()*mposc1.x() + mposc1.y()*mposc1.y());
          float mthe1           = atan2(mr1, mposc1.z()-zvertex);

          ntpval_mvtxpair[ 0] = 0; //evt;
          ntpval_mvtxpair[ 1] = evtseq; //evtCount;
          ntpval_mvtxpair[ 2] = mphi0;
          ntpval_mvtxpair[ 3] = mphi1;
          ntpval_mvtxpair[ 4] = mpos0.z();
          ntpval_mvtxpair[ 5] = mpos1.z();
          ntpval_mvtxpair[ 6] = vertex[1][0];
          ntpval_mvtxpair[ 7] = vertex[1][1];
          ntpval_mvtxpair[ 8] = vertex[2][2];
          ntpval_mvtxpair[ 9] = mthe0;
          ntpval_mvtxpair[10] = mthe1;
          h_ntp_mvtxpair->Fill(ntpval_mvtxpair);
          //h_ntp_mvtxpair = new TNtuple("ntp_mvtxpair", "MVTX Cluster Pair Ntuple", "nclus:evt:mang0:mang1:mz0:mz1:vx:vy:vz:mthe0:mthe1");
    
    } 
  } 


  float ntpval3[40];
  ntpval3[ 0]  = nclusadd; //bco_full
  ntpval3[ 1]  = nclusadd2; //bco_full
  ntpval3[ 2]  = bco;
  ntpval3[ 3]  = evtseq;
  ntpval3[ 4]  = zvertex; //zvtx;
  ntpval3[ 5]  = zvtx; //zrms;
  ntpval3[ 6]  = 0; //zmean;
  ntpval3[ 7]  = 0; //zcenter;
  ntpval3[ 8]  = mbdz;
  ntpval3[ 9]  = mbdqn; //mbdt.bqn;
  ntpval3[10]  = mbdqs; //mbdt.bqs;
  ntpval3[11]  = 0; //mbdt.femclk;
  ntpval3[12]  = vertex[1][0]; //x-vertex
  ntpval3[13]  = vertex[1][1]; //y-vertex
  ntpval3[14]  = vertex[2][2]; //y-vertex
  ntpval3[15]  = nclus_inner; //x-vertex
  ntpval3[16]  = nclus_outer; //y-vertex

  ntpval3[17]  = (zvtxobj!=nullptr) ? zvtxobj->get_nclus() : 0;
  ntpval3[18]  = (zvtxobj!=nullptr) ? zvtxobj->get_ntracklet() : 0;
  ntpval3[19]  = (zvtxobj!=nullptr) ? zvtxobj->get_chi2ndf() : 0;
  ntpval3[20]  = (zvtxobj!=nullptr) ? zvtxobj->get_width() : 0;
  ntpval3[21]  = (zvtxobj!=nullptr) ? zvtxobj->get_ngroup() : 0;
  ntpval3[22]  = (zvtxobj!=nullptr && zvtxobj->get_good()) ? 1 : 0;
  ntpval3[23]  = (zvtxobj!=nullptr) ? zvtxobj->get_peakratio() : 0;

  ntpval3[24]  = vtx_sim[0]; //sim x-vertex
  ntpval3[25]  = vtx_sim[1]; //sim y-vertex
  ntpval3[26]  = vtx_sim[2]; //sim z-vertex

  ntpval3[27]  = (svtxvertex!=nullptr) ? svtxvertex->get_x() : -9999; // svtx vertex x
  ntpval3[28]  = (svtxvertex!=nullptr) ? svtxvertex->get_y() : -9999; // svtx vertex y
  ntpval3[29]  = (svtxvertex!=nullptr) ? svtxvertex->get_z() : -9999; // svtx vertex z

  ntpval3[30]  = nclusmvtx[0];
  ntpval3[31]  = nclusmvtx[1];
  ntpval3[32]  = nclusmvtx[2];
  ntpval3[33]  = (svtxtrackmap!=nullptr)?svtxtrackmap->size() : -9999; 
  ntpval3[34]  = nemc; 
  ntpval3[35]  = nemc1; 
  ntpval3[36]  = nemc2; 

  h_ntp_evt->Fill(ntpval3);
 // h_ntp_evt = new TNtuple("ntp_evt", "Event Ntuple", 
 //"nclus:nclus2:bco_full:evt:zv:
 //zvs:zvm:zvc:bz:bqn:
 //bqs:bfemclk:xvtx:yvtx:zvtx:
 //nclusin:nclusout:nclszv:ntrkzv:chi2ndfzv:
 //widthzv:ngroupzv:goodzv:peakratiozv:xvsim:
 //yvsim:zvsim");
 //
 
  h_t_evt_bco->Fill();

  m_evtbcoinfo_prev.copy(m_evtbcoinfo);

  evtCount++;

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttAna::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttAna analysis package" << std::endl;
  }

  if(anafile_!=nullptr){
    anafile_->Write();
    anafile_->Close();
  }

  return 0;
}

void InttAna::readRawHit(PHCompositeNode* topNode)
{
	TrkrHitSetContainer* m_hits = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
	if (!m_hits)
	{
		cout << PHWHERE << "ERROR: Can't find node TRKR_HITSET" << endl;
		return ;
	}


//uint8_t getLadderZId(TrkrDefs::hitsetkey key);
//uint8_t getLadderPhiId(TrkrDefs::hitsetkey key);
//int getTimeBucketId(TrkrDefs::hitsetkey key);
//
//uint16_t getCol(TrkrDefs::hitkey key); // z-strip = offline chip
//uint16_t getRow(TrkrDefs::hitkey key); // y-strip = offline channel

	// loop over the InttHitSet objects
	TrkrHitSetContainer::ConstRange hitsetrange =
		m_hits->getHitSets(TrkrDefs::TrkrId::inttId);
	for (TrkrHitSetContainer::ConstIterator hitsetitr = hitsetrange.first;
			hitsetitr != hitsetrange.second;
			++hitsetitr)
	{
		// Each hitset contains only hits that are clusterizable - i.e. belong to a single sensor
		TrkrHitSet *hitset = hitsetitr->second;

		if(Verbosity() > 1) cout << "InttClusterizer found hitsetkey " << hitsetitr->first << endl;
		if (Verbosity() > 2)
			hitset->identify();

		// we have a single hitset, get the info that identifies the sensor
		int layer          = TrkrDefs::getLayer(hitsetitr->first);
		int ladder_z_index = InttDefs::getLadderZId(hitsetitr->first);
                int ladder_phi_index = InttDefs::getLadderPhiId(hitsetitr->first);
                int ladder_bco_index = InttDefs::getTimeBucketId(hitsetitr->first);

                cout<<"layer "<<layer<<" "<<ladder_z_index<<" "<<ladder_phi_index<<" "<<ladder_bco_index<<endl;

		//// we will need the geometry object for this layer to get the global position	
		//CylinderGeomIntt* geom = dynamic_cast<CylinderGeomIntt*>(geom_container->GetLayerGeom(layer));
		//float pitch = geom->get_strip_y_spacing();
		//float length = geom->get_strip_z_spacing();

		// fill a vector of hits to make things easier - gets every hit in the hitset
		std::vector <std::pair< TrkrDefs::hitkey, TrkrHit*> > hitvec;
		TrkrHitSet::ConstRange hitrangei = hitset->getHits();
		for (TrkrHitSet::ConstIterator hitr = hitrangei.first;
				hitr != hitrangei.second;
				++hitr)
		{
			hitvec.push_back(make_pair(hitr->first, hitr->second));
			int chip = InttDefs::getCol(hitr->first);
			int chan = InttDefs::getRow(hitr->first);
			int adc  = (hitr->second)->getAdc();
			cout<<"     hit : "<<chip<<" "<<chan<<" "<<adc<<endl;
		}
		cout << "hitvec.size(): " << hitvec.size() << endl;
	}
}

void InttAna::calculateMomentum(cluspair& clspair, Acts::Vector3& pos, int posorder)
{
  vector<double> x{0.0, clspair.p1_x, clspair.p2_x}; // this should be posorder=0
  vector<double> y{0.0, clspair.p1_y, clspair.p2_y};

  if(posorder==2){
   x[0] = clspair.p1_x;
   x[1] = clspair.p2_x;
   x[2] = pos.x();
   y[0] = clspair.p1_y;
   y[1] = clspair.p2_y;
   y[2] = pos.y();
  }

  calculate2DMomentum(x, y, clspair.cx, clspair.cy, clspair.radius, clspair.px, clspair.py, clspair.charge);
  
  double pt = sqrt(clspair.px*clspair.px + clspair.py*clspair.py);
  calculatepZ(pt, clspair.slope_rz, clspair.pz);
}

void InttAna::calculate2DMomentum(std::vector<double>& x,std::vector<double>& y, 
                                double& cx, double& cy, double& radius,
                                double& px, double& py, int &charge)
{
  /////////////////////
  // calculate pT using 2 INTT hit and XY-vertex
  // 1: extract the center of circle and radius using 2INTT hit and XY-vertex
  //     x[3], y[3]: 3 position, x[0], y[0]: should be inner most position
  //     double cx, cy : circle center (cx, cy)
  //     double radius : radius of the circle
  //
  // 2: calculate pT using radius, and (cx, cy)
  //

  // 1: extract the center of circle and radius using 2INTT hit and XY-vertex
  //  hit is shifted relative to beam center already

  //////////////////
  // calculate radius and center from data
  // https://gleamath.com/uploads/circle-equation02.pdf
  //  
  //double px, py;
  double v10[2] = {x[0]-x[1], y[0]-y[1]};
  double v20[2] = {x[0]-x[2], y[0]-y[2]};
  //double v1d[2] = {px-x[1], py-y[1]};
  //double v2d[2] = {px-x[2], py-y[2]};
  
  double a = v10[0], b=v10[1], c=v20[0], d=v20[1];
  double A = -x[1]*x[0] -y[1]*y[0] +x[1]*x[1] + y[1]*y[1];
  double B = -x[2]*x[0] -y[2]*y[0] +x[2]*x[2] + y[2]*y[2];
  
  double x0b = (1./(a-(c*b/d))) * ((b/d)*B - A);
  double y0b = (1./b)*(-a*x0b - A);

  cx = (x0b+x[0])*0.5;
  cy = (y0b+y[0])*0.5;
  radius = sqrt((x0b-cx)*(x0b-cx) + (y0b-cy)*(y0b-cy));

  //////////////////////////////
  // 2: calculate pT and phi charge using radius, and (cx, cy)
  static const double Bf = 1.4; // [T]

  double pt  = 0.3*Bf*radius*0.01; // radius cm -> m

  // dir of the pt vector is perpendicular to C0
  double dir[2] = { cy, -cx};
  double dot    = dir[0]*x[1] + dir[1]*y[1]; 
  if(dot<0) {dir[0]*=-1; dir[1]*=-1; }
  double phi = atan2(dir[1], dir[0]);
 

  px = pt*cos(phi);
  py = pt*sin(phi);

  // 4: calculate charge using radius, and (cx, cy)
  // charge is direction of the curveture
  // vector CO (center to origin)
  // vector p1 (origin, inner hit)
  // cross prod. of vector_CO and vector
  double vCO[2] = {-cx, -cy};
  double cross = vCO[0]*y[1] - vCO[1]*x[1];

  charge = (cross<0) ? 1 :-1;
}

void InttAna::calculatepZ(const double &pt, const double& slope_rz, double& pz)
{
  //////////////////////////////
  // 2: calculate pz using a slope in r-z plane
  pz = pt * slope_rz; // slope_rz by fitting straight line
}


void InttAna::linefitXYplane(cluspair& clspair, Acts::Vector3& /*beamspot*/)
{
  //double beamspot_r = sqrt(beamspot.x()*beamspot.x() + beamspot.y()*beamspot.y());
  vector<double> vx{0., clspair.p1_x, clspair.p2_x};
  vector<double> vy{0., clspair.p1_y, clspair.p2_y};
  vector<double> vy_e{0.01, 0.02, 0.02};

  clspair.chi2_xy = linefit(vx, vy, vy_e, clspair.slope_xy, clspair.offset_xy);
  //cout<<"xy chi2: "<<clspair.chi2_xy<<endl;
}

void InttAna::linefitRZplane(cluspair& clspair, Acts::Vector3& beamspot)
{
  //double beamspot_r = sqrt(beamspot.x()*beamspot.x() + beamspot.y()*beamspot.y());
  vector<double> vr{0., clspair.p1_r, clspair.p2_r};
  vector<double> vz{beamspot.z(), clspair.p1_z, clspair.p2_z};
  vector<double> vz_e{0.5, 0.8, 0.8};

  clspair.chi2_rz = linefit(vr, vz, vz_e, clspair.slope_rz, clspair.offset_rz);
  //cout<<"rz chi2: "<<clspair.chi2_rz<<endl;
}

double InttAna::linefit(vector<double>& vx, vector<double>& vy, vector<double>& vey, double& slope, double& offset)
{
  double A=0, B=0, C=0, D=0, E=0;
  for(uint i=0; i<vx.size(); i++){
    A += ((vx[i]*vx[i]) / (vey[i]*vey[i]));
    B += ((vx[i])       / (vey[i]*vey[i]));
    C += ((vx[i]*vy[i]) / (vey[i]*vey[i]));
    D += (1             / (vey[i]*vey[i]));
    E += (vy[i]         / (vey[i]*vey[i]));
  }

  double det = (A*D) - (B*B);

  slope = 0; 
  offset = 0; 

  double chi2=0;
  if(fabs(det)>0.00000001){ // det==0
    slope  = (1./det)*(D*C - B*E);
    offset = (1./det)*(A*E - B*C);

    for(uint i=0; i<vx.size(); i++){
      double exp = slope * vx[i] + offset;
      double diff = vy[i]-exp;
      chi2 += (diff*diff)/(vey[i]*vey[i]);
    }
  }

  return chi2;
}

void InttAna::projectTrack(cluspair& clspair, Acts::Vector3& beamspot, const double& targetR, 
                        double& prjx, double& prjy, double& prjz)
{
   projectToXY(clspair, beamspot, targetR, prjx, prjy);
   projectToRZ(clspair, beamspot, targetR, prjz);
}

void InttAna::projectToXY(cluspair& clspair, Acts::Vector3& /*beamspot*/, const double& targetR, 
                          double &prjx, double& prjy)
{
  /////////////////////////
  // clspair variables
  //  double cx, cy, radius;
  //  double px, py, pz, pt;
  //  double phi, theta;
  //  double charge;
  //
 
  // beam center is assumed to (0,0) since the cluster positions are shifted 
  ///////////////////////
  double& px  =  clspair.px;
  double& py  =  clspair.py;
  

  
  double& track_radius = clspair.radius;
  double  track_c[2] = {clspair.cx,clspair.cy};

  double L = sqrt(clspair.cx*clspair.cx + clspair.cy*clspair.cy);
  
  double prj[2]{0};
  if ((L+track_radius)< targetR){
    cout<<" no overlap"<<endl;
  }
  else {
    ///////////////////////
    double a = track_c[0], b=track_c[1];
    double r = targetR, R=track_radius;
    double A = R*R - r*r - a*a - b*b;
    double B = (fabs(b)<0.0000001) ? 0 : -a/b;
    double C = (fabs(b)<0.0000001) ? 0 : -1./(2*b);
    double CA = C*A;
    
    double dir = 1;
    
    if(fabs(b)<0.0000001){
      prj[0] = -A/(2*a);
      prj[1] = sqrt(r*r - prj[0]*prj[0]);
    }
    else {
      double par = sqrt((B*B*CA*CA) - (1.+B*B)*(CA*CA - r*r));
      prj[0] = (-B*CA + par)/(1.+B*B);
      prj[1] = B*prj[0] + CA;
    
      dir = px*prj[0]+py*prj[1];
      if(dir<0){
        prj[0] = (-B*CA - par)/(1.+B*B);
        prj[1] = B*prj[0] + CA;
      }
    }
    
    cout<<"x : y = "<<prj[0]<<" "<<prj[1]<<endl;
    
  }
  

  prjx = prj[0];
  prjy = prj[1];
  /////////
}

void InttAna::projectToRZ(cluspair& clspair, Acts::Vector3& beamspot, const double& targetR, 
                          double& prjz)
{

  double slope  = clspair.pz/clspair.pt;
  double offset = beamspot.z();
  double beam_r = sqrt(beamspot.x()*beamspot.x() + beamspot.y()*beamspot.y());
 

  prjz = slope*(targetR-beam_r)+offset;
}

float InttAna::calc_pair(
                float pxp, float pyp, float pzp, float Mp,
		float pxm, float pym, float pzm, float Mm,
		float& Mee,  float& px,   float& py,   float& pz, float& pt,
		float& thev, float& ptep, float& ptem, float& phiv)
{
  //float pxep = pxp;
  //float pyep = pyp;
  //float pzep = pzp;
  float momp = sqrt(pxp*pxp+pyp*pyp+pzp*pzp);
  float Ep  = sqrt(momp*momp+Mp*Mp);
  
  //float pxem = pxm;
  //float pyem = pym;
  //float pzem = pzm;
  float momm = sqrt(pxm*pxm+pym*pym+pzm*pzm);
  float Em   = sqrt(momm*momm+Mm*Mm);
  
  px = pxp + pxm;
  py = pyp + pym;
  pz = pzp + pzm;
  float E  = Ep + Em;
  
  pt = sqrt(px*px + py*py);
  Mee = sqrt(E*E - (px*px+py*py+pz*pz));
  
  //////////////
  //unit vector of (pep+pem)
  float pl = sqrt(px*px+py*py+pz*pz);
  float ux = px/pl;
  float uy = py/pl;
  float uz = pz/pl;
  
  //axis defined by (ux,uy,ux)X(0,0,1).
  // this is the axis that is perpendicular to the direction of
  // pair, and also perpendicular to the Z axis (field direction).
  // If the pair is conversion at R!=0, it must have (apparent)
  // momentum component in this axis (caused by field intergral from the
  // vertex point to the conversion point).
  // The sign of the component is opposite for e+ and e-.
  //
  // (ux,uy,ux)X(0,0,1)=(uy,-ux,0)
  //
  //cout<<sqrt(ux*ux+uy*uy)<<endl;
  float ax =  uy/sqrt(ux*ux+uy*uy);
  float ay = -ux/sqrt(ux*ux+uy*uy);
  
  //momentum of e+ and e- in (ax,ay,az) axis. Note that az=0 by
  //definition.
  ptep = pxp*ax + pyp*ay;
  ptem = pxm*ax + pym*ay;
  
  //vector product of pep X pem
  float vpx = pyp*pzm - pzp*pym;
  float vpy = pzp*pxm - pxp*pzm;
  float vpz = pxp*pym - pyp*pxm;
  float vp = sqrt(vpx*vpx+vpy*vpy+vpz*vpz);
  thev = acos(vpz/vp);
  
  //unit vector of pep X pem
  float vx = vpx/vp;
  float vy = vpy/vp;
  float vz = vpz/vp;
  
  //The third axis defined by vector product (ux,uy,uz)X(vx,vy,vz)
  float wx = uy*vz - uz*vy;
  float wy = uz*vx - ux*vz;
  float wz = ux*vy - uy*vx;
  float wl = sqrt(wx*wx+wy*wy+wz*wz);
  // by construction, (wx,wy,wz) must be a unit vector.
  if(fabs(wl - 1.0) > 0.00001) cout << "Calculation error in W vector"<<endl;
  
  // measure angle between (wx,wy,wz) and (ax,ay,0). The angle between them
  // should be small if the pair is conversion
  //
  float cosPhiV = wx*ax + wy*ay;
  phiv = acos(cosPhiV);
  
  return Mee;
}
  
