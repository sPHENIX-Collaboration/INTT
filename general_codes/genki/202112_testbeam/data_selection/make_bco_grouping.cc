#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__

#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

#include "../Database.hh"
#include "../lib/functions.hh" // ClearVector
/*!
  @fn void CopyData( vector < int >& indices_ref, vector < int > *adc, vector < int > *ampl     , vector < int > *chip_id  , vector < int > *fpga_id  , vector < int > *module   , vector < int > *chan_id  , vector < int > *fem_id   , vector < int > *bco      , vector < int > *bco_full , vector < int > *event,	       vector < int >& adc_dest, vector < int >& ampl_dest, vector < int >& chip_id_dest, vector < int >& fpga_id_dest, vector < int >& module_dest, vector < int >& chan_id_dest, vector < int >& fem_id_dest, vector < int >& bco_dest, vector < int >& bco_full_dest, vector < int >& event_dest, bool is_removed )
  @brief values in the 2nd-11th vector variables are copied to the 12th-21st 
  @param is_removed A switch to copy the original hits (true) or clone hits (false)
*/
void FillBCOGroups(
		 TTree* tr,
		 vector < int > *camac_adc,
		 vector < int > *camac_tdc,
		 vector < int > *adc      ,
		 vector < int > *ampl     ,
		 vector < int > *chip_id  ,
		 vector < int > *fpga_id  ,
		 vector < int > *module   ,
		 vector < int > *chan_id  ,
		 vector < int > *fem_id   ,
		 vector < int > *bco      ,
		 vector < int > *bco_full ,
		 vector < int > *event,

		 int trigger_id,
		 int& bco_group_id,
		 //vector < int >& hit_id,
		 vector < int >& adc_bco_grouping,
		 vector < int >& ampl_bco_grouping,
		 vector < int >& chip_id_bco_grouping,
		 vector < int >& fpga_id_bco_grouping,
		 vector < int >& module_bco_grouping,
		 vector < int >& chan_id_bco_grouping,
		 vector < int >& fem_id_bco_grouping,
		 vector < int >& bco_bco_grouping,
		 vector < int >& bco_full_bco_grouping,
		 vector < int >& event_bco_grouping
		 )

{

  if( adc->size() == 0 )
    return;
  
  int current_bco_value = (*bco_full)[0];

  bco_group_id = 0;  
  ClearVector( adc_bco_grouping );
  ClearVector( ampl_bco_grouping );
  ClearVector( chip_id_bco_grouping );
  ClearVector( fpga_id_bco_grouping );
  ClearVector( module_bco_grouping );
  ClearVector( chan_id_bco_grouping );
  ClearVector( fem_id_bco_grouping );
  ClearVector( bco_bco_grouping );
  ClearVector( bco_full_bco_grouping );
  ClearVector( event_bco_grouping );

  stringstream line_original, line_grouping;

  // loop over all INTT hits
  for( int i=0; i<adc->size(); i++ )
    {

      // if this INTT hit belongs to the same BCO group, keep the info
      if( current_bco_value == (*bco_full)[i] )
	{
	  
	  adc_bco_grouping.push_back( (*adc)[i] );
	  ampl_bco_grouping.push_back( (*ampl)[i] );
	  chip_id_bco_grouping.push_back( (*chip_id)[i] );
	  fpga_id_bco_grouping.push_back( (*fpga_id)[i] );
	  module_bco_grouping.push_back( (*module)[i] );
	  chan_id_bco_grouping.push_back( (*chan_id)[i] );
	  fem_id_bco_grouping.push_back( (*fem_id)[i] );
	  bco_bco_grouping.push_back( (*bco)[i] );
	  bco_full_bco_grouping.push_back( (*bco_full)[i] );
	  event_bco_grouping.push_back( (*event)[i] );

	}
      else
	{

	  tr->Fill();

	  ClearVector( adc_bco_grouping );
	  ClearVector( ampl_bco_grouping );
	  ClearVector( chip_id_bco_grouping );
	  ClearVector( fpga_id_bco_grouping );
	  ClearVector( module_bco_grouping );
	  ClearVector( chan_id_bco_grouping );
	  ClearVector( fem_id_bco_grouping );
	  ClearVector( bco_bco_grouping );
	  ClearVector( bco_full_bco_grouping );
	  ClearVector( event_bco_grouping );

	  bco_group_id++;
	  
	  if( i < adc->size() )
	    {
	      i--;
	      current_bco_value = (*bco_full)[i+1]; // set the current BCO value for the next BCO group
	      continue;
	    }
	    
	  
	}
    }

  tr->Fill(); // Don't forget filling the last BCO group
  

  string cut = string( "trigger_id==" ) + to_string( trigger_id );
}

int make_bco_grouping( int run_num = 89, bool is_MC = false )
{

  if( is_MC == false )
    {
      Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
      db->SetRun( run_num );
      
      if( db->IsBeamRun() == false )
	return 0;
    }
  
  string output_root_dir = "results/ELPH/trim_data/";  
  string data = output_root_dir + "no_clone/" + "run" + to_string( run_num ) + "_no_clone.root";
  if( is_MC )
    data = data.substr( 0, data.size() - 5 ) + "_MC.root";
  
  TFile* tf = new TFile( data.c_str(), "READ" );
  auto tr = (TTree*)tf->Get( "tree_both" );

  // preparation to read data from TTree event by event (event means trigger event, some (or 1 or no) INTT hits belong to the trigger event)
  vector < int > *camac_adc(0); // initialization by (0) is necessary
  vector < int > *camac_tdc(0);
  vector < int > *adc      (0);
  vector < int > *ampl     (0);
  vector < int > *chip_id  (0);
  vector < int > *fpga_id  (0);
  vector < int > *module   (0);
  vector < int > *chan_id  (0);
  vector < int > *fem_id   (0);
  vector < int > *bco      (0);
  vector < int > *bco_full (0);
  vector < int > *event    (0);
  //bool INTT_event; // it's not so important...

  tr->SetBranchAddress( "camac_adc"	, &camac_adc );
  tr->SetBranchAddress( "camac_tdc"	, &camac_tdc );
  tr->SetBranchAddress( "adc"		, &adc       );
  tr->SetBranchAddress( "ampl"		, &ampl      );
  tr->SetBranchAddress( "chip_id"	, &chip_id   );
  tr->SetBranchAddress( "fpga_id"	, &fpga_id   );
  tr->SetBranchAddress( "module"	, &module    );
  tr->SetBranchAddress( "chan_id"	, &chan_id   );
  tr->SetBranchAddress( "fem_id"	, &fem_id    );
  tr->SetBranchAddress( "bco"		, &bco       );
  tr->SetBranchAddress( "bco_full"	, &bco_full  );
  tr->SetBranchAddress( "event"		, &event     );
  //tr->SetBranchAddress( "INTT_event"	, &INTT_event );

  string output = output_root_dir + "bco_grouping/" + "run" + to_string( run_num ) + ".root";
  if( is_MC )
    output = output.substr( 0, output.size() - 5 ) + "_MC.root" ;

  TFile* tf_output = new TFile( output.c_str(), "RECREATE" );
  TTree* tr_output = new TTree( "tree_both", "tree for CAMAC data and associated INTT hits without clone hits" );

  int trigger_id = 0, bco_group_id = 0;
  
  vector < int > adc_bco_grouping, ampl_bco_grouping, chip_id_bco_grouping, fpga_id_bco_grouping, module_bco_grouping, chan_id_bco_grouping, fem_id_bco_grouping, bco_bco_grouping, bco_full_bco_grouping, event_bco_grouping;
  vector < int > hit_id_in_bco_group;
  //  tr_output->Branch( "hit_id", &hit_id_in_bco_group );
  tr_output->Branch( "group_id", &bco_group_id, "group_id/I" );  
  tr_output->Branch( "adc",			&adc_bco_grouping );
  tr_output->Branch( "ampl",			&ampl_bco_grouping );
  tr_output->Branch( "chip_id",		&chip_id_bco_grouping );
  tr_output->Branch( "fpga_id",		&fpga_id_bco_grouping );
  tr_output->Branch( "module",		&module_bco_grouping );
  tr_output->Branch( "chan_id",		&chan_id_bco_grouping );
  tr_output->Branch( "fem_id",		&fem_id_bco_grouping );
  tr_output->Branch( "bco",			&bco_bco_grouping );
  tr_output->Branch( "bco_full",		&bco_full_bco_grouping );
  tr_output->Branch( "event",			&event_bco_grouping );

  tr_output->Branch( "trigger_id", &trigger_id, "trigger_id/I" );
  tr_output->Branch( "camac_adc",		&(*camac_adc) );
  tr_output->Branch( "camac_tdc",		&(*camac_tdc) );
  //  tr_output->Branch( "INTT_event",		&INTT_event, "INTT_event/O" );


  cout << " - Run: " << run_num << endl;
  cout << " - Data: " << data << endl;
  cout << " - Output: " << output << endl;
  cout << " - Is MC? " << is_MC << endl;
  
  bool debug = false;
  // loop over all entries in tree_both
  for( int i=0; i<tr->GetEntries(); i++, trigger_id++ )
    {

      tr->GetEntry( i );
      cout << flush << "\r" << "#Entry: " << setw(7) << i << "/" << tr->GetEntries()
	   << ",\t#INTT hits: " << adc->size();      
      FillBCOGroups( tr_output,
		     camac_adc,	            camac_tdc,
		     adc, 	            ampl,                 chip_id,              fpga_id,
		     module,                chan_id,              fem_id,               bco,
		     bco_full,              event,
		     trigger_id,            bco_group_id,
		     adc_bco_grouping,      ampl_bco_grouping,    chip_id_bco_grouping, fpga_id_bco_grouping,
		     module_bco_grouping,   chan_id_bco_grouping, fem_id_bco_grouping,  bco_bco_grouping,
		     bco_full_bco_grouping, event_bco_grouping );

      if( debug && i == 4 )
	break;
    }


  tf_output->WriteTObject( tr_output, "tr" );
  tf_output->Close();
  tf->Close();
  return 0;
}
