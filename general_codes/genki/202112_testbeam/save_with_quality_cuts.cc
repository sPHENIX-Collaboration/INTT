#define __CINT__

#include "Database.hh"
#include "RemoveThisHit.hh"

int save_with_quality_cuts( string data = "data/ELPH/BeamData_20211207-1156_0.root", string mode = "noise" ) // noise, clone-hit, clone-hit-pm1 
{
  Database* db = new Database();
  db->SetRun( data );
  db->GetRun()->Print();

  TCut cut_module( db->GetModuleCut().c_str() );
  TCut cut_undefined( "0<chip_id && chip_id<27 && -1 < chan_id && chan_id < 129 && ampl == 0" );
  
  auto tf = new TFile( data.c_str(), "READ" );
  auto tr_original = (TTree*)tf->Get( "tree_both" );
  
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
  bool INTT_event; // it's not so important...

  tr_original->SetBranchAddress( "camac_adc"	, &camac_adc );
  tr_original->SetBranchAddress( "camac_tdc"	, &camac_tdc );
  tr_original->SetBranchAddress( "adc"		, &adc       );
  tr_original->SetBranchAddress( "ampl"		, &ampl      );
  tr_original->SetBranchAddress( "chip_id"	, &chip_id   );
  tr_original->SetBranchAddress( "fpga_id"	, &fpga_id   );
  tr_original->SetBranchAddress( "module"	, &module    );
  tr_original->SetBranchAddress( "chan_id"	, &chan_id   );
  tr_original->SetBranchAddress( "fem_id"	, &fem_id    );
  tr_original->SetBranchAddress( "bco"		, &bco       );
  tr_original->SetBranchAddress( "bco_full"	, &bco_full  );
  tr_original->SetBranchAddress( "event"	, &event     );
  tr_original->SetBranchAddress( "INTT_event"	, &INTT_event );

  auto tr_output = new TTree( "name_temp", "title_temp" );
  tr_original->Print();


  int all_entries = tr_original->GetEntries();
  for( int i=0; i<all_entries; i++ )
    {
      tr_original->GetEntry( i );
      cout << i << " " << adc->size() << endl;
      
      // loop over all INTT hits in this trigger event
      for( int j=0; j<adc->size(); j++ )
	{

	  cout << right << setw(5) << i << " "
	       << setw(5) << j << " "
	       << setw(5) << (*module)[j] << " "
	       << setw(5) << (*chip_id)[j] << " "
	       << setw(4) << (*chan_id)[j] << " "
	       << setw(8) << (*bco_full)[j]
	       << endl;
	    
	  bool remove_this_hit = false;
	  if( mode.find( "noise" ) != string::npos )
	    {


	      // if the module of this hit is not in the module list, set the flag true
	      for( auto& module_in_use : db->GetModules() )
		{
		  if( (*module)[j] == module_in_use )
		    break;
		  //		  cout << (*module)[j] << " " << module_in_use << " " << ( (*module)[j] == module_in_use ) << endl;
		  remove_this_hit = remove_this_hit || ((*module)[j] == module_in_use );
		}

	      
	      // if the chip of this hit is out of range, set the flag true
	      if( (*chip_id)[j] < 1 || 26 < (*chip_id)[j] )
		remove_this_hit = true;
	      
	      // if the channel of this hit is out of range, set the flag true
	      if( (*chan_id)[j] < 0 || 127 < (*chan_id)[j] )
		remove_this_hit = true;
	      
	    }

	  
	  if( mode.find( "clone-hit" ) != string::npos )
	    {

	      for( int k=j+1; k<adc->size(); k++ )
		{
		  if( (*module)[j] == (*module)[k] )
		    if( (*chip_id)[j] == (*chip_id)[k] )
		      if( (*chan_id)[j] == (*chan_id)[k] )
			if( (*adc)[j] == (*adc)[k] )
			  {
			    if( mode.find( "clone-hit-pm1" ) != string::npos )
			      {
				//				if( abs((*bco_full)[j]-(*bco_full)[k]) < 2 )
				  
			      }
			    else
			      {
				//				if( abs((*bco_full)[j] == (*bco_full)[k]))

			      }

			  }
		  
		}


	    }
	  
	  if( remove_this_hit )
	    RemoveThisHit( j,
			   camac_adc, camac_tdc,
			   adc      , ampl     , chip_id  ,
			   fpga_id  , module   , chan_id  ,
			   fem_id   , bco      , bco_full ,
			   event    			   
			   );
	} // for j

      cout << "---------------------------=" << endl;
      // loop over all INTT hits in this trigger event
      for( int j=0; j<adc->size(); j++ )
	{

	  cout << right << setw(5) << i << " "
	       << setw(5) << j << " "
	       << setw(5) << (*module)[j] << " "
	       << setw(5) << (*chip_id)[j] << " "
	       << setw(4) << (*chan_id)[j] << " "
	       << setw(8) << (*bco_full)[j]
	       << endl;
	}
      
      if( i == 40 )
	break;
    } // for i



  return 0;
}
