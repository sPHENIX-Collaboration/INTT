#ifdef __CINT__
#pragma once
#include "CutData.hh"
#endif 
/*! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern bool show_details;

bool IsOneOfModule( int module )
{
  vector < int > module_list;
  module_list.push_back( 1 );
  module_list.push_back( 6 );
  //  module_list.push_back( 8 );
  module_list.push_back( 5 );

  for( auto& value : module_list )
    if( value == module )
      return true;

  return false;
}

bool IsOneOfFemId( int module, int fem_id )
{
  if( module == 1 || module == 2 )
    {
      if( fem_id == 1 )
	return true;
    }
  else if( module == 3 || module == 4 )
    {
      if( fem_id == 2 )
	return true;
    }
  else if( module == 5 || module == 6 )
    {
      if( fem_id == 8 )
	return true;
    }
  else if( module == 7 || module == 8 )
    {
      if( fem_id == 4 )
	return true;
    }

  return false;
}

/*!
  @fn void RemoveThisHit
  @brief Element at index in the vector arguments is removed
*/
void RemoveThisHit(
		   int index, 
		   vector < int > *adc      ,
		   vector < int > *ampl     ,
		   vector < int > *chip_id  ,
		   vector < int > *fpga_id  ,
		   vector < int > *module   ,
		   vector < int > *chan_id  ,
		   vector < int > *fem_id   ,
		   vector < int > *bco      ,
		   vector < int > *bco_full ,
		   vector < int > *event    )
{
  adc->erase		( adc->begin() + index		);
  ampl->erase		( ampl->begin() + index		);
  chip_id->erase	( chip_id->begin() + index	);
  fpga_id->erase	( fpga_id->begin() + index	);
  module->erase		( module->begin() + index	);
  chan_id->erase	( chan_id->begin() + index	);
  fem_id->erase		( fem_id->begin() + index	);
  bco->erase		( bco->begin() + index		);
  bco_full->erase	( bco_full->begin() + index	);
  event->erase		( event->begin() + index	);
}


// void RequireHitOnAllLadder( 
// 			   vector < int > *adc      ,
// 			   vector < int > *ampl     ,
// 			   vector < int > *chip_id  ,
// 			   vector < int > *fpga_id  ,
// 			   vector < int > *module   ,
// 			   vector < int > *chan_id  ,
// 			   vector < int > *fem_id   ,
// 			   vector < int > *bco      ,
// 			   vector < int > *bco_full ,
// 			   vector < int > *event    )
// {
//   vector < int > module_list;
//   module_list.push_back( 8 );
//   module_list.push_back( 6 );
//   module_list.push_back( 1 );

  
//   for( auto& using_modue : modules )
//     {

//       for( auto& mod : module )
// 	if( mod == sing_module )
	  

//     }
  
// }

bool RemoveCloneHitEvent(
			 bool pm1bco,
			 vector < int > *adc      ,
			 vector < int > *ampl     ,
			 vector < int > *chip_id  ,
			 vector < int > *fpga_id  ,
			 vector < int > *module   ,
			 vector < int > *chan_id  ,
			 vector < int > *fem_id   ,
			 vector < int > *bco      ,
			 vector < int > *bco_full ,
			 vector < int > *event    )
{
  bool is_clone_exist = false;
  bool is_removed = false;

  if( show_details )
    {
      cout << "Removing cloned hits: ";
      PrintVectorContents( *module, " ", " -> " );
    }
  
  // loop over a hit
  for( int i=0; i<adc->size(); i++ )
    {

      // loop over another hit for comparison
      for( int j=i+1; j<adc->size(); j++ )
	{

	  // if parameters below are the same, the hit is cloned
	  if( (*adc)[i] == (*adc)[j] )
	    if( (*chip_id)[i] == (*chip_id)[j] )
	      if( (*module)[i] == (*module)[j] )
		if( (*chan_id)[i] == (*chan_id)[j] )
		  {
		    cout << i << "-" << j << " almost same" << endl;
		    cout << (*bco_full)[i] << " and " <<  (*bco_full)[j] ) << endl;
		    if( pm1bco == false )
		      {
			if( (*bco_full)[i] == (*bco_full)[j] )
			  is_removed = true;
		      }
		    else if( abs( (*bco_full)[i] - (*bco_full)[j] ) < 2 )
		      is_removed = true;
		    
		    cout << " ===> " << is_removed << endl;
		  } // if( chan_id )
	  
	  if( is_removed )
	    {
	      cout << j << " should be rmoved" << endl;
	      is_clone_exist = true;
	      RemoveThisHit( j, adc, ampl, chip_id, fpga_id,
			     module, chan_id, fem_id, bco, bco_full, event );
	      j--;
	    }
	  
	} // for( j )

    } // for( i )
  
  if( show_details )
    {
      PrintVectorContents( *module );
    }

  cout << is_clone_exist << endl;
  return is_clone_exist;
}

/*!
  @fn void RemoveNoise
  @brief Noise-like events, which are defined inside this function, are removed.
*/
void RemoveNoise(
		 vector < int > *adc      ,
		 vector < int > *ampl     ,
		 vector < int > *chip_id  ,
		 vector < int > *fpga_id  ,
		 vector < int > *module   ,
		 vector < int > *chan_id  ,
		 vector < int > *fem_id   ,
		 vector < int > *bco      ,
		 vector < int > *bco_full ,
		 vector < int > *event    )
{

  if( show_details )
    {
      cout << "Removing noise:";
      PrintVectorContents( *module, " ", " -> " );
    }


  // loop over all INTT hits
  for( int i=0; i<adc->size(); i++ )
    {

      // make a flag to judge whether it should be removed or not
      bool is_removed = false;

      // if module of this hit is not one of modules we used, remove this hit
      if( IsOneOfModule( (*module)[i] ) == false )// module cut
	is_removed = true;
      else if( (*chip_id)[i] < 1 || 26 < (*chip_id)[i] ) // chip cut
	is_removed = true;
      else if( (*chan_id)[i] < 0 || 127 < (*chan_id)[i] ) // channel cut
	is_removed = true;
      else if( (*ampl)[i] != 0 ) // amplitude cut
	is_removed = true;
      else if( IsOneOfFemId( (*module)[i], (*fem_id)[i] ) == false )
	is_removed = true;
	        
      // if this hit is tagged for removal, remove it
      if( is_removed == true )
	{
	  if( show_details )
	    {
	      cout << "The hit to be removed: index " << i
		   << ", module " << (*module)[i]
		   << ", adc " << (*adc)[i]
		   << ", chip_id " << (*chip_id)[i]
		   << ", adc " << (*adc)[i]
		   << ", chan_id " << (*chan_id)[i]
		   << ", ampl " << (*ampl)[i]
		   << endl;
	      
	    }
	  
	  RemoveThisHit( i, adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, bco_full, event );
	  i--; // index need to be subtracted by 1 because 1 hit was removed
	}
      
    }

  if( show_details )
    {
      PrintVectorContents( *module, " ", "\n" );
    }
}

  
/*!
  @fn int tracking_sample( string data = "../data/NWU_fphx_raw_20211129-1747_0.root" )
  @brief This is the main function of this macro file.
  @details You can specify the data to be analyzed throught the 1st argument. Results are written in a PDF file, whose name is {original name}_tracking_sample.pdf, in the current directory.
  
*/
int CutData( string mode,
	     vector < int > *adc      ,
	     vector < int > *ampl     ,
	     vector < int > *chip_id  ,
	     vector < int > *fpga_id  ,
	     vector < int > *module   ,
	     vector < int > *chan_id  ,
	     vector < int > *fem_id   ,
	     vector < int > *bco      ,
	     vector < int > *bco_full ,
	     vector < int > *event
	     )
{

  if( mode == "" || mode == "h" || mode == "help" )
    {
      cerr << " int CutData( string mode = \"\",\
	     vector < int > *adc      ,\
	     vector < int > *ampl     ,\
	     vector < int > *chip_id  ,\
	     vector < int > *fpga_id  ,\
	     vector < int > *module   ,\
	     vector < int > *chan_id  ,\
	     vector < int > *fem_id   ,\
	     vector < int > *bco      ,\
	     vector < int > *bco_full ,\
	     vector < int > *event" << endl;
      cerr << " Choice of mode: none, noise, no-double, no-double-pm1bco, hit-on-all-ladder, golden, and debug" << endl;
      cerr << " here, " << endl;
      cerr << right << setw(15) << "- noise:" << "noise cut( 0<=ch<=127, 1=<chip<=26, ampl==0" << endl;
      cerr << right << setw(15) << "- no-double:" << "Events having the exactly same hits (module, chip_id, chan_id, adc, bco) are removed" << endl;
      cerr << right << setw(15) << "- no-double-pm1bco:" << "Events having the exactly same hits (module, chip_id, chan_id, adc, and bco +/- 1 ) are removed" << endl;
      cerr << right << setw(15) << "- hit-on-all-ladder:" << "If one of ladders donesn't have hit in a event, the event is removed." << endl;
      cerr << right << setw(15) << "- golden:" << "noise && no-double-pm1bco && hit-on-all-ladder" << endl;
      cerr << right << setw(15) << "- debug:" << "for debugging" << endl;
      cerr << " For example: mode = \"noise no-double\"" << endl;
      cerr << "              mode" << endl;
      return 0;
    }

  if( mode == "debug" )
    {
      //GetDataForDebug( adc, ampl, chip_id, fpga_id, module, chan_id, fem_id , bco, bco_full, event );
    }

  bool noise_cut = false, no_double_cut = false, no_double_pm1bco_cut = false, hit_on_all_ladder = false, never_no_double_cut = false;
  if( mode.find( "golden" ) != string::npos )
    {
      noise_cut = no_double_cut = no_double_pm1bco_cut = hit_on_all_ladder = true;
    }
  else
    {
      if( mode.find( "noise" ) != string::npos )
	noise_cut = true;

      if( mode.find( "no-double" ) != string::npos )
	{

	  no_double_cut = true;
	  if(  mode.find( "no-double-pm1bco" ) != string::npos )
	    no_double_pm1bco_cut = true;

	  if(  mode.find( "never-no-double" ) != string::npos )
	    never_no_double_cut = true;
	}

      if( mode.find( "hit-on-all-ladder-cut" ) != string::npos )
	hit_on_all_ladder = true;      
    }
  
  // loop over all trigger events, and process event by vent
  for( int i=0; i<adc->size(); i++ )
    {

      // remove noise from hits in this event
      if( noise_cut )
	{
	  cout << "noise cut" << endl;
	  RemoveNoise( adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, bco_full, event );
	}

      bool is_clone_exist = false;
      if( no_double_cut || no_double_pm1bco_cut )
	{
	  cout << "clone hits cut" << endl;
	  is_clone_exist = RemoveCloneHitEvent( no_double_pm1bco_cut, adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, bco_full, event );
	}

      if( is_clone_exist )
	cout << "\t\t\t" << is_clone_exist << endl;
      cout << "---> " << never_no_double_cut  << endl;
      if( is_clone_exist && never_no_double_cut )
	{
	  cout << "trigger events containing clone hits never accepted" << endl;

	  for( int i=0; i<adc->size(); i++ )
	    {
	    RemoveThisHit( i, adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, bco_full, event );
	    i--;
	    }
	}


	// if( hit_on_all_ladder )
      // 	RequireHitOnAllLadder( adc, ampl, chip_id, fpga_id, module, chan_id, fem_id, bco, 
			       // bco_full, event );
      
      // if no INTT hit was taken, skip this loop
      if( adc->size() == 0 )
	{
	  if( show_details )
	    cout << "Nothing remains after noise removal." << endl << endl;

	  //continue;
	}


    }

  return 0;
}
