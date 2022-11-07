#include "INTTEvent.hh"

double INTTEvent::alignment_map_[3][13] = {}; // 2D map of the alignmet offset

// constructor
INTTEvent::INTTEvent( vector < int > *nhit_in_cluster_arg,
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
		      ) :
  nhit_in_cluster_( *nhit_in_cluster_arg ),
  is_over_chip_( *is_over_chip_arg ),
  module_( *module_arg ),
  chip_id_( *chip_id_arg ),
  chan_id_( *chan_id_arg ),
  chan_id_simple_( *chan_id_simple_arg ),
  chan_sigma_( *chan_sigma_arg ),
  chan_sigma_simple_( *chan_sigma_simple_arg ),
  chan_id_min_( *chan_id_min_arg ),
  chan_id_max_( *chan_id_max_arg ),
  dac_( *dac_arg ),
  alignment_offset_( 3 )
{
  
  Init(); // actually do nothing

}

/**
   @brief Users can do tracking with this function. The tracking mode can be specified through INTTEvent:SetTrackingMode( string mode )
   @retval true in the case of successful tracking, otherwise false
   @details If it returns false, you don't need to count this event to the efficiency calculation neither denominator nor numerator.
*/
bool INTTEvent::DoTracking()
{

  if( mode_ == "fitting" )
    return this->TrackingFitting();
  
  return this->TrackingSimplest();
  //return true;
}

/**
   @brief not in use
 */
bool INTTEvent::IsGoodToAnalyze()
{

  // check whether the testing ladders have only single cluster, 
  for( int i=0; i<3; i++ )
    {
      if( focusing_ladder_ == -1 )
	continue;

      // all ladders should not have hits on chips other than the focusing chip
      if( !this->IsOnlyFocusingChip(i) )
	return false;

      if( i == focusing_ladder_ )
	continue;
      
      if( !this->IsSingleHit( i ) )
	return false;

      
      int index = find( module_.begin(), module_.end(), this->Ladder2Module(i) ) - module_.begin();

      if( focusing_chip_!= -1 && chip_id_[index] != focusing_chip_ )
	return false;
      
    }

  return true;
}

/**
   @brief It checks whether the specified ladder has only one cluster on the tested chip or not
 */
bool INTTEvent::IsOnlyFocusingChip( int ladder )
{

  // loop over all clusters in this event
  for( int i=0; i<dac_.size(); i++ )
    {

      // if this ladder is not one to be checked, skip it
      if( module_[i] != INTTEvent::Ladder2Module( ladder ) )
	continue;

      // if this hit is not on the focusing chip, return false
      if( chip_id_[i] != focusing_chip_ )
	{
	  return false;
	}
    }

  return true;
}

/**
   @brief The number of clusters belong to this ladder is returned
 */
int INTTEvent::GetHitNum( int ladder )
{
  int rtn = 0;

  // loop over all clusters in this event
  for( int i=0; i<chip_id_.size(); i++ )
    {
      int this_module = this->Ladder2Module( ladder );

      // module selection
      if( this_module != module_[i] )
	continue;

      // if the focusing chip is set, check whether this chip is the chip or not
      if( chip_id_[i] != this->focusing_chip_ && this->focusing_chip_ != -1 )
       	continue;
      
      rtn++;
      
    }

  return rtn;
}

/**
   @brief Switch to debug mode or normal mode
   @details In debug mode, std. out. is thrown away
 */
void INTTEvent::SetDebugMode( bool arg )
{
  debug_ = arg;
  if( debug_ )
    fitting_option_ = "QC";
}

/**
   @brief Hit information of this event is printed
 */
void INTTEvent::PrintEvent( int index )
{

  cout << "   " << "#hit, mod, chip, chan (range), DAC" << endl;

  for( int i=0; i<dac_.size(); i++ )
    {
      if( index != -1 )
	i = index;

      cout << "  - "
	   << setw(2) << nhit_in_cluster_[i] << ", "
	   << setw(2) << module_[i] << ", "
	   << setw(3) << chip_id_[i] << ", "
	   << setw(5) << setprecision(5) << chan_id_[i] << ", "
	   << "(" << setw(3) << chan_id_min_[i] << "-" << setw(3) << chan_id_max_[i] << "), "
	   << setw(3) << dac_[i];
      cout << endl;

      if( index != -1 )
	break;
    }

}

/**
   @brief Condition of tracking and eff. calc. is printed
*/
void INTTEvent::PrintCondition()
{
  cout << " * focusing "
       << "ladder" << setw(2) << focusing_ladder_ << ", "
       << "chip"   << setw(3) << focusing_chip_ << ", "
       << "channel"  << setw(3) << focusing_chan_ << ", ";
  //      << "alignment : " ;
  // for( auto& val : alignment_offset_ )
  //   cout << setw(5) << val << ", ";
  
    //<< "alignment : " << setw(5) << setprecision(3) << focusing_alignment_offset_ << " (strip)"
  cout << endl;
}

/*!
  @brief Everything is printed
 */
void INTTEvent::Print()
{
  cout << "-----------INTTEvent::Print---------------" << endl;
  this->PrintEvent();

  if( focusing_ladder_ != -1 )
    this->PrintCondition();

  cout << " * Alignment map:" << endl;
  for( int i=0; i<3; i++ )
    {
      cout << "ladder" << i << " | " ;
      for( int j=0; j<13; j++ )
	{
	  cout << setw(5) << setprecision(3) << alignment_map_[i][j] << ", ";
	}
      cout << endl;
    }
  
  
}

// private
void INTTEvent::Init()
{

}

/*!
  @brief The geometrical tracking is done in this function
  @param[in] index A list of index to be used for tracking. 2 indices must be provided.
  @details The calculation is quite simple. 
 */
void INTTEvent::GetTrackSimplest( vector < int >& index )
{

  pair < double , double > p1( this->Module2Ladder(module_[index[0]]), chan_id_[index[0]] ); // (x = ladder, y=channel)
  pair < double , double > p2( this->Module2Ladder(module_[index[1]]), chan_id_[index[1]] ); // (x = ladder, y=channel)

  cout << " - Tracking points: "
       << index[0] << ": " << "(" << p1.first << ", " << p1.second << ") -- "
       << index[1] << ": " << "(" << p2.first << ", " << p2.second << ")"
       << endl;
  
  //  y = ax + b
  // y1 = a x1 + b
  // y2 = a x2 + b
  // y1 - y2 = a (x1 - x2 )
  // a = (y1 - y2)/(x1 - x2)
  double slope = (p1.second - p2.second) / (p1.first - p2.first );

  // y1 = a x1 + b
  // b = y1 - a x1
  double intercept = p1.second - slope * p1.first;

  tracking_ = new TF1( "track", "pol1", 0, 3 );
  tracking_->SetParameters( intercept, slope );
}

/**
   @brief The closest hit to the predicted hit position is searched.
   @details Looking all hits on the tested chip of the tested ladder, take a position difference b/w its position and the predicted position, compare to the previous difference, and the smallest one is finally taken.
 */
void INTTEvent::SearchClosestHit()
{

  // tracking has to be perfomed in advance
  assert( tracked_position_ != -1 );

  double distance = 99999;
  int index_temp = -1;

  // loop over all hits
  cout << "\t" << "Searching..." << endl;
  for( int i=0; i<dac_.size(); i++ )
    {

      // If this hit doesn't belong to the tested ladder, skip it
      if( this->Module2Ladder(module_[i]) != focusing_ladder_ )
	{
	  cout << "\t\t" << "module: Is " << this->Module2Ladder(module_[i])
	       << " same as the focusing ladder" << focusing_ladder_ << "?  --> NO, skip it" << endl;
	  continue;
	  
	}

      // If this hit doesn't belong to the tested chip, skip it
      if( chip_id_[i] != focusing_chip_ )
	{
	  cout << "\t\t" << "chip: Is " << chip_id_[i]
	       << " same as the focusing chip" << focusing_chip_ << "?  --> NO, skip it" << endl;
	  continue;
	  
	}

      cout << "\t\t" << "module and chip are the same as the focuing ones" << endl;
      // Take the position difference with alignment correction
      double this_distance = fabs( chan_id_[i] - focusing_alignment_offset_ - tracked_position_ );
      cout << "\t\t" << "distance = "
	   << "fabs(" <<  chan_id_[i] << "+ " << -focusing_alignment_offset_  << " - " << tracked_position_ << ") = "
	   << this_distance << " ";

      // compare current difference to the previous result, if the current one is smaller, keep it
      if( this_distance < distance )
	{
	  cout << "hired";
	  index_temp = i;

	}
      else
	{
	  cout << "NOT hired";

	}

      cout << endl;
    }

  // store the index of the closest hit to the variable
  index_closest_to_tracking_ = index_temp;
}


void INTTEvent::SetAlignmentMap( string data )
{

  
  TFile* tf = new TFile( data.c_str(), "READ" );  
  for( int i=0; i<3; i++ )
    {
      for( int j=1; j<14; j++ )
	{

	  string name = string("ladder") + to_string(i) + "_chip" + to_string(j);
	  
	  auto hist = (TH1D*)tf->Get( name.c_str() );
	  if( hist == nullptr )
	    continue;
	  
	  auto f = (TF1*)hist->GetListOfFunctions()->At(0)->Clone();
	  alignment_map_[i][j-1] = f->GetParameter(1);

	}
    }
  tf->Close();
  
}

/**
   @brief This function does event selection, tracking, and the closest hit search   
 */
bool INTTEvent::TrackingSimplest()
{

  // The index of the closest hit is initialized
  index_closest_to_tracking_ = -1;

  // index of the testing ladders
  vector < int > testing_ladder_index;

  cout << "-- DoTracking" << endl;
  this->PrintCondition();
  
  // check whether the testing ladders have only single cluster,
  cout << " - check whether all ladders have appropriate hits or not" << endl;
  // loop over all ladders
  for( int i=0; i<3; i++ )
    {

      // If the tested ladder is not set, skip it
      if( focusing_ladder_ == -1 )
	continue;

      // stop checking tested ladder
      if( i == focusing_ladder_ )
	continue;
      
      // testing ladders should not have hits on chips other than the focusing chip
      if( !this->IsOnlyFocusingChip(i) )
	{
	  this->PrintEvent();
	  cout << "\t" << "+ ladder" << i << " have hit on non-focusing chip" << endl;
	  return false;
	}

      // If this ladder has multiple hits, skip this event
      if( !this->IsSingleHit( i ) )
	{
	  cout << "\t" << "+ ladder" << i << " have some hits on the focusing chip" << endl;
	  return false;
	}

      // Get the index of the hit this ladder (not moulde number1, 6, 5)
      int index = find( module_.begin(), module_.end(), this->Ladder2Module(i) ) - module_.begin();

      // If the tested chip has not beed set, skip this event
      // If the hit belongs to the other chip, skip this event
      if( focusing_chip_!= -1 && chip_id_[index] != focusing_chip_ )
	{
	  cout << "\t" << "+ ladder" << i << " have a hit on non-focusing chip" << chip_id_[index] << endl;
	  return false;
	}

      // store the index of this testing ladder
      testing_ladder_index.push_back( index );
    }

  // if the testing ladders don't have single cluster, skip this event
  cout << " - check there are 2 testing hits" << endl;
  if( testing_ladder_index.size() != 2 )
    {
      cout << "\t" << "+ No, there is " << testing_ladder_index.size() << " hit(s) as a testing hit. Skip it" << endl;
      return false;
    }

  // store indices of the hits for tracking, one for upstream ladder should be assigned to index_upstream_ladder....
  if( INTTEvent::Module2Ladder(module_[ testing_ladder_index[0] ])
      > INTTEvent::Module2Ladder(module_[ testing_ladder_index[1] ]) )
    {
      index_upstream_ladder_for_tracking_ = testing_ladder_index[0] ;
      index_downstream_ladder_for_tracking_ = testing_ladder_index[1] ;
    }
  else
    {
      index_upstream_ladder_for_tracking_ = testing_ladder_index[1] ;
      index_downstream_ladder_for_tracking_ = testing_ladder_index[0] ;

    }

  
  cout << " - the testing hits are:" << endl;
  for( auto& index : testing_ladder_index )
    {
      cout << "\t" << "+ index = " << index << "\n\t" ;
      this->PrintEvent( index );

    }

  cout << " - Do the simplest tracking" << endl;
  GetTrackSimplest( testing_ladder_index );

  tracked_position_ = tracking_->Eval( focusing_ladder_ );

  // if predicted position is outside the sensor, tracking is failed.
  if( tracked_position_ < 0 || tracked_position_>256 )
    return false;
  
  cout << " - Prediction: A hit should be ladder" << focusing_ladder_
       << " chip" << focusing_chip_
       << " ch" << tracked_position_
       << endl;

  cout << " - Search the closest hit to the predicted hit position" << endl;
  SearchClosestHit();
  
  if( index_closest_to_tracking_ != -1 )
    {  
      cout << "\t" << "Index of the closest hit: " << index_closest_to_tracking_ << endl;
      this->PrintEvent( index_closest_to_tracking_ );
    }
  else
    cout << "     No closest hit was found";
  
  
  cout << endl;

  return true;
}

///////////////////////////////////////////////////////////
// tracking with the fitting-method

void INTTEvent::GetTrackFitting( vector < int >& index_testing, vector < int >& index_tested )
{
  vector < double > chi2;

  // made a base graph to be used many times
  TGraph* g_base = new TGraph();
  for( auto& index : index_testing )
    {
      int ladder = INTTEvent::Module2Ladder(module_[index]);
      g_base->SetPoint( g_base->GetN(),
			ladder,
			//chan_id_[index] - this->GetAlignmentOffset(ladder) );
			chan_id_[index] - alignment_map_[ladder][focusing_chip_-1] );

    }

  if( index_tested.size() == 0 )
    {
      
      auto g = (TGraph*)g_base->Clone();
      tracking_ = new TF1( "track", "pol1", 0, 2 );      
      g->Fit( tracking_, fitting_option_.c_str() );
      index_closest_to_tracking_ = -1;
      return 0;      
    }
  
  vector < TF1* > track_candidates;
  // loop over all hits on the tested ladder to get the best track
  for( auto& index : index_tested )
    {

      cout << "GetTrackingFitting, " << index << ": " << chip_id_[index] << ", " << chan_id_[index] << " is tested" << endl;
      
      double ladder = INTTEvent::Module2Ladder(module_[index]);
      TF1* track = new TF1( "track_candidate", "pol1", 0, 2 );
      auto g = (TGraph*)g_base->Clone();
      g->SetPoint( g->GetN(), 
		   ladder, 
		   chan_id_[index] - alignment_map_[focusing_ladder_][focusing_chip_-1] );
		   //chan_id_[index] - this->GetAlignmentOffset(ladder) );

      if( debug_ )
	g->Print();
      
      g->Fit( track, fitting_option_.c_str() );

      track_candidates.push_back( track );
      chi2.push_back( track->GetChisquare() );
    }  
  
  int order_of_best_hit = min_element( chi2.begin(), chi2.end() ) - chi2.begin();
  int index_best_hit = index_tested[ order_of_best_hit ];
  
  tracking_ = track_candidates[ order_of_best_hit ];
  index_closest_to_tracking_ = index_best_hit;
  
}

bool INTTEvent::TrackingFitting()
{
  index_closest_to_tracking_ = -1;
  
  //this->SetFocusingAlignmentOffset( this->GetAlignmentOffset( focusing_ladder_ ) );
  this->SetFocusingAlignmentOffset( alignment_map_[focusing_ladder_][focusing_chip_-1] );
  
  vector < int > testing_ladder_index;  // list of index of the hits on the testing ladders
  vector < int > tested_ladder_index; // list of index of the hits on the tested ladder
  cout << "-- DoTracking, fitting method" << endl;
  this->PrintCondition();
  
  // check whether the testing ladders have only single cluster,
  cout << " - check whether all ladders have appropriate hits or not" << endl;
  for( int i=0; i<3; i++ )
    {
      if( focusing_ladder_ == -1 )
	continue;
      
      int index = find( module_.begin(), module_.end(), this->Ladder2Module(i) ) - module_.begin();

      // stop checking tested ladder
       if( i == focusing_ladder_ )
	 {
	   if( chip_id_[index] == focusing_chip_ )
	     tested_ladder_index.push_back( index );
	   
	   continue;
	 }
       
      // all ladders should not have hits on chips other than the focusing chip
      if( !this->IsOnlyFocusingChip(i) )
	{
	  cout << i << "\t" << focusing_ladder_ << endl;

	  cout << "\t" << "+ ladder" << i << " have hit on non-focusing chip" << endl;
	  return false;
	}


      // The testing ladders can have only a cluster on the testing chip. Nothing to do for the tested ladder.
      if( !this->IsSingleHit( i ) && i != focusing_ladder_ )
	{
	  cout << "\t" << "+ ladder" << i << " have some hits on the focusing chip" << endl;
	  return false;
	}

      if( focusing_chip_!= -1 && chip_id_[index] != focusing_chip_ )
	{
	  cout << "\t" << "+ ladder" << i << " have a hit on non-focusing chip" << chip_id_[index] << endl;
	  return false;
	}

      testing_ladder_index.push_back( index );
    }

  // if the testing ladders don't have single cluster, skip this event
  cout << " - check there are 2 testing hits" << endl;
  if( testing_ladder_index.size() != 2 )
    {
      cout << "\t" << "+ No, there is " << testing_ladder_index.size() << " hit(s) as a testing hit. Skip it" << endl;
      return false;
    }

  
  // store indices of the hits for tracking, one for upstream ladder should be assigned to index_upstream_ladder....
  if( INTTEvent::Module2Ladder(module_[ testing_ladder_index[0] ])
      > INTTEvent::Module2Ladder(module_[ testing_ladder_index[1] ]) )
    {
      index_upstream_ladder_for_tracking_ = testing_ladder_index[1] ;
      index_downstream_ladder_for_tracking_ = testing_ladder_index[0] ;
    }
  else
    {
      index_upstream_ladder_for_tracking_ = testing_ladder_index[0] ;
      index_downstream_ladder_for_tracking_ = testing_ladder_index[1] ;

    }

  cout << " - the testing hits are:" << endl;
  for( auto& index : testing_ladder_index )
    {
      cout << "\t" << "+ index = " << index << "\n\t" ;
      this->PrintEvent( index );
      
    }
  
  cout << " - Do tracking with the fitting method" << endl;
  GetTrackFitting( testing_ladder_index, tested_ladder_index );
  
  tracked_position_ = tracking_->Eval( focusing_ladder_ );
  
  // if predicted position is outside the sensor, tracking is failed.
  if( tracked_position_ < 0 || tracked_position_>256 )
    return false;
  
  cout << " - Prediction: A hit should be ladder" << focusing_ladder_
       << " chip" << focusing_chip_
       << " ch" << tracked_position_
       << endl;
  
  //cout << " - Search the closest hit to the predicted hit position" << endl;
  //SearchClosestHit();
  
  if( index_closest_to_tracking_ != -1 )
    {  
      cout << "\t" << "Index of the closest hit: " << index_closest_to_tracking_ << endl;
      this->PrintEvent( index_closest_to_tracking_ );
    }
  else
    cout << "     No closest hit was found";
  
  cout << endl;

  return true;
}
