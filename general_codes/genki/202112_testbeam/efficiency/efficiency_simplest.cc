#define __CINT__
//#define DEBUG

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


#include "../Database.hh"
#include "../lib/functions.hh"
//#include "INTThit.hh"
#include "../lib/INTTEvent.hh"


#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif 

bool FindLadder( int ladder_looking_for, vector < int > *modules )
{
  for( auto& module : *modules )
    if( ladder_looking_for == module )
      return true;

  return false;
}

/*!
  @fn int efficiency_simplest
*/
int efficiency_simplest( int run_num = 89, bool is_MC = false  )
{
  
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run_num );
  const string kOriginal_data_dir = "data/ELPH/";
  
  if( is_MC == false )
    {
      
      if( db->IsBeamRun() == false )
	return 0;
      
    }
  else // for MC data
    {
      
      // use Takashi's config in any case for the moment
      db->SetRun( 89 );
      db->SetRun( run_num );
      
    }
  
  //const string kOutput_root_dir = "results/ELPH/efficiency/simplest/";
  const string kOutput_root_dir = "results/ELPH/";
  string data = kOutput_root_dir + "trim_data/clustered/" + "run" + to_string( run_num ) + ".root";
  if( is_MC )
    data = data.substr( 0, data.size() - 5 ) + "_MC.root";
  
  TFile* tf = new TFile( data.c_str(), "READ" );
  auto tr = (TTree*)tf->Get( "tr" );

  // preparation to read data from TTree event by event (event means trigger event, some (or 1 or no) INTT hits belong to the trigger event)
  int trigger_id = -1, group_id = -1;
  vector < int > *nhit_in_cluster(0);
  vector < int > *is_over_chip(0);
  vector < int > *chan_id_min(0);
  vector < int > *chan_id_max(0);
  vector < int > *adc0_num(0);
  vector < int > *adc1_num(0);
  vector < int > *adc2_num(0);
  vector < int > *adc3_num(0);
  vector < int > *adc4_num(0);
  vector < int > *adc5_num(0);
  vector < int > *adc6_num(0);
  vector < int > *adc7_num(0);
  vector < int > *camac_adc(0);
  vector < int > *camac_tdc(0);
  vector < int > *module(0);

  vector < int > *adc(0);
  vector < double > *chip_id(0);
  vector < int > *fpga_id(0);
  vector < double > *chan_id(0);
  vector < int > *fem_id(0);
  vector < int > *bco(0);
  vector < int > *bco_full(0);
  vector < double > *dac(0);
  vector < double > *chip_id_simple(0);
  vector < double > *chip_sigma(0);
  vector < double > *chip_sigma_simple(0);
  vector < double > *chan_id_simple(0);
  vector < double > *chan_sigma(0);
  vector < double > *chan_sigma_simple(0);

  tr->SetBranchAddress( "trigger_id"		, &trigger_id		);
  tr->SetBranchAddress( "camac_adc"		, &camac_adc		);
  tr->SetBranchAddress( "camac_tdc"		, &camac_tdc		);

  tr->SetBranchAddress( "group_id"		, &group_id		);
  tr->SetBranchAddress( "nhit_in_cluster"	, &nhit_in_cluster	);
  tr->SetBranchAddress( "is_over_chip"		, &is_over_chip		);
  tr->SetBranchAddress( "fem_id"		, &fem_id		);
  tr->SetBranchAddress( "fpga_id"		, &fpga_id		);
  tr->SetBranchAddress( "module"		, &module		);

  tr->SetBranchAddress( "chip_id"		, &chip_id		);
  tr->SetBranchAddress( "chan_id"		, &chan_id		);
  tr->SetBranchAddress( "chan_id_simple"	, &chan_id_simple	);
  tr->SetBranchAddress( "chan_sigma"		, &chan_sigma		);
  tr->SetBranchAddress( "chan_sigma_simple"	, &chan_sigma_simple	);
  tr->SetBranchAddress( "chan_id_min"		, &chan_id_min		);

  tr->SetBranchAddress( "chan_id_max"		, &chan_id_max		);
  tr->SetBranchAddress( "adc"			, &adc			);
  tr->SetBranchAddress( "adc0_num"		, &adc0_num		);
  tr->SetBranchAddress( "adc1_num"		, &adc1_num		);
  tr->SetBranchAddress( "adc2_num"		, &adc2_num		);
  tr->SetBranchAddress( "adc3_num"		, &adc3_num		);
  tr->SetBranchAddress( "adc4_num"		, &adc4_num		);
  tr->SetBranchAddress( "adc5_num"		, &adc5_num		);
  tr->SetBranchAddress( "adc6_num"		, &adc6_num		);
  tr->SetBranchAddress( "adc7_num"		, &adc7_num		);
  tr->SetBranchAddress( "dac"			, &dac			);
  tr->SetBranchAddress( "bco"			, &bco			);
  tr->SetBranchAddress( "bco_full"		, &bco_full		);

  // preparation for outputs
  const string kOutput_dir = kOutput_root_dir + "efficiency/simplest/" ;
  string output = kOutput_dir + "run" + to_string( run_num ) + ".root";
  if( is_MC )
    output = output.substr( 0, output.size() - 5 ) + "_MC.root";

  TFile* tf_output = new TFile( output.c_str(), "RECREATE" );
  TTree* tr_output = new TTree( "tr", "tree for clustered INTT hits in a BCO group and their trigger information" );

  // clustered tree variables, int
  int trigger_id_out, group_id_out, nhit_in_cluster_out[3], is_over_chip_out[3], fem_id_out[3],
    fpga_id_out[3], module_out[3], chan_id_min_out[3], chan_id_max_out[3], adc_out[3],
    adc0_num_out[3], adc1_num_out[3], adc2_num_out[3], adc3_num_out[3],
    adc4_num_out[3], adc5_num_out[3], adc6_num_out[3], adc7_num_out[3],
    bco_out, bco_full_out;

  // clustered tree variables, double
  double chip_id_out[3], chan_id_out[3], chan_id_simple_out[3],
    chan_sigma_out[3], chan_sigma_simple_out[3], dac_out[3];

  //new variables, int
  int focusing_module_out, focusing_chip_out;
  // new variables, double
  double chip_id_tracking_out, chan_id_tracking_out, track_slope_out, track_intercept_out, diff_chan_out, diff_chip_out;
  
  tr_output->Branch( "trigger_id"		, &trigger_id, "trigger_id/I" ); // connected to the same variable
  // tr_output->Branch( "camac_adc"		, &camac_adc_out, "camac_adc/I" );
  // tr_output->Branch( "camac_tdc"		, &camac_tdc_out, "camac_tdc/I" );
  tr_output->Branch( "group_id"			, &group_id, "group_id/I" );  // connected to the same variable
  tr_output->Branch( "nhit_in_cluster"		, nhit_in_cluster_out, "nhit_in_cluster[3]/I" );
  tr_output->Branch( "is_over_chip"		, is_over_chip_out, "is_over_chip[3]/I" );
  tr_output->Branch( "fem_id"			, fem_id_out, "fem_id[3]/I" );
  tr_output->Branch( "fpga_id"			, fpga_id_out, "fpga_id[3]/I" );
  tr_output->Branch( "module"			, module_out, "module[3]/I" );
  tr_output->Branch( "chip_id"			, chip_id_out, "chip_id[3]/D" );
  tr_output->Branch( "chan_id"			, chan_id_out, "chan_id[3]/D" );
  tr_output->Branch( "chan_id_simple"		, chan_id_simple_out, "chan_id_simple[3]/D" );
  tr_output->Branch( "chan_sigma"		, chan_sigma_out, "chan_sigma[3]/D" );
  tr_output->Branch( "chan_sigma_simple"	, chan_sigma_simple_out, "chan_sigma_simple[3]/D" );
  tr_output->Branch( "chan_id_min"		, chan_id_min_out, "chan_id_min[3]/I" );
  tr_output->Branch( "chan_id_max"		, chan_id_max_out, "chan_id_max[3]/I" );
  tr_output->Branch( "adc"			, adc_out, "adc[3]/I" );
  tr_output->Branch( "adc0_num"			, adc0_num_out, "adc0_num[3]/I" );
  tr_output->Branch( "adc1_num"			, adc1_num_out, "adc1_num[3]/I" );
  tr_output->Branch( "adc2_num"			, adc2_num_out, "adc2_num[3]/I" );
  tr_output->Branch( "adc3_num"			, adc3_num_out, "adc3_num[3]/I" );
  tr_output->Branch( "adc4_num"			, adc4_num_out, "adc4_num[3]/I" );
  tr_output->Branch( "adc5_num"			, adc5_num_out, "adc5_num[3]/I" );
  tr_output->Branch( "adc6_num"			, adc6_num_out, "adc6_num[3]/I" );
  tr_output->Branch( "adc7_num"			, adc7_num_out, "adc7_num[3]/I" );
  tr_output->Branch( "dac"			, dac_out, "dac[3]/D" );
  tr_output->Branch( "bco"			, &bco_out, "bco/I" );
  tr_output->Branch( "bco_full"			, &bco_full_out, "bco_full/I" );

  tr_output->Branch( "focusing_module"          , &focusing_module_out, "focusing_moudle_out/I" );
  tr_output->Branch( "focusing_chip"            , &focusing_chip_out, "focusing_moudle_out/I" );
  tr_output->Branch( "chip_id_tracking"         , &chip_id_tracking_out, "chip_id_tracking_out/D" );
  tr_output->Branch( "chan_id_tracking"         , &chan_id_tracking_out, "chan_id_tracking_out/D" );
  tr_output->Branch( "diff_chip"                , &diff_chip_out, "diff_chip_out/D" );
  tr_output->Branch( "diff_chan"                , &diff_chan_out, "diff_chan_out/D" );
  tr_output->Branch( "track_slope"              , &track_slope_out, "track_slope_out/D" );
  tr_output->Branch( "track_intercept"          , &track_intercept_out, "track_intercept_out/D" );

  // loop over all entries in tree_both
  vector < int > hot_chips;
  hot_chips.push_back( GetHotChip( tr, INTTEvent::Ladder2Module(0), "" ) );
  hot_chips.push_back( GetHotChip( tr, INTTEvent::Ladder2Module(1), "" ) );
  hot_chips.push_back( GetHotChip( tr, INTTEvent::Ladder2Module(2), "" ) );

  for( int i=10; i<tr->GetEntries(); i++ )
    {
      
      tr->GetEntry( i );
      
      // for debugging
      if( adc->size() == 0 )
	continue;

      if( i % int(tr->GetEntries() * 0.05 ) == 0 )
	cout << setw(6) << setprecision(3) << 100.0 * i / tr->GetEntries()  << "% "
	     << setw(6) << i << " "
	     << setw(6) << group_id << " "
	     << setw(6) << trigger_id << " "
	     << setw(6) << adc->size()
	     << endl;
      
      INTTEvent* event = new INTTEvent( nhit_in_cluster, is_over_chip,
				       module,
				       chip_id,
				       chan_id,  chan_id_simple, chan_sigma, chan_sigma_simple,
				       chan_id_min, chan_id_max,
				       dac );
      event->SetDebugMode( debug );

      if( debug )
	{
	  cout << endl << "---------------------------" << endl;
	  event->PrintEvent();
	}

      // loop over tested ladder
      for( int j=0; j<3; j++ )
	{
	  event->SetFocusingLadder( j );
	  event->SetFocusingChip( -1 );

	  focusing_module_out = INTTEvent::Ladder2Module( j );
	  // loop over chips to be forcused
	  for( int k = hot_chips[j]-2; k<hot_chips[j]+2+1; k++ )
	    {

	      for( int m=0; m<3; m++ )
		{
		  // initialization of variables for tr_output
		  // int	      
		  trigger_id_out = group_id_out = nhit_in_cluster_out[m] = is_over_chip_out[m] = fem_id_out[m]
		    = fpga_id_out[m] = module_out[m] = chan_id_min_out[m] = chan_id_max_out[m] = adc_out[m]
		    = adc0_num_out[m] = adc1_num_out[m] = adc2_num_out[m] = adc3_num_out[m]
		    = adc4_num_out[m] = adc5_num_out[m] = adc6_num_out[m] = adc7_num_out[m]
		    = bco_out = bco_full_out
		    = -9999;
		  
		  // double
		  chip_id_out[m] = chan_id_out[m] = chan_id_simple_out[m]
		    = chan_sigma_out[m] = chan_sigma_simple_out[m] = dac_out[m]
		    = chip_id_tracking_out = chan_id_tracking_out = track_slope_out = track_intercept_out
		    = diff_chip_out = diff_chan_out
		    = -9999.9;
		}
	      
	      focusing_chip_out = k;
	      event->SetFocusingChip( focusing_chip_out );

	      if( !event->IsGoodToAnalyze() )
		continue;

	      if( !is_MC )
		{
		  if( j == 0 )
		    //event->SetAlignmentOffset( 7.220671 );
		    event->SetFocusingAlignmentOffset( 0.0 );
		  else if( j == 1 )
		    event->SetFocusingAlignmentOffset( 3.634177 );
		  else if( j == 2 )
		    event->SetFocusingAlignmentOffset( -7.220671 );
		}
	      else
		{
		  event->SetFocusingAlignmentOffset( 0.0 );
		}
	      
	      bool tracking_condition = event->DoTracking();

	      if( tracking_condition )
		{

		  event->PrintCondition();
		}

	      // if tracking cannot be done, skip this event
	      if( tracking_condition == false )
		{
		  if( debug )
		    cout << " - Tracking failed, so skipped" << endl;
		  continue;
		}

	      int hit_indices[3];
	      hit_indices[0] = event->GetHitIndexClosestToTracking(); // for the hit on the tested ladder
	      hit_indices[1] = event->GetHitIndexForTrackingUpstream(); // hit on the upstream ladder for tracking
	      hit_indices[2] = event->GetHitIndexForTrackingDownstream(); // hit on the downstream ladder for tracking

	      for( int m=0; m<3; m++ )		
		{
		  int index = hit_indices[m];
		  if( index != -1 )
		    {
		      
		      nhit_in_cluster_out[m] = (*nhit_in_cluster)[index];
		      is_over_chip_out[m] = (*is_over_chip)[index];
		      fem_id_out[m] = (*fem_id)[index];
		      fpga_id_out[m] = (*fpga_id)[index];
		      module_out[m] = (*module)[index];
		  
		      chip_id_out[m] = (*chip_id)[index];
		      chan_id_out[m] = (*chan_id)[index] + event->GetFocusingAlignmentOffset();
		      chan_id_simple_out[m] = (*chan_id_simple)[index];
		      chan_sigma_out[m] = (*chan_sigma)[index];
		      chan_sigma_simple_out[m] = (*chan_sigma_simple)[index];
		      chan_id_min_out[m] = (*chan_id_min)[index];
		  
		      chan_id_max_out[m] = (*chan_id_max)[index];
		      adc_out[m] = (*adc)[index];
		      adc0_num_out[m] = (*adc0_num)[index];
		      adc1_num_out[m] = (*adc1_num)[index];
		      adc2_num_out[m] = (*adc2_num)[index];
		      adc3_num_out[m] = (*adc3_num)[index];
		      adc4_num_out[m] = (*adc4_num)[index];
		      adc5_num_out[m] = (*adc5_num)[index];
		      adc6_num_out[m] = (*adc6_num)[index];
		      adc7_num_out[m] = (*adc7_num)[index];
		      dac_out[m] = (*dac)[index];
		      bco_out = (*bco)[index];
		      bco_full_out = (*bco_full)[index];
		  
		    }
		}

	      chip_id_tracking_out = event->GetTrackedChip( j );
	      chan_id_tracking_out = event->GetTrackedChannel( j );
	      diff_chip_out = chip_id_out[0] - chip_id_tracking_out;
	      diff_chan_out = chan_id_out[0] - event->GetFocusingAlignmentOffset()- chan_id_tracking_out;
	      
	      track_slope_out = event->GetTrackSlope();
	      track_intercept_out = event->GetTrackIntercept();
	      
	      tr_output->Fill();

	      if( debug )
		break;
	      
	    }

	  // if( debug )
	  //   break;
	}
      
      // for debugging
      if( debug && i > 30 )
	{

	  break;
	}

      
    }
  
  stringstream comment;
  comment << "Input: " << data << endl;
  comment << "Output: " << output << endl;

  cout << comment.str() << endl;

  tr_output->Print();
  tf_output->WriteTObject( tr_output, tr_output->GetName() );
  tf_output->Close();
  
  tf->Close();
  return 0;
}
