#ifndef INTTQVECTOR_H__
#define INTTQVECTOR_H__

#include <fun4all/SubsysReco.h>
#include <trackbase/TrkrDefs.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <intt/CylinderGeomIntt.h>

class PHCompositeNode;
class ActsGeometry;
class TrkrClusterContainer;
class SvtxTrackMap;
class GlobalVertex;
class PHG4CylinderGeomComtainer;
class CylinderGeomIntt;

class TFile;
class TTree;
class TH1F;
class TH2S;
class TCanvas;

class InttOfflineClusterList;

class InttQvector : public SubsysReco
{
 public :
  //Coustructor
  InttQvector(const std::string &name = "InttQvector",
               const std::string &fname = "InttQvector.root");
    
  //Destructor
  ~InttQvector();

   /// SubsysReco initialize processing method
  int Init(PHCompositeNode * topNode);
  int InitRun(PHCompositeNode *topNode);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *topNode);

  int Reset(PHCompositeNode *topNode);
  int ResetEvent(PHCompositeNode *topNode);

  /// SubsysReco end processing method
  int End(PHCompositeNode *topNode);
  int EndRun(const int runnumber);

  /// define nodes needed
  int NodeInitialize(PHCompositeNode *topNode);

  // calcurate qx, qy, nclus, qvec = (qx,qy,nclus)
  void cal_q(double qvec[]);

 private:
  ActsGeometry *m_tGeometry = nullptr;
  TrkrClusterContainer *m_clusterMap = nullptr;
  PHG4CylinderGeomContainer *geom_container = nullptr;
  CylinderGeomIntt *geom = nullptr;

  std::string fname_;
  TFile* anafile_;
  TTree* tree1; 
  TTree* tree2;
  TH1F* psidis;
  TH1F* adcdis;
  TH2S* qvecdis;

  TCanvas*c1;

  const static unsigned int m_nInttLayers = 4; //intt has 4 layer
  double qx,qy;
  double psi;
  double nclus;

  InttOfflineClusterList* inttOfflineCls_;
};

#endif
