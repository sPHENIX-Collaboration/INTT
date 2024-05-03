// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

#include <string>
#include <iomanip>
#include <algorithm>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <trackbase/InttEventInfo.h>

#include <trackbase/InttEventInfov1.h>
#include <trackbase/TrkrHitSetContainerv1.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrClusterHitAssocv3.h>
#include <trackbase/TrkrClusterCrossingAssocv1.h>
#include <trackbase/ActsGeometry.h>

#include <ffaobjects/FlagSavev1.h>

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

using namespace std;

class PHCompositeNode;

class AnalysisInttCosmicCommissioning : public SubsysReco
{
private:

  // variables
  int run_num_ = 0;
  string data_;
  string output_path_ = "./";
  string output_name_ = "output.root";
  string output_pdf_ = "";
  int misc_counter_ = 0;
  
  // objects
  TCanvas* c_;
  TFile* outFile_;
  TTree* outTree_; 
  // Branches for TTree
  int n_cluster_;
  uint64_t bco_;
  std::vector<double> posX_;
  std::vector<double> posY_;
  std::vector<double> posZ_;
  std::vector<double> radius_;
  std::vector<int> cluster_size_; 
  std::vector<int> adc_;

  //TF1 Parameters 
  double slope_xy_;
  double constant_xy_;
  double chi2_xy_;
  double ndf_xy_;
  double chi2ndf_xy_;
  double average_dist_xy_;
  double slope_yz_;
  double constant_yz_;
  double chi2_yz_;
  double ndf_yz_;
  double chi2ndf_yz_;
  double average_dist_yz_;




  // nodes
  InttEventInfo* node_intteventheader_map_;
  InttRawHitContainer* node_inttrawhit_map_;
  TrkrClusterContainerv4* node_cluster_map_;
  ActsGeometry* node_acts_;

  // private functions
  int GetNodes(PHCompositeNode *topNode);
  double GetDistance( const Acts::Vector3 a, const Acts::Vector3 b, bool use_x=true, bool user_y=true, bool use_z=true); // general function
  double GetDistanceXY( const Acts::Vector3 a, const Acts::Vector3 b ){ return GetDistance( a, b, true, true, false); }; // distance in X-Y place

  vector < pair < TrkrCluster*, const Acts::Vector3 > > GetClusters();
  
public:

  AnalysisInttCosmicCommissioning(const std::string &name = "AnalysisInttCosmicCommissioning",const std::string &output_name = "output.root");

  ~AnalysisInttCosmicCommissioning() override;

  //////////////////////////////////////////////////////////////////
  // functions for SubsysReco //////////////////////////////////////
  //////////////////////////////////////////////////////////////////

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  //////////////////////////////////////////////////////////////////
  // other functions ///////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////
  void SetData( string path );
  void SetOutputPath( string path ){ output_path_ = path;};

};
