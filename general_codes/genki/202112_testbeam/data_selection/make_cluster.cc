#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__

#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

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
#include "INTThit.hh"

bool debug = false;
void FillCluster(
		 TTree* tr,
		 const vector < int >& dac_config,
		 int group_id,
		 vector < int > *adc,
		 vector < int > *ampl,
		 vector < int > *chip_id,
		 vector < int > *fpga_id,
		 vector < int > *module,
		 vector < int > *chan_id,
		 vector < int > *fem_id,
		 vector < int > *bco,
		 vector < int > *bco_full,
		 
		 int trigger_id,
		 vector < int >& nhit_in_cluster,
		 vector < int >& is_over_chip,
		 vector < int > *camac_adc,
		 vector < int > *camac_tdc,
		 vector < int >& adc_clustered,
		 vector < int >& adc0_num,
		 vector < int >& adc1_num,
		 vector < int >& adc2_num,
		 vector < int >& adc3_num,
		 vector < int >& adc4_num,
		 vector < int >& adc5_num,
		 vector < int >& adc6_num,
		 vector < int >& adc7_num,
		 vector < double >& dac_clustered,
		 vector < double >& chip_id_clustered,
		 vector < double >& chip_id_clustered_simple,
		 vector < double >& chip_sigma_clustered,
		 vector < double >& chip_sigma_clustered_simple,
		 vector < int >& fpga_id_clustered,
		 vector < int >& module_clustered,
		 vector < double >& chan_id_clustered,
		 vector < double >& chan_id_clustered_simple,
		 vector < int >& chan_id_min_clustered,
		 vector < int >& chan_id_max_clustered,
		 vector < double >& chan_sigma_clustered,
		 vector < double >& chan_sigma_clustered_simple,
		 vector < int >& fem_id_clustered,
		 vector < int >& bco_clustered,
		 vector < int >& bco_full_clustered
		 )
{

  ClearVector( nhit_in_cluster );
  ClearVector( is_over_chip );
  ClearVector( adc_clustered );
  ClearVector( adc0_num );
  ClearVector( adc1_num );
  ClearVector( adc2_num );
  ClearVector( adc3_num );
  ClearVector( adc4_num );
  ClearVector( adc5_num );
  ClearVector( adc6_num );
  ClearVector( adc7_num );
  ClearVector( dac_clustered );
  ClearVector( chip_id_clustered );
  ClearVector( chip_id_clustered_simple );
  ClearVector( chip_sigma_clustered );
  ClearVector( chip_sigma_clustered_simple );
  ClearVector( fpga_id_clustered );
  ClearVector( module_clustered );
  ClearVector( chan_id_clustered );
  ClearVector( chan_id_clustered_simple );
  ClearVector( chan_id_min_clustered );
  ClearVector( chan_id_max_clustered );
  ClearVector( chan_sigma_clustered );
  ClearVector( chan_sigma_clustered_simple );
  ClearVector( fem_id_clustered );
  ClearVector( bco_clustered );
  ClearVector( bco_full_clustered );

  vector < INTThit* > hits;
  // loop over all hits to make INTThit objects
  for( int i=0; i<adc->size(); i++ )
    {
      
      INTThit* hit = new INTThit( (*adc)[i], (*ampl)[i], (*chip_id)[i],
				  (*fpga_id)[i], (*module)[i], (*chan_id)[i],
				  (*fem_id)[i], (*bco)[i], (*bco_full)[i],
				  -1,
				  dac_config );

      if( debug )
	hit->Print( 1, i==0 );
      
      // if it's noise, skip it 
      if( hit->IsNoise() )
	continue;

      // store the object
      hits.push_back( hit );
    }

  // loop over all INTThit objects to form hit clusters
  for( int i=0; i<hits.size(); i++ )
    {
      // if this hit is already added to the other hit, skip it
      if( hits[i]->IsAlreadyAdded() )
	continue;

      // loop over the remaining hit to include them to this hit
      for( int j=i+1; j<hits.size(); j++ )
	hits[i]->AddForClustering( hits[j] );
      
      // do clustering at the end of the for loop
      //      if( debug && hits[i]->Clustering() ) // if clustering is done, print the status
      if( hits[i]->Clustering() ) // if clustering is done, print the status
	if( debug )
	hits[i]->Print( 9, 1 );
    }

  if( debug )
    cout << string( 50, '-' ) << endl;
  
  // loop over cluster hits to store their info
  for( auto& hit : hits )
    {
      if( hit->IsAlreadyAdded() )
	continue;

      if( debug )
	hit->Print(1);
      
      nhit_in_cluster.push_back( hit->GetHitNumInCluster() );
      is_over_chip.push_back( hit->IsOverChip() );
      adc_clustered.push_back( hit->GetAdc() );

      adc0_num.push_back( hit->GetNhitWithAdc(0) );
      adc1_num.push_back( hit->GetNhitWithAdc(1) );
      adc2_num.push_back( hit->GetNhitWithAdc(2) );
      adc3_num.push_back( hit->GetNhitWithAdc(3) );
      adc4_num.push_back( hit->GetNhitWithAdc(4) );
      adc5_num.push_back( hit->GetNhitWithAdc(5) );
      adc6_num.push_back( hit->GetNhitWithAdc(6) );
      adc7_num.push_back( hit->GetNhitWithAdc(7) );
      
      dac_clustered.push_back( hit->GetDacCluster() );

      chip_id_clustered.push_back( hit->GetChipCol() );
      fpga_id_clustered.push_back( hit->GetFpgaId() );
      module_clustered.push_back( hit->GetModule() );

      //      cout << hit->GetChanIdCluster() << "\t" << hit->GetChanIdClusterSimple() << endl;
      chan_id_clustered.push_back( hit->GetChanIdCluster() );
      chan_id_clustered_simple.push_back( hit->GetChanIdClusterSimple() );

      chan_id_min_clustered.push_back( hit->GetChanIdMinCluster() );
      chan_id_max_clustered.push_back( hit->GetChanIdMaxCluster() );
      
      chan_sigma_clustered.push_back( hit->GetChanIdClusterSigma() );
      chan_sigma_clustered_simple.push_back( hit->GetChanIdClusterSigmaSimple() );

      fem_id_clustered.push_back( hit->GetFemId() );
      bco_clustered.push_back( hit->GetBco() );
      bco_full_clustered.push_back( hit->GetBcoFull() );
    }

  tr->Fill();

  if( debug )
    {
      string cut = to_string( trigger_id ) + "==trigger_id && " + to_string( group_id ) + "==group_id";
      tr->Scan( "trigger_id:group_id:module:chip_id:chan_id:adc", cut.c_str() );
    }
  
}

/*!
  @fn int trim_clone_hit( string data_dir = "data/ELPH/",int run_num = 89, int threshold = 5 )
  @param data_dir The path to the directory that contains the data to be used (because this information is not in the database)
  @param run_num The run number you want to process
  @param threshold The search range
  @brief The input file is divided into 2 files. One contains only the original INTT hits, and the other only the clone hits.
*/
int make_cluster( int run_num = 89, bool is_MC = false  )
{

  
  vector < int > dac_config;

  Database* db = new Database( "documents/2021_test_beam/run_list - Setup.tsv" );
  db->SetRun( run_num );
  const string kOriginal_data_dir = "data/ELPH/";
  
  if( is_MC == false )
    {
      
      if( db->IsBeamRun() == false )
	return 0;
      
      dac_config = db->GetDACConfig( kOriginal_data_dir );
      if( dac_config.size() == 0 )
	{
	  db->SetRun( 89 );
	  dac_config = db->GetDACConfig( kOriginal_data_dir );
	  db->SetRun( run_num );
	  
	}
    }
  else // for MC data
    {
      
      // use Takashi's config in any case for the moment
      db->SetRun( 89 );
      dac_config = db->GetDACConfig( kOriginal_data_dir );
      db->SetRun( run_num );
      
    }
  
  const string kOutput_root_dir = "results/ELPH/trim_data/";
  string data = kOutput_root_dir + "bco_grouping/" + "run" + to_string( run_num ) + ".root";
  if( is_MC )
    data = data.substr( 0, data.size() - 5 ) + "_MC.root";
  
  TFile* tf = new TFile( data.c_str(), "READ" );
  auto tr = (TTree*)tf->Get( "tr" );
  
  // preparation to read data from TTree event by event (event means trigger event, some (or 1 or no) INTT hits belong to the trigger event)
  int group_id = -1;
  vector < int > *adc      (0);
  vector < int > *ampl     (0);
  vector < int > *chip_id  (0);
  vector < int > *fpga_id  (0);
  vector < int > *module   (0);
  vector < int > *chan_id  (0);
  vector < int > *fem_id   (0);
  vector < int > *bco      (0);
  vector < int > *bco_full (0);

  int trigger_id = -1;
  vector < int > *camac_adc(0); // initialization by (0) is necessary
  vector < int > *camac_tdc(0);

  tr->SetBranchAddress( "group_id"	, &group_id );
  tr->SetBranchAddress( "adc"		, &adc       );
  tr->SetBranchAddress( "ampl"		, &ampl      );
  tr->SetBranchAddress( "chip_id"	, &chip_id   );
  tr->SetBranchAddress( "fpga_id"	, &fpga_id   );
  tr->SetBranchAddress( "module"	, &module    );
  tr->SetBranchAddress( "chan_id"	, &chan_id   );
  tr->SetBranchAddress( "fem_id"	, &fem_id    );
  tr->SetBranchAddress( "bco"		, &bco       );
  tr->SetBranchAddress( "bco_full"	, &bco_full  );

  tr->SetBranchAddress( "trigger_id"    , &trigger_id);
  tr->SetBranchAddress( "camac_adc"	, &camac_adc );
  tr->SetBranchAddress( "camac_tdc"	, &camac_tdc );


  // ***************************************************
  // some hits -> a hit cluster
  // - noise hits (ampl != 0, out-range value ) are removed at first.
  // - chip_id of 2 chips in the same column of hits is merged, for example, chip1 & chip14 -> chip1
  // - chan_id of 2 chips in the same column of hits is merged, for example, ch3 of chip23 -> ch(127+3+1) = ch131 of chip10
  // - dac value of a hit is calculated by the formula: dac = 4 * ADC + 210
  // For example: hit1, hit2, hit3 -> a cluster
  // The clusters parameters:
  //     - chan_id : weighted mean of hits by dac values, (ch_1 * dac_1 + ch_2 * dac_2+ ch_3 * dac_3) / 3 / (dac_1 + dac_2 + dac_3)
  //     - chan_dev: weighted standard deviation of channel of the hits by dac values,
  //     - chan_min: minimum channel of the hits, Min( ch_1, ch_2, ch_3)
  //     - chan_max: maximum channel of the hits, Max( ch_1, ch_2, ch_3)
  //     - dac     : dac value of the cluster, dac_1 + dac_2 + dac_3
  //     - dac_min : minimum dac value of the hits, Min(dac_1, dac_2, dac_3)
  //     - dac_max : maximum dac value of the hits, Max(dac_1, dac_2, dac_3)
  //     - dac_dev : standard deviation of dac value of the hits
  //     - chan_id_simple : simple mean of hits by dac values, (ch_1 + ch_2 + ch_3) / 3
  // ****************************************************************************
  const string kOutput_dir = kOutput_root_dir + "clustered/" ;
  string output = kOutput_dir + "run" + to_string( run_num ) + ".root";
  if( is_MC )
    output = output.substr( 0, output.size() - 5 ) + "_MC.root";

  TFile* tf_output = new TFile( output.c_str(), "RECREATE" );
  TTree* tr_output = new TTree( "tr", "tree for clustered INTT hits in a BCO group and their trigger information" );

  vector < double > chan_id_clustered, chip_id_clustered;
  vector < int > adc_clustered, fpga_id_clustered, module_clustered, fem_id_clustered, bco_clustered, bco_full_clustered;
  vector < int > nhit_in_cluster, is_over_chip, chan_id_min_clustered, chan_id_max_clustered;
  vector < int > adc0_num, adc1_num, adc2_num, adc3_num, adc4_num, adc5_num, adc6_num, adc7_num;

  vector < double > dac_clustered,
    chip_id_clustered_simple,
    chip_sigma_clustered, chip_sigma_clustered_simple,
    chan_id_clustered_simple,
    chan_sigma_clustered, chan_sigma_clustered_simple;

  tr_output->Branch( "trigger_id", &trigger_id, "trigger_id/I" );
  tr_output->Branch( "camac_adc",		&(*camac_adc) );
  tr_output->Branch( "camac_tdc",		&(*camac_tdc) );

  tr_output->Branch( "group_id", &group_id ); // same as BCO grouped tree
  tr_output->Branch( "nhit_in_cluster", &nhit_in_cluster );
  tr_output->Branch( "is_over_chip",    &is_over_chip );
  tr_output->Branch( "fem_id",		&fem_id_clustered );
  tr_output->Branch( "fpga_id",		&fpga_id_clustered );
  tr_output->Branch( "module",		&module_clustered );

  tr_output->Branch( "chip_id",		&chip_id_clustered );

  tr_output->Branch( "chan_id",		&chan_id_clustered );
  tr_output->Branch( "chan_id_simple",	&chan_id_clustered_simple );
  tr_output->Branch( "chan_sigma",	&chan_sigma_clustered );
  tr_output->Branch( "chan_sigma_simple",&chan_sigma_clustered_simple );
  tr_output->Branch( "chan_id_min",	&chan_id_min_clustered );
  tr_output->Branch( "chan_id_max",	&chan_id_max_clustered );
  tr_output->Branch( "adc",		&adc_clustered );
  tr_output->Branch( "adc0_num",	&adc0_num );
  tr_output->Branch( "adc1_num",	&adc1_num );
  tr_output->Branch( "adc2_num",	&adc2_num );
  tr_output->Branch( "adc3_num",	&adc3_num );
  tr_output->Branch( "adc4_num",	&adc4_num );
  tr_output->Branch( "adc5_num",	&adc5_num );
  tr_output->Branch( "adc6_num",	&adc6_num );
  tr_output->Branch( "adc7_num",	&adc7_num );
  tr_output->Branch( "dac",		&dac_clustered );
  tr_output->Branch( "bco",		&bco_clustered );
  tr_output->Branch( "bco_full",	&bco_full_clustered );


  // loop over all entries in tree_both
  for( int i=0; i<tr->GetEntries(); i++ )
    {

      tr->GetEntry( i );
      
      if( debug )
	cout << "About this BCO group: "
	     << trigger_id << "\t"
	     << group_id << "\t"
	     << adc->size()
	     << endl;
      //      else if( i / (tr->GetEntries()/20) %5 == 0 )
      else
	cout << flush << "\r\t" << setw(6) << i  << "/" << tr->GetEntries() << " ";
	
      // copy the original hits to the variables
      FillCluster( tr_output, dac_config,
		   group_id,
		   adc,       ampl,      chip_id,
		   fpga_id,   module,    chan_id,
		   fem_id,    bco,       bco_full,

		   // for output
		   trigger_id, nhit_in_cluster, is_over_chip,
		   camac_adc, camac_tdc,
		   adc_clustered,
		   adc0_num, adc1_num, adc2_num, adc3_num,
		   adc4_num, adc5_num, adc6_num, adc7_num,
		   dac_clustered,
		   chip_id_clustered, chip_id_clustered_simple,
		   chip_sigma_clustered, chip_sigma_clustered_simple,
		   fpga_id_clustered,   module_clustered,
		   chan_id_clustered, chan_id_clustered_simple,
		   chan_id_min_clustered, chan_id_max_clustered, 
		   chan_sigma_clustered, chan_sigma_clustered_simple,
		   fem_id_clustered,    bco_clustered,       bco_full_clustered );
      
      
      // for debugging
      if( debug && i > 1000)
	{

	  break;
	}

      // for debugging
      // if( adc->size() != 0 )
      // 	break;

    }
  

  stringstream comment;
  comment << "Input: " << data << endl;
  comment << "Output: " << output << endl;

  // to get the number of hits from tree_both, TTree::GetEntries() isn't good.
  auto total_hits = tr->Draw( "adc", "", "goff" );
  comment << "All INTT hits: " << total_hits << endl;

  auto cluster_hits = tr_output->Draw( "chan_id", "", "goff" );
  comment << "Clustered INTT hits w/o noise hits: " << cluster_hits << " -> "
	  << setprecision(3) << (double)cluster_hits / total_hits * 100 << "%" << endl;

  comment << "all hits - cluster hits - noise hits = "
	  << total_hits - cluster_hits 
	  << endl;

  cerr << run_num << "\t"
       << total_hits << "\t"
       << cluster_hits << "\t"
       << 100.0 * cluster_hits / total_hits
       << endl;
  

  cout << comment.str() << endl;

  tf_output->WriteTObject( tr_output, tr_output->GetName() );
  tf_output->Close();

  
  tf->Close();
  return 0;
}
