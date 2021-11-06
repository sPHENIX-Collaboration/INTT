/*!
  @file check_chip_prototypeMaximum7.c
  @date Nov/14/2020
  @author G. Nukazuka
  @brief A TTree is made using data in .dat file and saved to a ROOT file. Some plots are made to have a look.
  @details INTT data with camac system for the external trigger. If no camac data, use other version.
*/

#include <iomanip>
#include "functions/getFileSize.hh"
#include "functions/DrawPlots.hh"
#include "functions/FindLatestFile.hh"
#include "functions/INTTHit.hh"

#include "functions/DrawHitMap.c"


// Data in a .dat file is decode and filled to a TTree. A path to the ROOT file is returned. If error occured, "" is returned.
// string MakeTree(string fname, int usemod = 3, int maxbuf = 0, int n_meas = 64, float maxscale = 200., bool decoded_output = false);
//string MakeTree(string fname, int usemod = 3, int maxbuf = 0, bool decoded_output = false);
string MakeTree(string fname, int usemod = 3, string mode = "calib", string cut = "", bool decoded_output = false);

void MakeCluster( vector < int >& adcs,
		  vector < float >& adc_voltages,
		  vector < int >& ampls,
		  vector < int >& chip_ids,
		  vector < int >& fpga_ids,
		  vector < int >& modules,
		  vector < int >& chan_ids,
		  vector < int >& fem_ids,
		  vector < int >& bcos,
		  vector < int >& bco_fulls,
		  vector < int >& events,
		  vector < int >& nhits_in_cluster,
		  int module );
		   

void ShowMessage();

/*!
  @fn int GetModules
  @brief Module numbers in the argument are extracted and returned as a vector < int >
  @param usemod module numbers sparated by commas and/or spaces
  @details
*/
vector < int > GetModules( string usemod )
{
  vector < int > modules;

  // all commas are replaced with a space
  while( usemod.find( "," ) != string::npos )
    {
      int pos_comma = usemod.find( "," );
      usemod = usemod.substr(0, pos_comma ) + " " + usemod.substr( pos_comma + 1, usemod.size() );

    }

  stringstream ss( usemod );
  int module;
  while( ss >> module )
    modules.push_back( module );

  cout << " Modules to be used: ";
  for( int i=0; i<modules.size(); i++ )
    cout << modules[i] << " ";

  cout << endl;
  return modules;
}


/*!
  @fn int check_chip_prototypeMaximam7
  @brief 
  @param fname A name of dat file OR a path to the directory. 
  @param usemod The module ID
  @param mode Mode of data taking, "calib", "external", "camac", "camac_clustering" are acceped
  @details The feature of this version is
  - The latest file in a directory can be selected automatically.
  If the given string end with ".dat", it's treated as a .dat file and processed.
  If not, the string is treated as a path to a directory. The latest dat file in the directory is found and used.
  A path to directory should end with "\".
  - It works depending on mode. 
  "calib": for calibration data. Only TTree for INTT event is made. Plots for calibration data are made.
  "external": for data with external trigger. Only TTree for INTT event is made. Plots for the external trigger mod are mode.
  "camac": for data in external trigger mode together with CAMAC DAQ. Tree for INTT event, CAMAC event, and CAMAC-wise tree are made. Plots for this mode are made.
  "camac_clustering": for data internal trigger mode toghter with CAMAC DAQ. Tree for INTT event, CAMAC event, and CAMAC-wise tree, here INTT events are clustered, are made. Event cuts on both INTT and CAMAC events for clustering are needed. Plots for that are made.
*/
int check_chip_prototypeMaximum7
(
 string fname,
 string usemod,
 string mode,
 string cut = ""
 )
{
  ShowMessage();

  vector < int > modules = GetModules( usemod );
  
  if (mode != "calib" && mode != "external" && mode != "camac" && mode != "camac_clustering")
    {
      ShowMessage();
      cout << " Given mode\"" << mode << "\" is not supported." << endl;
      return 0;
    }
  // read the data file, decode the data, fill data to TTree, and save the TTree into a ROOT file
  bool decoded_out = false;
  //decoded_out = true;
  
  // for example: data/INTT_test_bench/camac_integration/nwu_fphx_raw_20201113-0137_0.dat -> nwu_fphx_raw_20201113-0137_0.dat
  string file_name = fname.substr( fname.find_last_of( "/" )+1, fname.size()-fname.find_last_of( "/" ) );

  // for example: nwu_fphx_raw_20201113-0137_0.dat -> dat
  string file_suffix = file_name.substr( file_name.find_last_of( "." ) + 1 , file_name.size() - file_name.find_last_of( "." ) );

  // if given thing is not a dat file, use the latest dat file in the directory
  if( file_suffix != "dat" )
    fname = FindLatestFile( fname.substr( 0, fname.find_last_of("/") + 1 ) );

  //const string root_file = MakeTree(fname, usemod, maxbuf, n_meas, maxscale, decoded_out);
  string root_file = MakeTree(fname, usemod[0], mode, cut, decoded_out);
  //return 0;
  cout << " - " << root_file << " generated" << endl;
  
  // If there was no error in MakeTree, draw some plots!
  if (root_file != "")
    for( int i=0; i<modules.size(); i++ )
      DrawPlots(root_file, modules[i], mode);
  
  gStyle->SetOptStat();
  gStyle->SetOptFit();
  return 0;
}

/*!
  @fn int check_chip_prototypeMaximum7( string fname,  int usemod,  string mode, string cut )
  @brief checking functions with the same arguments as before. It's for backword compatibility
*/
int check_chip_prototypeMaximum7( string fname,  int usemod,  string mode, string cut )
{
  return check_chip_prototypeMaximum7( fname, to_string(usemod), mode, cut );
}

/*
  @fn string MakeTree(const char* fname,int usemod,int maxbuf,int n_meas,float maxscale)
  @brief TTree fron data in the data file is saved in a ROOT file
  @param const char* fname
  //	@param int usemod
  //	@param int maxbuf
  //      @param int n_meas
  @param float maxscale
  @retval A path to the ROOT file or "" in the case of an error
  @details Some unused parameters and etc. are remained for the moment.
*/
//string MakeTree(string fname, int usemod, int maxbuf, int n_meas, float maxscale, bool decoded_output)
string MakeTree(string fname, int usemod, string mode, string cut, bool decoded_output)
{

  int maxbuf = 0; // no need to take argument, I think
  bool check_sum_val = false;

  // constant parameters for the setup: a number of channels, chips
  const int NCHAN = 128;
  const int NCHIPS = 26;

  // make a file name to save
  string output = fname; // conversion fron const char* to string to use string::substr and string::find_last_of
  output = output.substr(0, output.find_last_of(".dat") - 3) + ".root"; // it means, for example, nwu_fphx_20200101_000000_0.dat -> nwu_fphx_20200101_000000_0 + .root

  if( mode == "camac_clustering" )
    output = output.substr(0, output.find_last_of(".root") - 4) + "_clustered.root"; // it means, for example, nwu_fphx_20200101_000000_0.dat -> nwu_fphx_20200101_000000_0 + .root
    
  cout << "filename   = " << output << endl;

  // save TTree into a root file
  const char* filename = output.c_str();
  TFile *file = new TFile(filename, "RECREATE");

  string output_decoded = fname; // conversion fron const char* to string to use string::substr and string::find_last_of
  output_decoded = output.substr(0, output.find_last_of(".dat") - 4) + "_decoded.dat";
  ofstream ofs;

  if (decoded_output) {
    cout << "decoded output file = " << output_decoded << endl;
    ofs = ofstream(output_decoded);
  }

  // return output; // quick solution to get the path to the output file without decoding, it means the ROOT file already exists 

  //--------------------------------------------------------------------------------------------------
  // made TTree to be filled with data
  UInt_t command;
  int bco, adc, ampl, col_raw, col, chip_id, fpga_id, module, mchip;
  int chan_id;
  int nhit_event, nchip_event, fem_id;
  int chiphit[8];
  int bco_full, old_bco_full, old_bco;
  int ievent = 0, event;
  
  vector < int >  adcs, ampls, chip_ids, fpga_ids, modules, chan_ids, fem_ids, bcos,  bco_fulls, events, nhits_in_cluster;
  vector < float > adc_voltages;
  bool is_INTT = true, is_single_bco_event = false;
  
  vector < int > camac_adcs, camac_tdcs;

  // made branches for each column
  TTree *tree = new TTree("tree", "chip info");
  tree->Branch("adc", &adc, "adc/I");
  tree->Branch("ampl", &ampl, "ampl/I");
  tree->Branch("chip_id", &chip_id, "chip_id/I");
  tree->Branch("fpga_id", &fpga_id, "fpga_id/I");
  tree->Branch("module", &module, "module/I");
  tree->Branch("chan_id", &chan_id, "chan_id/I");
  tree->Branch("fem_id", &fem_id, "fem_id/I");
  tree->Branch("bco", &bco, "bco/I");
  tree->Branch("bco_full", &bco_full, "bco_full/I");
  tree->Branch("event", &ievent, "event/I");
  tree->Branch("command", &command, "command/i" );
  
  //--------------------------------------------------------------------------------------------------
  TTree* tree_camac = new TTree( "tree_camac", "CAMAC data" );
  tree_camac->Branch("camac_adc", &camac_adcs);
  tree_camac->Branch("camac_tdc", &camac_tdcs);
  tree_camac->Branch("INTT_event", &is_INTT, "is_INTT/O");
	

  //--------------------------------------------------------------------------------------------------
  TTree* tree_both = new TTree( "tree_both", "CAMAC-wise TTree for CAMAC events and INTT events" );
  tree_both->Branch("camac_adc", &camac_adcs);
  tree_both->Branch("camac_tdc", &camac_tdcs);
  tree_both->Branch("INTT_event", &is_INTT, "is_INTT/O");

  if( mode == "camac" || mode == "camac_clustering" ){

    if( mode == "camac_clustering" )
      {
	tree_both->Branch("nhit_in_cluster", &nhits_in_cluster );
      }

    tree_both->Branch("adc", &adcs );
    tree_both->Branch("adc_vol", &adc_voltages);
    tree_both->Branch("ampl", &ampls );
    tree_both->Branch("chip_id", &chip_ids );
    tree_both->Branch("fpga_id", &fpga_ids );
    tree_both->Branch("module", &modules );
    tree_both->Branch("chan_id", &chan_ids );
    tree_both->Branch("fem_id", &fem_ids );
    tree_both->Branch("bco", &bcos );
    tree_both->Branch("bco_full", &bco_fulls );
    tree_both->Branch("single_bco_event", &is_single_bco_event, "single_bco_event/O" );
    tree_both->Branch("event", &events );
  }
  
	
  //--------------------------------------------------------------------------------------------------

  // open the data file
  std::ifstream f(fname.c_str(), std::ifstream::binary);

  // quit this function if the data file cannot be opened
  if (!f.is_open()) {
    std::cerr << "Failed to open input file " << fname << std::endl;
    return "";
  }

  // get size of file
  int size = getFileSize(f);
  std::cout << "Input file " << fname << std::endl;
  std::cout << "Number of bytes in file = " << size << std::endl;

  if (size == 0) {
    cerr << "size is 0. return" << endl;
    return "";
  }

  // Brute force approach: allocate a single array to hold the entire
  // contents of the file
  UInt_t* data = new UInt_t[size / sizeof(UInt_t)];

  // read the data file and put all data into the array UInt_t* data
  f.read((char*)data, size);

  // if reading data failed, quit
  if (f.fail()) {
    std::cerr << "eof, fail, or bad condition during read" << std::endl;
    delete[] data;
    return "";
  }

  // close the data file since it's no longer needed
  f.close();

  // Counter to count the number of hits for each channel and amplitude, not used, why?
  int nhits[128][128] = { { 0 } };

  // Extract bco, channel, amplitude, and adc from each hit, and count the number of
  // hits at each amplitude for each channel:

  //  int ievent           = 0;
  int bufcnt = 0;
  int event_fem = -999, old_event_fem = -999;
  int inoise = 0, ihealthy = 0;

  int clock_data_num = 0;

  int counter = 0;
  //--------------------------------------------------------------------------------------------------
  // loop over all data
  for (int index = 0; index < size / sizeof(int); bufcnt++) { // note: value of index is changed not only here

    // init variables for branches
    command = 0;
    adc = ampl = chip_id = fpga_id = module = chan_id = bco = bco_full = event = -1;
    is_INTT = true;
    is_single_bco_event = false;

    // erase elements in vectors
    camac_adcs.erase	( camac_adcs.begin()		, camac_adcs.end()		);
    camac_tdcs.erase	( camac_tdcs.begin()		, camac_tdcs.end()		);
    bcos.erase		( bcos.begin()			, bcos.end()			);
    adcs.erase		( adcs.begin()			, adcs.end()			);
    adc_voltages.erase  ( adc_voltages.begin()		, adc_voltages.end()		);
    ampls.erase		( ampls.begin()			, ampls.end()			);
    chip_ids.erase	( chip_ids.begin()		, chip_ids.end()		);
    fpga_ids.erase	( fpga_ids.begin()		, fpga_ids.end()		);
    modules.erase	( modules.begin()		, modules.end()			);
    fem_ids.erase	( fem_ids.begin()		, fem_ids.end()			);
    chan_ids.erase	( chan_ids.begin()		, chan_ids.end()		);
    bco_fulls.erase	( bco_fulls.begin()		, bco_fulls.end()		);
    events.erase	( events.begin()		, events.end()			);
    nhits_in_cluster.erase(nhits_in_cluster.begin()	, nhits_in_cluster.end()	);
    
    if (maxbuf && bufcnt >= maxbuf)
      break;

#ifdef OLD
    // Get record size
    int start = index;
    int cnt = data[start];
    // Unpack the record
    int word1 = data[start + 1];
    int word2 = data[start + 2];
#else
    // here is used now
    int buflen = data[index];
    int bufid = data[index + 1];
    int cnt = buflen - 1;
    int start = index + 2;

    // Fake the old fmt logic
    int word1 = 0xFFFFFFFF;
    int word2 = 0xFFFFFFFF;

    if (bufid == 100) {
      word1 = 100;
    }
    else if (bufid == 101) {
      word1 = 0xFFFFFFFF;
      word2 = 0xFFFFFFFF;
    }
    else if (bufid == 102)
      {
	word1 = 102;
      }

    // if decoded output is required, write data
    if (decoded_output) {
      /*
	for (int j = 0; j < 3; j++) {
	ofs << setw(11) << data[index + j] << " ";
	cout << data[index + j] << " ";
	}*/
      ofs << endl; // break line 
      /*
	cout << data[index + 1] << " "
	<< data[index + buflen - 2] << " "
	<< data[index + buflen - 1] << " "
	<< endl;
      */
    }

#endif

    if (word1 == 0xFFFFFFFF && word2 == 0xFFFFFFFF) {
      if (bufid == 101) {
	// This record is a time stamp
	// Format of timestamp record is
	//   data[0] = word count, excluding this word and checksum
	//   data[1] = 0xFFFFFFFF
	//   data[2] = 0xFFFFFFFF
	//   data[3] = time in clock ticks for cpu
	//   data[4] = CLOCKS_PER_SEC taken from the system value
	//   data[5] = checksum for buffer
	//std::cout << "Buffer " << bufcnt << ": Timestamp" << std::endl;

#ifdef OLD
	index = start + cnt + 2;
#else
	index = start + cnt;
#endif
	//index = start + cnt + 2;
	continue;

      }

    } // end of if(word1==0xFFFFFFFF && word2==0xFFFFFFFF){
    else if (word1 == 100) {
      // This buffer is a configuration record
      std::cout << "Buffer " << bufcnt << ": Configuration " << std::endl;

      index += 2;
      int runno = data[index++];

      // print the run number
      std::cout << "    Run " << runno << std::endl;

      unsigned short* p = (unsigned short*)&data[index];

      int n = 0;
      for (n = 0; n < 9; ) {
	unsigned short chipid = p[n++];
	unsigned short masks[8];

	for (int m = 0; m < 8; m++, n++)
	  masks[m] = p[n];

	std::cout << "    Chip " << chipid << std::endl;
	std::cout << "      enable masks ";

	for (int m = 0; m < 8; m++)
	  std::cout << "0x" << std::hex << masks[m] << " ";

	std::cout << std::dec << std::endl;
      }

      unsigned short* p2 = &p[n];
      for (n = 0; n < 16; n++) {
	std::cout << "      Reg " << n << ": " << (int)p2[n] << std::endl;
      }
#ifdef OLD
      index = start + cnt + 2;
#else
      index = start + cnt;
#endif
    } // end of if( word1 == 100 )
    else {

      // Format of record is
      //   data[0] = # of data words
      //   data[1..n] = data words
      //   data[n+1] = checksum for buffer
      if (bufcnt < 10)
	std::cout << "Buffer " << bufcnt << ": Data record, "
		  << "nwords = " << cnt << " checksum = "
		  << "0x" << std::hex << data[index + cnt + 1] << std::dec << std::endl;

      int checksum = 0;

      int index_camac = start + cnt;

      if( mode != "calib" && mode != "external" )
	{

	  cout << "------------------------------------------=" << endl;
	  
	  int camac_ADC_num = data[index_camac];
	  //cout << "CAMAC ADC num: " << camac_ADC_num << endl;
	  // only for some data (taken at the beginning of the implementation)
	  if( fname.find( "nwu_fphx_raw_20201113-0137_0.dat" ) != string::npos || 
	      fname.find( "nwu_fphx_raw_20201113-1355_0.dat" ) != string::npos || 
	      fname.find( "nwu_fphx_raw_20201116-1112_0.dat" ) != string::npos || 
	      fname.find( "nwu_fphx_raw_20201116-1208_0.dat" ) != string::npos || 
	      fname.find( "nwu_fphx_raw_20201117-1801_0.dat" ) != string::npos || 
	      fname.find( "nwu_fphx_raw_20201117-1908_0.dat" ) != string::npos || 
	      fname.find( "nwu_fphx_raw_20201118-1435_0.dat" ) != string::npos	      
	      )
	    {

	      int camac_data_num = data[index_camac];
	      if (camac_data_num % 2 != 0) {
		cerr << "A number of camac data \"" << camac_data_num << "\" is odd. It's not expected (for the moment)" << endl;
		cerr << "Consider modfy this macro or use even number of channels (normaly #ADC = #TDC)" << endl;
	      }
	      
	      for (int i=0; i < camac_data_num / 2; i++)
		{
		  index_camac++;
		  camac_adcs.push_back(data[index_camac]);
		  //cout << "ADCs " << camac_adcs[i] << endl;
		}
	      
	      for (int i = 0; i < camac_data_num / 2; i++)
		{ 
		  index_camac++;
		  camac_tdcs.push_back(data[index_camac]);
		  //cout << "TDCs: " << camac_tdcs[i] << endl;
		}
	      
	    }
	  else
	    {
	      for (int i=0; i < camac_ADC_num; i++)
		{
		  index_camac++;
		  camac_adcs.push_back(data[index_camac]);
		  //cout << "ADCs " << camac_adcs[i] << endl;
		}
	      
	      index_camac++;
	      int camac_TDC_num = data[index_camac];
	      //cout << "CAMAC TDC num: " << camac_TDC_num << endl;
	      for (int i = 0; i < camac_TDC_num; i++)
		{ 
		  index_camac++;
		  camac_tdcs.push_back(data[index_camac]);
		  //cout << "TDCs: " << camac_tdcs[i] << endl;
		}
	    }
	}
      // loop over data in this event group until the next event group shows up
#ifdef OLD
      for (index++; index < start + cnt + 1; index++)
#else
	for (index += 2; index < start + cnt - 1; index++)
#endif
	  {
	    if ((index + 1) * sizeof(unsigned int) > size) {
	      // Safety belt against a partially-written buffer (it will have the full length
	      // field, but the whole buffer hasn't been read in).  This can happen, for instance,
	      // if we are reading a file that is actively being written.
	      std::cout << "Partial buffer detected, bailing" << std::endl;
	      break;
	    }

	    // ^= means XOR
	    checksum ^= data[index];

	    command = data[index];
	    int rawchip = -1;

	    // Check whether this word holds an event#, a full 16-bit BCO# or hit data:
	    if ((data[index] & 0xFFFF) == 1) {
	      event_fem = (data[index] & 0xFFFF0000) >> 16;
	      //cout  <<  "event_fem = "  <<  event_fem  << endl;
	      continue;

	    }
	    else if ((data[index] & 0xFFFF) == 2) {
	      bco_full = (data[index] & 0xFFFF0000) >> 16;
	      //cout  <<  "bco_full = "  <<  bco_full  <<  endl;
	      continue;
	    }
	    else {
	      //check if this is a FEM_ID (bottom 12 bits = 0) or a data word:
	      if ((data[index] & 0xfff) == 0) {
		fem_id = (data[index] & 0xf000) >> 12;
		//cout << "fem_id = " << fem_id << endl;
		continue;
	      }
	      else {
		rawchip = (data[index] >> 3) & 0x3F;

		if (rawchip < 27)
		  chip_id = rawchip;
		else
		  chip_id = rawchip - 26;

		fpga_id = 0;

		if (fem_id == 1) {
		  if (rawchip < 27)
		    module = 1;
		  else
		    module = 2;
		}
		else if (fem_id == 2) {
		  if (rawchip < 27)
		    module = 3;
		  else
		    module = 4;
		}
		else if (fem_id == 4) {
		  if (rawchip < 27)
		    module = 7;
		  else
		    module = 8;
		}
		else if (fem_id == 8) {
		  if (rawchip < 27)
		    module = 5;
		  else
		    module = 6;
		}

		// assign data to each variable to fill TTree
		mchip = 0;
		ampl = (data[index] >> 24) & 0x7F;
		bco = (data[index] >> 16) & 0x7F;
		chan_id = (data[index] >> 9) & 0x7F; //((data[index] & 0x200) >>3) | ((data[index] & 0xFC00)>>10); //data[index]>>9) & 0x7F; //
		adc = (data[index] & 0x07);

		/*
		  if( chan_id<0 || chan_id>=128 || ampl<0 || ampl>=64 ){
		  cout << "---" << chan_id << " " << ampl << endl;
		  }
		*/
		nhits[chan_id][ampl]++;
	      }
	    }

	    int noise; //,inoise,ihealthy;

	    if (ampl != 0)
	      noise = 1;
	    else if (chip_id == 0 || chip_id > 26)
	      noise = 1;
	    else if (chan_id == 0 || chan_id > 126)
	      noise = 1;
	    else if (fem_id != 4 || module != 7)
	      noise = 1;
	    else
	      noise = 0;

	    if (noise != 0)
	      inoise++;
	    else if (noise == 0)
	      ihealthy++;

	    if (event_fem != old_event_fem) {

	      if (old_event_fem >= 0) {

		for (int ichip = 0; ichip < 8; ichip++) {
		  if (chiphit[ichip] != 0)
		    nchip_event++;

		}

	      } // end of if( old_event_fem >= 0 )

	      nhit_event = 1;
	      nchip_event = 0;

	      for (int ichip = 0; ichip < 8; ichip++) {
		chiphit[ichip] = 0;
	      }

	      if ((chip_id < 9) && (chip_id != 0)) {
		chiphit[chip_id - 1]++;
	      }

	      old_event_fem = event_fem;
	      old_bco = bco;
	      old_bco_full = bco_full;
	    } // end of if( event_fem != old_event_fem )
	    else {
	      nhit_event++;
	      if ((chip_id < 9) && (chip_id != 0))
		chiphit[chip_id - 1]++;
	    }
	    
	    bcos      .push_back( bco      );
	    adcs      .push_back( adc      );
	    ampls.     push_back( ampl     );
	    chip_ids  .push_back( chip_id  );
	    fpga_ids  .push_back( fpga_id  );
	    modules   .push_back( module   );
	    fem_ids   .push_back( fem_id   );
	    chan_ids  .push_back( chan_id  );
	    bco_fulls .push_back( bco_full );
	    events    .push_back( event    );

	    tree->Fill();

	    //Note:  we seem to get some odd chip_ids out of the new DAQ VHDL code
	    //after the event gets larger than some value.  Need to understand this:

	    ievent++;
	  }  //for loop on 	for (index += 2; index < start + cnt - 1; index++)


      // If there are only CAMAC data but no INTT data, fill CAMAC data to the tree
      // -1 is filled to the branches for INTT events in this case. 
      if (cnt == 1) {
	is_INTT = false;
      } else {
	is_INTT = true;
      }

      /*
      cout << endl;
      cout << "-- CAMAC Data: ADC ";
      for( const auto& it : camac_adcs )
	cout << setw(3) << it << " ";

      cout << ",   TDC ";
      for( const auto& it : camac_tdcs )
	cout << setw(3) << it << " ";

      cout << " -----------------------------------" << endl;
      */

      
      // make flag whether is event consists of a single bco or not
      if( bcos.size() != 0 ) // skip here if no INTT event is 
	{
	  
	  int first_bco = bcos[0];
	  int bco_min = *min_element( bcos.begin(), bcos.end() );
	  int bco_max = *max_element( bcos.begin(), bcos.end() );

	  if( bco_max - bco_min < 2 )
	    is_single_bco_event = true;
	  else
	    is_single_bco_event = false;

	  /*
	  for( const auto& it : bcos )
	    {
	      // if this bco is not the same as bco of the first INTT event, set the flag false and break
	      if( abs(first_bco - it) > 2 ) // 
		{
		  is_single_bco_event = false;
		  break;
		}
	      else
		{
		  is_single_bco_event = true;
		}
	      
	    }
	  */
	  
	}

      if( mode == "camac_clustering" )
	MakeCluster( adcs     , adc_voltages, ampls   , chip_ids, fpga_ids,
		     modules  , chan_ids, fem_ids , bcos    ,
		     bco_fulls, events, nhits_in_cluster, usemod );
      
      tree_both->Fill();
      
      // fill TTree for only CAMAC data if CAMAC data exits
      if( mode != "calib" && mode != "external" )
	tree_camac->Fill();

      //if ( checksum != data[index] )
      //std::cout << "WARNING: bad checksum = "
      //      << std::hex << checksum << std::dec << std::endl;
#ifdef OLD
      index = start + cnt + 2;
#else

      // set the index for the next data group in the case of calibration data or external trigger mode (without CAMAC data)
      if( mode == "calib" || mode == "external" )
	index = start + cnt;
      else if( mode == "camac" || mode == "camac_clustering" ) // in the case of with CAMAC data
	index = index_camac + 1;
#endif

    }  // if block on record type
  }  //Loop over data

  cout << "inoise   = " << inoise << endl;
  cout << "ihealthy = " << ihealthy << endl;

  if (decoded_output)
    ofs.close();

  cout << "Tree entries: " << tree->GetEntries() << endl;
  cout << "Clock data num: " << clock_data_num << endl;

  tree->Write();

  if( mode == "camac" || mode == "camac_clustering" ){
    tree_camac->Write();
    tree_both->Write();
  }
	
  file->Close();

  return filename;
}


void MakeCluster( 
		 vector < int >& adcs,
		 vector < float >& adc_voltages,
		 vector < int >& ampls,
		 vector < int >& chip_ids,
		 vector < int >& fpga_ids,
		 vector < int >& modules,
		 vector < int >& chan_ids,
		 vector < int >& fem_ids,
		 vector < int >& bcos,
		 vector < int >& bco_fulls,
		 vector < int >& events,
		 vector < int >& nhits_in_cluster,
		 int module
		  )
{

  int dac[8] = {10, 23, 48, 98, 148, 172, 223, 248};

  bool there_is_noise = false;
  const int kHit_num = adcs.size();
  INTTHit* hits[kHit_num];		//vector < INTTHit* > hits;
  for( int i=0; i<kHit_num; i++ )
    {
      hits[i] = new INTTHit(adcs[i], ampls[i], chip_ids[i], fpga_ids[i],
			    modules[i], chan_ids[i], fem_ids[i], bcos[i],
			    bco_fulls[i], events[i], dac);

      // Note: condition for the moment:
      //   fem_id == 8, fpga_id == 0, module == 6, ampl == 0, 
      if( ampls[i] != 0 || fpga_ids[i] != 0 || modules[i] != module || fem_ids[i] != 8 )
	{
	  hits[i]->SetIgnored( true );
	  //cout << "it's noise!" << endl;
	  //hits[i]->Print();
	  there_is_noise = true;
	}

      if( there_is_noise )
	if( 1 < kHit_num )
	  hits[i]->PrintInOneLine();

    }

  for( int i=0; i<kHit_num; i++ )
    for( int j=i+1; j<kHit_num; j++ )
      hits[i]->Clustering( hits[j] );

  
#if defined( __linux__ ) || defined( __APPLE__)

  bcos.erase		( bcos.begin()		, bcos.end()       );
  adcs.erase		( adcs.begin()		, adcs.end()       );
  adc_voltages.erase    ( adc_voltages.begin()  , adc_voltages.end() );
  ampls.erase		( ampls.begin()		, ampls.end()      );
  chip_ids.erase	( chip_ids.begin()	, chip_ids.end()   );
  fpga_ids.erase	( fpga_ids.begin()	, fpga_ids.end()   );
  modules.erase  	( modules.begin()	, modules.end()    );
  fem_ids.erase 	( fem_ids.begin()	, fem_ids.end()    );
  chan_ids.erase	( chan_ids.begin()	, chan_ids.end()   );
  bco_fulls.erase	( bco_fulls.begin()	, bco_fulls.end()  );
  events.erase	( events.begin()	, events.end()     );
  nhits_in_cluster.erase(nhits_in_cluster.begin(), nhits_in_cluster.end() );

  for( int i=0; i<kHit_num; i++ )
    {
      // if this INTTHit isn't included to another INTTHit to form a cluster, add data to vectors
      if (hits[i]->GetClusteringStatus() > -1 )
	{

	  if( there_is_noise )
	    if( 1 < kHit_num )
	      hits[i]->Print();
	  
	bcos.			push_back( hits[i]->bco_ );
	adcs.			push_back( hits[i]->adc_ );
	adc_voltages.		push_back( hits[i]->GetClusterADCVoltage() );
	ampls.			push_back( hits[i]->ampl_ );
	chip_ids.		push_back( hits[i]->chip_id_ );
	fpga_ids.		push_back( hits[i]->fpga_id_ );
	modules.		push_back( hits[i]->module_ );
	fem_ids.		push_back( hits[i]->fem_id_ );
	chan_ids.		push_back( hits[i]->chan_id_ );
	bco_fulls.		push_back( hits[i]->bco_full_ );
	events.			push_back( hits[i]->event_ );
	nhits_in_cluster.	push_back( hits[i]->cluster_channels_.size() );

	if( hits[i]->fem_id_ != 8 )
	  {
	    //cout << "@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	    //hits[i]->Print();
	  }
	}
    }

  //  if( there_is_noise )
  //cout << endl << endl << string(100, '-' ) << endl;
  
#endif // defined( __linux__ ) || defined( __APPLE__)
}

void ShowLine(int width, string words ){
  //  int  header = 4;
  cout << " |"
       << words << string( width - words.length() - 2 - 1, ' ' )
       << "|"
       << endl;
}

void ShowMessage(){
  int width = 130;
  cout << " +" << string(width-3, '-') << "+" << endl;
  ShowLine( width, " check_chip_prototypeMaximum7.c" );
  ShowLine( width, " It worls for calibration data and data with the external trigger system by CAMAC" );
  ShowLine(width, "");
  ShowLine( width, " Usage: .x check_chip_prototypeMaximum7.c( file_name, module_num, mode, cut)" );
  ShowLine(width, "    where file_name: name of the data file (.dat)");
  ShowLine(width, "         module_num: ID of the module in use, integer or string can be used");
  ShowLine(width, "               mode: mode of operation. Following modes are accepted: ");
  ShowLine(width, "");
  ShowLine(width, "                     calib: for calibration data");
  ShowLine(width, "                  external: for externaly triggered data, for example self-trigger, source, and cosmic ray measurements");
  ShowLine(width, "                     camac: for data with the CAMAC DAQ");
  ShowLine(width, "          camac_clustering: for data with the CAMAC DAQ, Clustring is also done. It dosen't work on Windows well.");
  cout << " +" << string(width-3, '-') << "+" << endl;
}
