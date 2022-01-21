#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

//#include <>
#include "Database.hh"

int thre = 1;
vector < int > GetCloneIndex(
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
  vector < int > index;

  // loop over all hits
  for( int i=0; i<adc->size(); i++ )
    {
      bool flag_clone_found = false;
      
      // loop over other hits to compare hit-i and hit-j
      for( int j=i+1; j<adc->size(); j++ )
	{
	  int bco_diff = (*bco_full)[i] - (*bco_full)[j];

	  // if BCOs are different a lot, skip this
	  //if( abs(bco_diff) >= 13 )
	  if( abs(bco_diff) >= thre )
	    continue;

	  if( (*adc)[i] != (*adc)[j] )
	    continue;
	  
	  if( (*ampl)[i] != (*ampl)[j] )
	    continue;
	  
	  if( (*chip_id)[i] != (*chip_id)[j] )
	    continue;
	  
	  if( (*fpga_id)[i] != (*fpga_id)[j] )
	    continue;
	  
	  if( (*module)[i] != (*module)[j] )
	    continue;
	  
	  if( (*chan_id)[i] != (*chan_id)[j] )
	    continue;
	  
	  if( (*fem_id)[i] != (*fem_id)[j] )
	    continue;


	  //	  if( abs(bco_diff) == 2 || j == 132 )
	  if( false )
	    {
	      cout << endl;
	  cout << i << " - " << j << "\t" << bco_diff << endl;
	  cout << "\e[0;32m" << (*adc)[i] << " "
	       << (*ampl)[i] << " "
	       << (*chip_id)[i] << " "
	       << (*fpga_id)[i] << " "
	       << (*module)[i] << " "
	       << (*chan_id)[i] << " "
	       << (*fem_id)[i] << " "
	       << (*bco)[i] << " "
	       << (*bco_full)[i] << " "
	       << "\e[0;37m" << endl;

	  cout << "\e[0;33m" << (*adc)[j] << " "
	       << (*ampl)[j] << " "
	       << (*chip_id)[j] << " "
	       << (*fpga_id)[j] << " "
	       << (*module)[j] << " "
	       << (*chan_id)[j] << " "
	       << (*fem_id)[j] << " "
	       << (*bco)[j] << " "
	       << (*bco_full)[j] << " "
	       << "\e[0;37m" << endl;
	    }
	  
	  index.push_back( j );
	  break;
	}

    }

  // keep unique indices, multiple assignment may be happen for multiple (2<) clone
  sort( index.begin(), index.end() );
  index.erase( unique( index.begin(), index.end()), index.end() );
  
  return index;
}


void CopyData(
	      vector < int >& indices_ref,
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
	      
	      vector < int >& adc_dest,
	      vector < int >& ampl_dest,
	      vector < int >& chip_id_dest,
	      vector < int >& fpga_id_dest,
	      vector < int >& module_dest,
	      vector < int >& chan_id_dest,
	      vector < int >& fem_id_dest,
	      vector < int >& bco_dest,
	      vector < int >& bco_full_dest,
	      vector < int >& event_dest,
	      bool is_removed )
{

  adc_dest.erase( adc_dest.begin(), adc_dest.end() );
  ampl_dest.erase( ampl_dest.begin(), ampl_dest.end() );
  chip_id_dest.erase( chip_id_dest.begin(), chip_id_dest.end() );
  fpga_id_dest.erase( fpga_id_dest.begin(), fpga_id_dest.end() );
  module_dest.erase( module_dest.begin(), module_dest.end() );
  chan_id_dest.erase( chan_id_dest.begin(), chan_id_dest.end() );
  fem_id_dest.erase( fem_id_dest.begin(), fem_id_dest.end() );
  bco_dest.erase( bco_dest.begin(), bco_dest.end() );
  bco_full_dest.erase( bco_full_dest.begin(), bco_full_dest.end() );
  event_dest.erase( event_dest.begin(), event_dest.end() );
  
  vector < int > indices;

  if( is_removed == false ) // in the case to select clone hits
    {
      indices = indices_ref;
    }
  else
    {

      for( int i=0; i<adc->size(); i++ )
	if( find( indices_ref.begin(), indices_ref.end(), i ) == indices_ref.end() )
	  indices.push_back( i );
      
    }


  // for( auto& index : indices )
  //   cout << index << endl;
  // cout << "total: " << indices.size() << endl;
  
  for( auto& index : indices )
    {
      //cout << index << "\t" << (*adc)[index] << endl;
      adc_dest.push_back( (*adc)[ index ] );
      ampl_dest.push_back( (*ampl)[ index ] );
      chip_id_dest.push_back( (*chip_id)[ index ] );
      fpga_id_dest.push_back( (*fpga_id)[ index ] );
      module_dest.push_back( (*module)[ index ] );
      chan_id_dest.push_back( (*chan_id)[ index ] );
      fem_id_dest.push_back( (*fem_id)[ index ] );
      bco_dest.push_back( (*bco)[ index ] );
      bco_full_dest.push_back( (*bco_full)[ index ] );
      event_dest.push_back( (*event)[ index ] );
      
    }  
}
		   
int trim_clone_hit( string data_dir = "data/ELPH/", int run_num = 89, int threshold = 2)
{
  thre = threshold;
  
  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run_num );

  if( db->IsBeamRun() == false )
    return -1;
  
  string data = data_dir + db->GetRootFile();
  
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
  bool INTT_event; // it's not so important...

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
  tr->SetBranchAddress( "INTT_event"	, &INTT_event );

  //string output_dir = "results/ELPH/trim_data/no_clone/" + to_string( thre );
  string output_dir = "results/ELPH/trim_data/no_clone/";
  string output_no_clone = output_dir + "run" + to_string( run_num ) + "_no_clone.root";

  TFile* tf_no_clone = new TFile( output_no_clone.c_str(), "RECREATE" );
  TTree* tr_no_clone = new TTree( "tree_both", "tree for CAMAC data and associated INTT hits without clone hits" );
  
  vector < int > adc_no_clone, ampl_no_clone, chip_id_no_clone, fpga_id_no_clone, module_no_clone, chan_id_no_clone, fem_id_no_clone, bco_no_clone, bco_full_no_clone, event_no_clone;
  tr_no_clone->Branch( "camac_adc",		&(*camac_adc) );
  tr_no_clone->Branch( "camac_tdc",		&(*camac_tdc) );
  tr_no_clone->Branch( "INTT_event",		&INTT_event, "INTT_event/O" );
  tr_no_clone->Branch( "adc",			&adc_no_clone );
  tr_no_clone->Branch( "ampl",			&ampl_no_clone );
  tr_no_clone->Branch( "chip_id",		&chip_id_no_clone );
  tr_no_clone->Branch( "fpga_id",		&fpga_id_no_clone );
  tr_no_clone->Branch( "module",		&module_no_clone );
  tr_no_clone->Branch( "chan_id",		&chan_id_no_clone );
  tr_no_clone->Branch( "fem_id",		&fem_id_no_clone );
  tr_no_clone->Branch( "bco",			&bco_no_clone );
  tr_no_clone->Branch( "bco_full",		&bco_full_no_clone );
  tr_no_clone->Branch( "event",			&event_no_clone );
  
  string output_clone = output_dir + "run" + to_string( run_num ) + "_clone.root";
  TFile* tf_clone = new TFile( output_clone.c_str(), "RECREATE" );

  TTree* tr_clone = new TTree( "tree_both", "tree for CAMAC data and associated INTT hits (only clone hits)" );
  vector < int > adc_clone, ampl_clone, chip_id_clone, fpga_id_clone, module_clone, chan_id_clone, fem_id_clone, bco_clone, bco_full_clone, event_clone;
  tr_clone->Branch( "camac_adc",	&(*camac_adc) );
  tr_clone->Branch( "camac_tdc",	&(*camac_tdc) );
  tr_clone->Branch( "INTT_event",	&INTT_event, "INTT_event/O" );
  tr_clone->Branch( "adc",		&adc_clone );
  tr_clone->Branch( "ampl",		&ampl_clone );
  tr_clone->Branch( "chip_id",		&chip_id_clone );
  tr_clone->Branch( "fpga_id",		&fpga_id_clone );
  tr_clone->Branch( "module",		&module_clone );
  tr_clone->Branch( "chan_id",		&chan_id_clone );
  tr_clone->Branch( "fem_id",		&fem_id_clone );
  tr_clone->Branch( "bco",		&bco_clone );
  tr_clone->Branch( "bco_full",		&bco_full_clone );
  tr_clone->Branch( "event",		&event_clone );

  for( int i=0; i<tr->GetEntries(); i++ )
    {

      tr->GetEntry( i );
      //cout << flush << "\r\t" << setw(6) << i  << "/" << tr->GetEntries() << " ";

      vector < int > clone_index = GetCloneIndex( adc,       ampl,      chip_id,
						  fpga_id,   module,    chan_id,
						  fem_id,    bco,       bco_full,
						  event );
      
      CopyData( clone_index,
		adc,       ampl,      chip_id,
		fpga_id,   module,    chan_id,
		fem_id,    bco,       bco_full,	event,
		adc_no_clone,       ampl_no_clone,      chip_id_no_clone,
		fpga_id_no_clone,   module_no_clone,    chan_id_no_clone,
		fem_id_no_clone,    bco_no_clone,       bco_full_no_clone,	event_no_clone,
		true );      
      
      
      CopyData( clone_index,
		adc,       ampl,      chip_id,
		fpga_id,   module,    chan_id,
		fem_id,    bco,       bco_full,	event,
		adc_clone,       ampl_clone,      chip_id_clone,
		fpga_id_clone,   module_clone,    chan_id_clone,
		fem_id_clone,    bco_clone,       bco_full_clone,	event_clone,
		false );
      
      tr_no_clone->Fill();
      tr_clone->Fill();

      //      if( adc->size() - adc_no_clone.size() - adc_clone.size() != 0 )
      if( false )
	{
	  cout << endl;
	  cout << adc->size() << " - (" <<  adc_no_clone.size() << " + " << adc_clone.size() << ") = "
	    //<< adc->size() - adc_no_clone.size() - adc_clone.size()
	       <<  adc_no_clone.size() + adc_clone.size() - adc->size()
	       << endl << endl	       << endl << endl;


	  for( auto& index : clone_index )
	    cout << index << endl;
	  
	}
      
      // if( adc->size() != 0 )
      // 	break;
    }
  

  // tr->Scan( "adc:chip_id:chan_id" );
  // tr_no_clone->Scan( "adc:chip_id:chan_id" );


  stringstream comment;
  comment << "Threshold: " << threshold << endl;
  comment << "Input: " << data << endl;
  comment << "Output: " << output_no_clone << endl;
  comment << "Output: " << output_clone << endl;

  //  string cut = string("Entry$<=") + to_string(end_at);
  //auto total_hits = tr->Draw( "adc", cut.c_str(), "goff" );
  auto total_hits = tr->Draw( "adc", "", "goff" );
  comment << "Total INTT hits: " << total_hits << endl;

  auto original_hits = tr_no_clone->Draw( "adc", "", "goff" );
  comment << "Total INTT hits w/o clone: " << original_hits << " -> "
	  << setprecision(3) << (double)original_hits / total_hits * 100 << "%" << endl;

  auto clone_hits = tr_clone->Draw( "adc", "", "goff" );
  // comment << "Total clone INTT hits: " << clone_hits << " -> " 
  // 	  << setprecision(3) << (double)clone_hits / total_hits * 100 << "%" << endl;
  
  comment << "total - (original + clone) = "
	  << total_hits - (original_hits + clone_hits)
	  << endl;


  cerr << run_num << "\t"
       << threshold << "\t"
       << total_hits << "\t"
       << original_hits << "\t"
       << 100.0 * original_hits / total_hits
       << endl;
  
  cout << comment.str() << endl;
  tf->Close();
  tf_no_clone->WriteTObject( tr_no_clone, tr_no_clone->GetName() );
  tf_no_clone->Close();
  
  tf_clone->WriteTObject( tr_clone, tr_clone->GetName() );
  tf_clone->Close();
  return 0;
}
