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
#include "TH1F.h"
#include "TEllipse.h"
#include "Math/Vector3D.h"

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
  //int run_num_ = 0;
  //  string data_ = "";
  string output_path_ = "./";
  string output_name_ = "output.root";
  string output_pdf_ = "";
  int misc_counter_ = 0;
  
  // objects
  TCanvas* c_;
  TFile* outFile_;
  TTree* outTree_;
  
  // Branches for TTree

  // event information
  int n_cluster_;
  uint64_t bco_;

  // hit (cluster) information
  std::vector<double> posX_;
  std::vector<double> posY_;
  std::vector<double> posZ_;
  std::vector < ROOT::Math::XYZVector > positions_;
  
  std::vector<double> radius_;
  std::vector<int> cluster_size_; 
  std::vector<int> adc_;

  // coordinate numbers: (x,y), (z,y), (z,x)
  vector < pair < int, int > > vcoordinates_;
  
  // TGraphs
  TGraph* graphs_[3]; // 0: xy, 1: zy, 2: zx
  
  // TF1
  TF1* lines_[3]; // straight line to be fitted to all clusters
  TF1* lines_best[3]; // straight lines with the best chi2/NDF from fitting to possible cluster combinations
  
  //TF1 Parameters
  double slopes_[3];
  double constants_[3];
  double chi2ndfs_[3];
  double average_distances_[3];
  
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

  // misc
  const double kLayer_radii[4] = {7.1888, 7.800, 9.680, 10.330 };
  
  // nodes
  InttEventInfo*          node_intteventheader_map_;
  InttRawHitContainer*    node_inttrawhit_map_;
  TrkrClusterContainerv4* node_cluster_map_;
  ActsGeometry*           node_acts_;

  // private functions
  void DrawIntt( double xmax, double ymax );
  int Fit();
  vector < pair < TrkrCluster*, const Acts::Vector3 > > GetClusters();
  double GetDistance( const Acts::Vector3 a, const Acts::Vector3 b, bool use_x=true, bool user_y=true, bool use_z=true); // general function
  double GetDistanceXY( const Acts::Vector3 a, const Acts::Vector3 b ){ return GetDistance( a, b, true, true, false); }; // distance in X-Y place
  int GetNodes(PHCompositeNode *topNode);
  
  std::string Int2Coordinate( int num );
  int MakeGraphs( vector < pair < TrkrCluster*, const Acts::Vector3 > >& cluster_pos_pairs );
  int ProcessEventRawHit();
    
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
  std::string GetOutput(){ return output_name_;};
  std::string GetOutputPdf(){ return output_pdf_;};
  // void SetData( string path = "" );
  // void SetOutputPath( string path );

};
