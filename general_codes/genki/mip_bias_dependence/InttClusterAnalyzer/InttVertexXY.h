// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTVERTEXXY_H
#define INTTVERTEXXY_H

#include <string>
#include <fstream>

#include <TFile.h>
#include <TTree.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>

#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/InttEventInfov1.h>

//#include <intt/InttVertexv1.h>
#include <intt/InttVertexMapv1.h>

#include "InttCluster.h"
#include "InttEvent.h"

class PHCompositeNode;

class InttVertexXY : public SubsysReco
{
 public:

  InttVertexXY(const std::string &name = "InttVertexXY");

  ~InttVertexXY() override;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;
  void Print(const std::string &what = "ALL") const override;

  void SetRunNum( int num ){ run_num_ = num; };
  // void SetOutput( std::string arg ){ output_ = arg; };
  // void SetOutputPath( std::string arg ){ output_dir_ = arg; };
  // void SetOutputName( std::string arg ){ output_name_ = arg; };
  
 private:
  int run_num_ = -1;
  int event_counter_ = 0;
  double vertex_x_ = -9999;
  double vertex_y_ = -9999;
  
  std::string output_dir_ = "./results/vertex_xy/";
  std::string output_name_ = "output.root";
  std::string output_ = "";
  std::string output_txt_ = "";
  
  TFile* tf_;
  TTree* tr_;

  InttVertexMapv1*        node_intt_vertex_map_;
  
  int GetNode( PHCompositeNode *topNode );
  int VertexAnalysis( PHCompositeNode *topNode );
};

#endif // INTTCLUSTERANALYZER_H
