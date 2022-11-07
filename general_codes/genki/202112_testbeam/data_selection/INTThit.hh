#pragma once

/*!
  @file make_cluster.cc
  @brief INTT hits are merged into cluesters and written to a ROOT file
  @details 

  # Data structure
  - camac_adc: no change
  - camac_tdc: no change
  - pulse_height: calculated from ADC value of hits using the DAC configuration
  - ladder: order of the ladders from the upstream to the downstream (0, 1, 2 ) using module
  - chip_col: chip column calculated from chip_id
  - chan_col: silicon strip channel of a chip column calculated with chan_id
  - bco: no change
  - bco_full: no change
  - bco_group_id: order of a BCO group in a trigger event
  - nhit: the number of hits forming this cluster
  - chan_center: weighted channel of this cluster by ADC values of hits
  - chan_sigma: standard deviation of the channel of hits
  - chan_range: Max( ch ) - Min ( ch )
  - over_chip: a flag to know whether this cluster was formed over silicon chips or not
  - 
 */

#include <vector>
#include <iostream>
#include <iomanip>

#include "../Database.hh"
#include "../lib/functions.hh"

class INTThit
{
public:

  // some more parameters
  INTThit(){};
  
  INTThit( int adc      , int ampl     , int chip_id  , int fpga_id  ,
	   int module   ,  int chan_id  , int fem_id   , int bco      ,
	   int bco_full , int event, vector < int > dac_config );

  bool IsNoise();
  bool IsAlreadyAdded() const { return is_already_added_;};
  void AddForClustering( INTThit* hit );
  bool Clustering();
  int  GetHitNumInCluster() const { return hits_added_.size() + 1;};
  
  int GetFemId()	const { return fem_id_;};
  int GetFpgaId()	const { return fpga_id_;};
  int GetModule()	const { return module_;};
  int GetChipId()	const { return chip_id_;};
  int GetChanId()	const { return chan_id_;};
  int GetAdc()		const { return adc_;};
  int GetDac()		const { return dac_;};
  int GetAmpl()		const { return ampl_;};
  int GetBco()		const { return bco_;};
  int GetBcoFull()	const { return bco_full_;};
  //  int event = -1;

  // new parameters
  int GetLadder()			const { return ladder_;};
  int GetPulseHeight()			const { return pulse_height_;};
  int GetChipCol()			const { return chip_col_;};
  int GetChanCol()			const { return chan_col_;};
  int GetNhit()				const { return nhit_;};
  int GetNhitWithAdc( int adc ) const;
  int GetDacCluster()			const { return dac_cluster_;};
  double GetChanIdCluster()		const { return chan_cluster_;};
  double GetChanIdClusterSimple()	const { return chan_cluster_simple_;};
  int GetChanIdMinCluster()             const { return min_chan_cluster_;};
  int GetChanIdMaxCluster()             const { return max_chan_cluster_;};
  double GetChanIdClusterSigma()	const { return chan_cluster_sigma_;};
  double GetChanIdClusterSigmaSimple()	const { return chan_cluster_sigma_simple_;};
  bool IsOverChip()			const { return is_over_chip_;};

  void Print( int verbosity = 0, bool is_header = false );

private:
  // original parameters
  int fem_id_	= -1;
  int fpga_id_	= -1;
  int module_	= -1;
  int chip_id_	= -1;
  int chan_id_	= -1;
  int adc_	= -1;
  int dac_	= -1;
  int ampl_	= -1;
  int bco_	= -1;
  int bco_full_ = -1;

  // new parameters
  int ladder_		= -1;
  int pulse_height_	= -1; // in mV, calculated from DAC value
  int chip_col_		= -1; // chip column ID
  int chan_col_		= -1; // channel ID of chip column
  int nhit_		= -1; // number of hit that form is cluster

  // parameters for cluster
  int dac_cluster_			= -1;

  int max_chip_cluster_			= -1;
  int min_chip_cluster_			= -1;

  double chan_cluster_			= -1; // weighting with DAC values
  double chan_cluster_simple_		= -1; // no weighting with DAC values
  int max_chan_cluster_			= -1;
  int min_chan_cluster_			= -1;

  double chan_cluster_sigma_		= -1; // std. dev. of channels
  double chan_cluster_sigma_simple_	= -1; // std. dev. of channels without weighting with DAC values

  bool is_over_chip_	 = false;
  bool is_already_added_ = false;

  vector < INTThit* > hits_added_;
  
  void Init();
  bool CanBeAdded( INTThit* hit );
};


#ifdef __CINT__
#include "INTThit.cc"
#endif
