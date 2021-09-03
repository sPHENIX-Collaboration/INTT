#pragma once

class INTTHit
{
public:
  int adc_	= -1;
  Double_t adc_voltage_ = -1.0;
  int ampl_	= -1;
  int chip_id_	= -1;
  int fpga_id_	= -1;
  int module_	= -1;
  int chan_id_	= -1;
  int fem_id_	= -1;
  int bco_	= -1;
  int bco_full_ = -1;
  int event_	= -1;

  int dac_values_[8] = {0};

  bool forced_ignored_ = false;
  string color_red_ = "\033[31m";
  string color_cyan_ = "\033[36m";
  string color_cancel_ = "\033[m";

  vector < pair < int, int > > cluster_geometry_; // vector of pair of module-channel
  vector < int > cluster_channels_;
  vector < Double_t > cluster_adc_voltages_;
  //vector < 


  // List of status
  //    -2: noise
  //    -1: cluster (included by another hit),
  //     0: just a hit,
  //     1: cluster (including another hits),
  // Positive status should be used.
  int clustering_status_ = 0; 
  
  INTTHit(){};
  INTTHit( int adc, int ampl, int chip_id,
	   int fpga_id, int module, int chan_id,
	   int fem_id, int bco, int bco_full, int event );
  
  INTTHit( int adc, int ampl, int chip_id,
	   int fpga_id, int module, int chan_id,
	   int fem_id, int bco, int bco_full, int event,
	   int dac[8]);
  
  //INTTHit( const INTTHit& obj );

  void Init();
  
  bool IsIgnored();
  bool IsCluster( INTTHit* another );
  void Clustering( INTTHit* another );
  INTTHit* MakeCluster( INTTHit* another );

  Double_t GetClusterADCVoltage(){ return accumulate( cluster_adc_voltages_.begin(), cluster_adc_voltages_.end(), 0.0); };;
  int GetClusteringStatus(){ return clustering_status_; };
  //double GetCluster
  
  void SetClusteringStatus( int status );
  void SetDAC( int id, int value );
  void SetAllDAC( int values[8] );
  void SetIgnored( bool status );
  
  void Print();
  void PrintInOneLine();
};

#ifndef INTTHIT_source
#define INTTHIT_source

#include "INTTHit.cc"
#endif //  INTTHIT_source
