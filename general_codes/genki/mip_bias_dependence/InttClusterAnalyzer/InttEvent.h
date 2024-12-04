#pragma once

#include <string>
#include <vector>

#include <TVector3.h>

#include "InttCluster.h"

class InttEvent
{
private:
  int run_ = -1;
  int event_num_ = -1;
  int cluster_num_ = -1;
  int cluster_num_inner_ = -1;
  int cluster_num_outer_ = -1;

  bool is_vertex_good_ = false;
  double z_chi2_ndf_ = -1;  
  TVector3 position_vertex_;
  
  std::vector < InttCluster* > clusters_;

  int verbosity_ = 0;
  void Init();
  
public:
  InttEvent();
  InttEvent( const InttEvent& event )
  {
    run_ = event.run_ ;
    event_num_ = event.event_num_ ;
    cluster_num_ = event.cluster_num_ ;
    cluster_num_inner_ = event.cluster_num_inner_ ;
    cluster_num_outer_ = event.cluster_num_outer_ ;
    position_vertex_ = event.position_vertex_;
    clusters_ = event.clusters_;
    verbosity_ = event.verbosity_ ;

  };
  // operator=(InttEvent& )
  // {


  // }
  
  void AddCluster( InttCluster* cluster ){ clusters_.push_back( cluster ); };

  void Calc(); // run this before use
  void Reset();
  
  ////////////////////////////////////////////////////////
  // Get functions
  ////////////////////////////////////////////////////////
  int GetRun (){ return run_ ; };
  int GetEventNum (){ return event_num_ ; };
  int GetClusterNum (){ return cluster_num_ ; };
  int GetClusterNumInner (){ return cluster_num_inner_ ; };
  int GetClusterNumOuter (){ return cluster_num_outer_ ; };
  double GetVertexZChi2Ndf  (){ return z_chi2_ndf_; };
  bool GetWhetherVertexGood(){ return is_vertex_good_;};
  
  TVector3 GetPositionVertex(){ return position_vertex_; };
  
  std::vector < InttCluster* >& GetClusters(){ return  clusters_; };
  
  ////////////////////////////////////////////////////////
  // Set functions
  ////////////////////////////////////////////////////////
  void SetRunNum( int num ){ run_ = num; };
  void SetEventId( int num ){ event_num_ = num; };
  void SetVertex( double x, double y, double z );
  void SetVertexZ( double z );
  void SetVertexZChi2Ndf( double value ){ z_chi2_ndf_ = value;};
  void SetVerbosity( int level ){ verbosity_ = level;};
  void SetWhetherVertexGood( bool flag ){ is_vertex_good_ = flag; };
  
  void Print();
};
