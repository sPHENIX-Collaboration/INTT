#define __CINT__

#include "Database.hh"
#include "RemoveThisHit.hh"

int hit_num_bco_rejection( string data = "data/ELPH/BeamData_20211210-0427_0.root")
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

  TH2D* hist2d = new TH2D( "hist2d", "title", 3, 1, 4, 300, 0, 300 );
  //  TH2D* hist2d_intt_event = new TH2D( "hist", "title", 3, 1, 4, 100, 0, 100 );
  TH1D* hist = new TH1D( "hist", "title", 100, 0, 100 );
  
  for( int i=0; i<all_entries; i++ )
    {
      tr_original->GetEntry( i );
      cout << i << " " << adc->size() << endl;

      if( adc->size() == 0 )
	continue;


      int first_bco = (*bco_full)[0];
      int counter[3] = { 0 };
      // loop over all INTT hits in this trigger event
      for( int j=0; j<adc->size(); j++ )
	{
	  if( abs((*bco_full)[j] - first_bco)<2 )
	    continue;

	  if( (*module)[j] == 1 )
	    counter[0]++;
	  else if( (*module)[j] == 6 )
	    counter[1]++;
	  else if( (*module)[j] == 5 )
	    counter[2]++;
	  
	}

      hist2d->Fill( 1, counter[0] );
      hist2d->Fill( 2, counter[1] );
      hist2d->Fill( 3, counter[2] );

      vector < int > bco_group;
      for( int j=0; j<adc->size(); j++ )
	{
	  if( abs((*bco_full)[j] - first_bco)<2 )
	    continue;

	  if( (*module)[j] == 1 )
	    bco_group.push_back( (*bco_full)[j] );
	}

      sort( bco_group.begin(), bco_group.end() );
      // for( auto& it : bco_group )
      // 	cout << setw(8) << it << " ";
      // cout << endl;
      
      bco_group.erase( unique( bco_group.begin(), bco_group.end() ), bco_group.end() );
      // for( auto& it : bco_group )
      // 	cout << setw(8) << it << " ";
      // cout << endl;

      hist->Fill( bco_group.size() );
    } // for i

  //hist2d->Draw( "colz" );
  hist->Draw();
  
  return 0;
}
