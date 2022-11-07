#pragma once

#ifdef __linux__
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh" // for ?

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

/*!
  @file INTTEvent.hh
  @brief Class to treat INTT event, which consists of some INTT hits
  @details 

 */

#include <vector>
#include <iostream>
#include <iomanip>

#include "functions.hh"

class INTTEvent
{
public:
  INTTEvent(){};

  /*! This constructor takes parameters of the event. They are made when clustering. */
  INTTEvent( vector < int > *nhit_in_cluster_arg,
	     vector < int > *is_over_chip_arg,
	     vector < int > *module_arg,
	     vector < double > *chip_id_arg,
	     vector < double > *chan_id_arg,
	     vector < double > *chan_id_simple_arg,
	     vector < double > *chan_sigma_arg,
	     vector < double > *chan_sigma_simple_arg,
	     vector < int > *chan_id_min_arg,
	     vector < int > *chan_id_max_arg,
	     vector < double > *dac_arg
	     );


  bool DoTracking();

  double GetFocusingAlignmentOffset(){ return focusing_alignment_offset_;};
  double GetAlignmentOffset( int i ){ return alignment_offset_[i];};
  int GetHitNum( int ladder );
  string GetTrackingMode(){ return mode_;};
  TF1* GetTrackingFunction(){ return tracking_;};
  int GetHitIndexForTrackingUpstream(){ return index_upstream_ladder_for_tracking_;};
  int GetHitIndexForTrackingDownstream(){ return index_downstream_ladder_for_tracking_;};

  int GetHitIndexClosestToTracking(){ return index_closest_to_tracking_;};
  double GetChannelFromTracking(){ return tracked_position_;};

  bool IsGoodToAnalyze(); // not used somehow

  //! If the given ladder has only one cluster, it returns true
  bool IsSingleHit( int ladder ){ return (this->GetHitNum(ladder)==1);};
  //! if the given laader has only one cluster, and the cluster is on the focusing (testing) chip, it returns true
  bool IsOnlyFocusingChip( int ladder );

  //! The predicted hit position by tracking is returned
  double GetTrackedChannel( int ladder ){ return tracked_position_;};
  double GetTrackedChip( int ladder ){ return -999;}; // not used for the moment
  //! A slope of the track is returned
  double GetTrackSlope(){ return tracking_->GetParameter(1);};
  //! A intercept of the track is returned
  double GetTrackIntercept(){ return tracking_->GetParameter(0);};
  
  void SetDebugMode( bool arg );

  //! You can choose "simplest" (geometrical method) or "fitting" (fitting method)
  void SetTrackingMode( string mode ){ mode_ = mode;};

  //! A ladder to be tested can be set
  void SetFocusingLadder( int ladder_arg ){ focusing_ladder_ = ladder_arg;};
  //! A chip to be tested can be set
  void SetFocusingChip( int chip_arg ){ focusing_chip_ = chip_arg;};
  void SetFocusingChan( int chan_arg ){ focusing_chan_ = chan_arg;}; // not used
  //! The offset from the alignment analysis for the tested ladder can be set.
  void SetFocusingAlignmentOffset( double offset ){ focusing_alignment_offset_ = offset;};
  //! The offset from the alignment analysis can be set. This value is subtracted from a hit channel for alignment correction
  void SetAlignmentOffset( int ladder, double offset ){ alignment_offset_[ladder] = offset;};

  //! Alignment results are in a ROOT file. Results are taken from the file and assigned to the 2D array map. To save reading time, this function is static so that you can just set only once in the beginning of the analysis.
  static void SetAlignmentMap( string data );
  //! usuful function to convert a ladder number to the module number, but why in this class?
  static int Ladder2Module( int ladder )
  {
    if( ladder == 0 )
      return 1;
    else if( ladder == 1 )
      return 6;
    else if( ladder == 2 )
      return 5;
    
    return -1;
  }

  //! usuful function to convert the module number to the ladder number, but why in this class?
  static int Module2Ladder( int module )
  {
    if( module == 1 )
      return 0;
    else if ( module == 6 )
      return 1;
    else if( module ==5 )
      return 2;
    
    return -1;
  }

  //! Functions to print status
  void PrintEvent( int index = -1 );
  void PrintCondition();
  void Print();
  
private:

  // A trick to suppress usuless std out when analyzing all data
#ifndef DEBUG
  stringstream cout;
#else
  //  using namespace std;
#endif
  
  // parametes of hits in an event
  const vector < int > nhit_in_cluster_; // the number of hits in a cluster
  const vector < int > is_over_chip_; // a flag to know whether this hit cluster is over chip (127-128) or not
  const vector < int > module_; 
  const vector < double > chip_id_;
  const vector < double > chan_id_;
  const vector < double > chan_id_simple_; // not weighted channel of a cluster
  const vector < double > chan_sigma_; // std. dev. of hit channels of a cluster
  const vector < double > chan_sigma_simple_; 
  const vector < int > chan_id_min_; // the largest hit channel of a cluster
  const vector < int > chan_id_max_; // the smallest hit channel of a cluster
  const vector < double > dac_; // DAC channel of a cluster
  
  int focusing_ladder_ = -1; // ID of the tested ladder. It can be 0, 1, and 2.
  int focusing_chip_ = -1; // The number of the tested chip.
  int focusing_chan_ = -1; // not used

  int index_upstream_ladder_for_tracking_ = -1; // ID of the upstream testing ladder
  int index_downstream_ladder_for_tracking_ = -1; // ID of the downstream testing ladder
  int index_closest_to_tracking_ = -1; // Channel of the closest cluster to the predicted position by tracking
  double tracked_position_ = -1; // The predicted hit position by tracking

  static double alignment_map_[3][13]; // 2D map of the alignmet offset, It needs to be static to asign values from static functions
  vector < double > alignment_offset_; // List of offset from the alignment analysis
  double focusing_alignment_offset_ = -1; // The offset of the tested ladder (for backword compatibility)

  TF1* tracking_; // the track
  double tracking_slope_ = -1; // a slope of the track
  double tracking_intercept_ = -1; // an intercept of the track
  
  bool debug_ = false; // a flag for the debud mode
  string mode_ = "simplest"; // type of tracking
  string fitting_option_ = "CQ"; // fitting option
   
  void Init(); // do nothing actually

  //! the geometrical tracking using the arguments, the arguments are the hit index
  void GetTrackSimplest( vector < int >& index );

  //! the fitting tracking using the arguments, the first argument contains hit index of the testing ladder, the second argument
  void GetTrackFitting( vector < int >& index_testing, vector < int >& index_tested );

  //! It searches the closest hit to the predicted hit position, To get the result, use INTTEvent::GetHitIndexClosestToTracking()
  void SearchClosestHit();

  //! It selects events, do tracking with the geometrical method
  bool TrackingSimplest();

  //! It selects events, do tracking with the fitting method
  bool TrackingFitting();

};

// It's my trick to include the source code with ROOT interpreter. Just define __CINT__ to use this class with ROOT interpreter, otherwise compile it
#ifdef __CINT__
#include "INTTEvent.cc"
#endif
