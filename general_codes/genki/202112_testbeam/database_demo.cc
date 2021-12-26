/*!
  @file database_demo.cc
  @brief A simple introduction how to use Database class
  @details This macro shows something like
  @code [genki 16:30:34 INTT] $ root -q -b macro/ELPH/database_demo.cc
   ------------------------------------------------------------------
  | Welcome to ROOT 6.24/06                        https://root.cern |
  | (c) 1995-2021, The ROOT Team; conception: R. Brun, F. Rademakers |
  | Built for linuxx8664gcc on Sep 02 2021, 14:20:23                 |
  | From tags/v6-24-06@v6-24-06                                      |
  | With                                                             |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q'       |
   ------------------------------------------------------------------


Processing macro/ELPH/database_demo.cc...
Run21 fphx_raw_20211207-0917_0.root        modules(6 1 5 ), roc_ports(C2 A2 C3 A1 ), ladders(Taiwan PPB2-L5 PPB2-L6 PPB2-L4 ), BEX(- - - - ),
                beam position: 0
                CAMAC ADC(sci_top,-,-,-,-,),
                CAMAC TDC(sci_top?,-,-,BCO,-,-,),

Run22 BeamData_20211207-1156_0.root        modules(1 6 5 ), roc_ports(A2 C3 C2 ), ladders(PPB2-L5 PPB2-L6 PPB2-L4 ), BEX(- - - ),
                beam position: 3
                CAMAC ADC(sci_top,?,?,?,-,),
                CAMAC TDC(?,?,?,BCO,sci_small,?,),
... (omitted) ...
Run21 data/ELPH/fphx_raw_20211207-0917_0.root          585666      module-6:     272096 module-1:     163108 module-5:     149950
Run22 data/ELPH/BeamData_20211207-1156_0.root         3310908      module-1:    1044125 module-6:    1272349 module-5:     992024
Run23 data/ELPH/BeamData_20211207-1229_0.root         3189858      module-1:     986192 module-6:    1207813 module-5:     948165
... (omitted) ...
Run87 data/ELPH/BeamData_20211210-1941_0.root         1275810      module-1:     424445 module-6:     431759 module-5:     419178
Run88 data/ELPH/BeamData_20211210-2018_0.root          827469      module-1:     210754 module-6:     219144 module-5:     216594
(int) 0

@endcode
 */

// The header file should be included
#include "Database.hh"

int database_demo()
{

  // a path to the tsv file to be used
  string db_file = "documents/2021_test_beam/run_list - Setup.tsv";

  // make an instance os the Database class, the path to the tsv file shold be given as a 1st argument
  Database* db = new Database( db_file );

  // show all information
  //db->Print();

  int first_run = 21; // this is the first run with the beam
  int last_run = db->GetNumberOfRuns(); // you can get the last run number with this function

  ///////////////////////////////////////////////////////////////////////////////
  // Section for accessing infomation in the run list ///////////////////////////
  ///////////////////////////////////////////////////////////////////////////////  
  // loop over runs, which starts from the first run and ends with the last run
  for( int run=first_run ; run<last_run; run++ )
    {
      // recognize the current run to the Database class
      db->SetRun( run );
      
      string root_file = db->GetRootFile();
      string source = db->GetSource(); // it can be beam, source, calibration

      // for example, if you want to analyze only beam runs, do it
      if( !(db->IsBeamRun()) )
	continue;

      // You can get the timing when this run was started
      TDatime timing = db->GetDatime();

      // You can select runs with CAMAC by looking DAQ mode variable
      // string daq_mode = db->GetDAQMode(); // it's not important because all beam runs were operated with the CAMAC DAQ
      
      cout << "Run" << run << " " << root_file << "\t";

      // You can get module numbers. Upstream module comes first.
      vector < int > modules = db->GetModules();
      cout << "modules(";

      // loop over all modules to print them
      for( int i=0; i<modules.size(); i++ )
	{
	  cout << modules[i] << " ";
	}
      cout << "), ";

      // You can get roc_ports used in this run by the runction "GetRoc_PortsInUse"
      vector < string > roc_ports = db->GetRocPortsInUse();
      cout << "roc_ports(";
      for( int i=0; i<roc_ports.size(); i++ )
	{
	  cout << roc_ports[i] << " ";
	}
      cout << "), ";

      // You can get ladders used in this run by the runction "GetLaddersInUse"
      vector < string > ladders = db->GetLaddersInUse();
      cout << "ladders(";
      for( int i=0; i<ladders.size(); i++ )
	{
	  cout << ladders[i] << " ";
	}
      cout << "), ";

      // You can get bus extenders used in this run by the following function
      vector < string > bus_extenders = db->GetBusExtendersInUse();
      cout << "BEX(";
      for( auto& bex : bus_extenders )
	{
	  cout << bex << " ";
	}
      cout << "), ";

      cout << endl << "\t\t";
      double beam_position = db->GetBeamPosition();
      cout << "beam position: " << beam_position << " cm";
      
      cout << endl << "\t\t";
      vector < string > camac_adcs = db->GetCamacAdcs();
      cout << "CAMAC ADC(";
      for( int i=0; i<camac_adcs.size(); i++ )
	{
	  cout << camac_adcs[i] << ",";
	}
      cout << "), ";

      cout << endl << "\t\t";
      vector < string > camac_tdcs = db->GetCamacTdcs();
      cout << "CAMAC TDC(";
      for( int i=0; i<camac_tdcs.size(); i++ )
	{
	  cout << camac_tdcs[i] << ",";
	}
      cout << "), " << endl;
      cout << endl;
    }

  return 0;
  
  ///////////////////////////////////////////////////////////////////////////////
  // Section for data analysis //////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // you need to set the path to the directory which contains ROOT file
  string data_directory = "data/ELPH/";
  
  // loop over runs, which starts from the first run and ends with the last run
  for( int run=first_run ; run<last_run; run++ )
    {
      // recognize the current run to the Database class
      db->SetRun( run );
      
      string source = db->GetSource(); // it can be beam, source, calibration
      // for example, if you want to analyze only beam runs, do it
      if( !(db->IsBeamRun()) )
	continue;
            
      string root_file = db->GetRootFile();
      string data_path = data_directory + root_file;
      cout << "Run" << run << " " << data_path << "\t";

      // open the ROOT file in READ mode. You need to pass char* to the 1st argument as the data path. To convert string to char*, use c_str() function.
      TFile* tf = new TFile( data_path.c_str(), "READ" );

      // if the ROOT file cannot be opened, the instance is nullprt
      if( tf == nullptr )
	{
	  cerr << data_path << " cannot be opened." << endl;
	  cerr << "Check whether the file exists or not." << endl;
	  cerr << "This run is skipped." << endl;
	  continue;
	}

      // get a TTree
      TTree* tree = (TTree*)tf->Get( "tree" );
      cout << setw(10) << tree->GetEntries() << "\t";

      // make a general cut
      TCut cut_general( "ampl==0 && 0<chip_id && chip_id<27 && 0<=chan_id && chan_id<128" );
      
      // You can get module numbers. Upstream module comes first.
      vector < int > modules = db->GetModules();

      // loop over modules to see the number of hit 
      for( int i=0; i<modules.size(); i++ )
	{
	  cout << "module-" << modules[i] << ": ";

	  // cut to select module
	  TCut cut_module( Form( "module==%d", modules[i]) );

	  // make all cuts into one
	  TCut cut = cut_general + cut_module;

	  // get the number of hit on this module and print it
	  cout << setw(10) << tree->GetEntries( cut ) << " ";
	}
      cout << endl;

      // close ROOT file
      tf->Close();
    }
  
  return 0;
}
