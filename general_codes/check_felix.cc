/*!
  @file check_felix
  @date Oct/16/2022
  @author G. Nukazuka
  @brief Plots are made using the data taken by the Felix syste in a ROOT file.
  @details INTT data with camac system for the external trigger. If no camac data, use other version.
*/

#include "functions/DrawPlotsMultipleLadders.hh"
#include "functions/DrawHitMap.c"
		   
void ShowMessage();

/*!
  @fn vector < int > GetModules( string usemod )
  @brief Given string, like "0, 1, 2", is divided into integers and returned
 */
vector < int > GetModules( string usemod )
{
  vector < int > modules;

  int pos = usemod.find_first_of( "," );
  while( pos != string::npos  ) // To replace all "," with " ", keep loop until the position reaches to the end of this string
    {
      usemod = usemod.substr(0, pos ) + " " + usemod.substr( pos+1, usemod.size() ); // replace "," to " " because " " is the key of this search
      pos = usemod.find_first_of( "," );
    }

  istringstream iss( usemod );
  int temp;
  while( iss >> temp )
    modules.push_back( temp );

  return  modules;
}


/*!
  @fn int check_felix
  @brief Parameters taken by the Felix system are drawn into each PDF. The PDFs are merged module by module. Simple parallel processing are available.
  @param fname  A name of the root file to be drawn
  @param usemod The module ID. It can contain multiple numbers, for example, "0,1,2".
  @param cut    Not implemented yet.
  @param mode   Mode of data taking, "calib", "external" are available
  @param plot_type Users don't need to give any values. If a values is given, it's passed to DrawPlotsMultipleLadders function, and only the parameter of the module is drawn.
  @details      It's based on check_chip_prototypeMaximum7.cc. Only ROOT6 or higher.
                It does:
		  - Making plots of parameters, which depends on the mode, and saving them into PDF and ROOT files.
		  - Merging plots of modules into a PDF.
		  - Open the merged PDFs.
		If you need to restrict the number of processes to be run at the same time... ... I have no idea.
  @todo         The hitmap should be implemented.
*/
int check_felix
(
 string fname = "data/calib_packv1_220927_1700.root", // a path to the data file
 string usemod = "0", // ID of the module
 string mode = "calib",
 string cut = "",
 string plot_type = "",
 bool debug = false
 )
{
  ShowMessage();

  if (mode != "calib" && mode != "external" )
    {
      cout << " Given mode\"" << mode << "\" is not supported." << endl;
      return -1;
    }

  // It's for multi-processing. The initial number of running processes can be used to confirm whether all processes are done or not, I think. If some people are using this felix server, it doesn't work.
  int init_num =  stoi( gSystem->GetFromPipe( "ps aux | wc -l" ).Data() );
  if( debug ) cout << "init num: " << init_num << endl;

  // list of parameters to be drawn. It depends on the mode.
  vector < string > draw_list;
  if( mode == "calib" ){
    draw_list.push_back( "ampl_adc" );
    draw_list.push_back( "ch_ampl" );
  }
  else{
    draw_list.push_back( "adc" );
  }
  
  draw_list.push_back( "ch" );
  
  vector < int > modules = GetModules( usemod );

  // loop over all modules for individual processes
  for( int i=0; i<modules.size(); i++ )
    {
      // DrawPlotsMultipleLadders( fname, modules[i], mode, "all" ); // for debugging, not used now

      // Shell command for this module. It completes when the parameter to be drawn is given.
      // It should be, for example,
      // root -q -b -l 'soft/INTT/general_codes/functions/DrawPlotsMultipleLadders.cc("data/calib_packv1_220927_1700.root", 0, "calib", "ampl_adc")'
      string command_base = "root -q -b -l \'/home/inttdev/soft/INTT_fork_genki/general_codes/functions/DrawPlotsMultipleLadders.cc(";
      command_base += "\"" + fname + "\", "
	+ "" + modules[i] + ", "
	+ "\"" + mode + "\", ";
      if( debug ) cout << command_base << endl;

      // Loop over all parameters to be drawn. The shell commands are executed at this level in parallel.
      for( auto& to_be_drawn : draw_list ){
	string command = command_base + "\"" + to_be_drawn + "\", "
	  + "\"" + cut + "\" )\' &";
	gSystem->Exec( command.c_str() );
	
      } // end of for( auto& to_be_drawn : draw_list )
      
    } // end of for( int i=0; i<modules.size(); i++ )

  // If the plot_type is given, it means that it's one of the multi-processes. So stop the program here.
  if( plot_type != "" )
    return 0;

  // Wait for the end of all processes. It can cause infinite loop because comparison of the number of line from "ps aux" is not so reliable, I think...
  int current_num = stoi( gSystem->GetFromPipe( "ps aux | wc -l" ).Data() );
  int counter = 0;
  while( current_num - init_num > 1 ){

    if( debug ) cout << "current num: " << current_num << endl;
    
    gSystem->Exec( "sleep 0.5s" ); // wait for 0.5s
    current_num = stoi( gSystem->GetFromPipe( "ps aux | wc -l" ).Data() );  // check the current number of running processes

    // interactive part. If the data file is too large, the processes may take long time. For such cases, this program asks users whether should it wait more or not
    if( counter++ > 60 ){ // 0.5 * 60 = 30s. I think all processes cannot take so long time.

      cerr << endl << "About 30s passesed. If you think you can go ahead, type \"y\" otherwise let's wait for more." << endl << "> ";
      string input;
      cin >> input;

      if( input == "y" ){
	cerr << "OK, let's go" << endl;
	break;
      }
      else{
	cerr << "OK, let's wait for another 30s" << endl;
	break;
	counter = 0;
      }
      
    }
  }

  if( debug ) cout << "current num: " << current_num << endl;
  cout << "Maybe all processes were done. Wait for more 3s." << endl;
  gSystem->Exec( "sleep 3s" );

  if( plot_type == "" ){
    cout << "All ROOT processes in check_felix.cc were done" << endl;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Merge pdfs into a pdf    ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  
  for( int i=0; i<modules.size(); i++ ){ // loop over all modules

    // Make canvas for this module. The size depends on the draw list.
    TCanvas* c_master = new TCanvas( "name", "title", 1625, 250 * draw_list.size() );
    c_master->Divide( 1, draw_list.size() );

    // Get all plots from each ROOT file
    vector < TCanvas* > canvases;

    // loop over all plots
    for( int j=0; j<draw_list.size(); j++ ){

      // make the name of the ROOT file for this parameter
      string root_name = fname.substr( 0, fname.size() - 5 ) + "_" + draw_list[j] + "_"
	+ "module" + to_string( modules[i] ) + ".root";

      // Open th ROOT file and store the canvas.
      TFile* tf = new TFile( root_name.c_str(), "READ" );
      canvases.push_back( (TCanvas*)(tf->Get( draw_list[j].c_str() )->Clone() ) );

    } // end of for( int j=0; j<draw_list.size(); j++ )

    // loop over all canvases obtained from each ROOT file, and draw cone of them in the master canvas
    for( int j=0; j<canvases.size(); j++ )
      {
	c_master->cd( j + 1 );
	canvases[j]->DrawClonePad();
      } // end of for( int j=0; j<canvases.size(); j++ )
    
    // save the master canvas into a PDF file. 
    string output =  fname.substr( 0, fname.size() - 5 ) + "_module" + to_string( modules[i] ) + ".pdf";
    c_master->Print( output.c_str() );
    cout << " output: " << output << endl;

    // Open the PDF file
    // string command = "open " ;
    // command += output + " >/dev/null 2>/dev/null &";
    // gSystem->Exec( command.c_str() );
    
  }  // end of for( int i=0; i<modules.size(); i++ )

  gStyle->SetOptStat();
  gStyle->SetOptFit();

  if( plot_type == "" ){
    cout << "All processes in check_felix.cc were done" << endl;
  }
    
  
  return 0;
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
  ShowLine( width, " check_felxi.c" );
  ShowLine( width, " It's for data with the external trigger system by CAMAC" );
  ShowLine(width, "");
  ShowLine( width, " Usage: .x check_chip_prototypeMaximum7.c( file_name, module_num, mode, cut)" );
  ShowLine(width, "    where file_name: name of the data file (.dat)");
  ShowLine(width, "         module_num: ID of the module in use");
  ShowLine(width, "               mode: mode of operation. Following modes are accepted: ");
  ShowLine(width, "");
  ShowLine(width, "                     calib: for calibration data");
  ShowLine(width, "                  external: for externaly triggered data, for example self-trigger, source, and cosmic ray measurements");
  ShowLine(width, "                     camac: for data with the CAMAC DAQ");
  cout << " +" << string(width-3, '-') << "+" << endl;

  if( gROOT->GetVersion()[0] == '5' ){
    cerr << "You are using ROOT" << gROOT->GetVersion() << "." << endl;
    cerr << "The version 5 is not supported. Upgrade ROOT or write codes by yourself." << endl;
    exit( -1 );

  }
    
  return;
}
