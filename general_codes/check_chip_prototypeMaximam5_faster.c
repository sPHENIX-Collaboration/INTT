#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "TNtuple.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"

int getFileSize(std::ifstream& s);

// Data in a .dat file is decode and filled to a TTree
string MakeTree(const char* fname,int usemod=3,int maxbuf=0,int n_meas=64,float maxscale=200.);

// Review plots are made using a TTree in the given ROOT file
void DrawPlots( string root_file, int usemod  );

int check_chip_prototypeMaximam5_faster
(
 const char* fname = "C:\root_5.34.36\macros",
 int usemod = 3,
 int maxbuf = 0,
 int n_meas = 64,
 float maxscale = 200.
 )
{

  const string root_file = MakeTree( fname, usemod, maxbuf, n_meas, maxscale );

  //  return 0;
  
  if( root_file != "" )
    DrawPlots( root_file, usemod );

  return 0;
}


string MakeTree(const char* fname,int usemod=3,int maxbuf=0,int n_meas=64,float maxscale=200.)
{
  //char cf_name[100]; // not used
  //  unsigned long int dataIn[1000000]; // not used
  //long lSize; // not used
  // size_t result; // not used

  int bco, adc, ampl, col_raw, col, chip_id, fpga_id, module, mchip;
  int chan_id;//, n_hits, // not used
  int nhit_event, nchip_event, fem_id;
  int chiphit[8];
  int event_fem, old_event_fem, bco_full, old_bco_full, old_bco;
  int ievent           = 0;

  //FILE* cal_file; // not used

  // make a file name to save
  string output = fname; // conversion fron const char* to string to use string::substr and string::find_last_of
  //output = output.substr( 0, output.find_last_of( ".dat" ) - 3 ) + ".root";
  output = output.substr( 0, output.find_last_of( ".dat" ) - 3 ) + "_debug.root";
  cout << "filename   = " << output << endl;

  return output;
  
  const int NCHAN    = 128;
  const int NMODULES = 8;
  const int NCHIPS   = 26;

  //Tree-----------------------------------------------------------------------------------------
  //TNtuple *hits  = new TNtuple("hits", "test", "chan_id:ampl:adc:rawchip:event_fem:bco_full:fem_id:module:chip_id:event"); // not saved, so no need, right?
  //TNtuple *event = new TNtuple("event","event","nhits:event_fem:bco:bco_full:nchip_event:ampl"); // filled but not used, so no need, right?

  TTree *tree = new TTree("tree","chip info");
  tree->Branch("adc",      &adc,      "adc/I"      );
  tree->Branch("ampl",     &ampl,     "ampl/I"     );
  tree->Branch("chip_id",  &chip_id,  "chip_id/I"  );
  tree->Branch("fpga_id",  &fpga_id,  "fpga_id/I"  );
  tree->Branch("module",   &module,   "module/I"   );
  tree->Branch("chan_id",  &chan_id,  "chan_id/I"  );
  tree->Branch("fem_id",   &fem_id,   "fem_id/I"   );
  tree->Branch("bco",      &bco,      "bco/I"      );
  tree->Branch("bco_full", &bco_full, "bco_full/I" );
  tree->Branch("event",    &ievent,   "event/I"    );

  //--------------------------------------------------------------------------------------------------
  std::ifstream f( fname, std::ifstream::binary );

  if(!f.is_open()){
    std::cerr << "Failed to open input file " << fname << std::endl;
    return "";
  }

  // get size of file
  int size = getFileSize(f);
  std::cout << "Input file " << fname << std::endl;
  std::cout << "Number of bytes in file = " << size << std::endl;

  if( size == 0 ) {
    cerr << "size is 0. return"  << endl;
    return "";
  }

  // Brute force approach: allocate a single array to hold the entire
  // contents of the file
  UInt_t* data = new UInt_t [ size/sizeof(UInt_t) ];

  /*
  for( int i=0; i<size/sizeof(UInt_t); i++ )
    cout << data[i] << " ";
  cout << endl;
  */
  
  f.read( (char*)data, size );
  if( f.fail() ){
    std::cerr << "eof, fail, or bad condition during read" << std::endl;
    delete [] data;
    return "";
  }
  
  f.close();
  
  // Counter to count the number of hits for each channel and amplitude:
  int aaa; // not used
  int nhits[128][128] = { { 0 } };

  // Extract bco, channel, amplitude, and adc from each hit, and count the number of
  // hits at each amplitude for each channel:

  int bufcnt           = 0;
  old_event_fem = -999;
  int inoise = 0, ihealthy = 0, icheck_sum_false = 0;

  //ofstream ofs( Replace( fname, ".dat", "_decoded.dat" ) );
  string output_dat = string(fname).substr( 0, string(fname).find_last_of( "." ) ) + "_decoded.dat";
  ofstream ofs( output_dat.c_str() );

  cout << "just before for loop" << endl;
  // loop over all data
  for( int index=0; index<size/sizeof(int); bufcnt++ ){

    if(maxbuf && bufcnt>=maxbuf)
      break;

#ifdef OLD
    // Get record size
    int start = index;
    int cnt   = data[start];
    // Unpack the record
    int word1 = data[start+1];
    int word2 = data[start+2];

#else
    // here is used now
    int buflen = data[index];
    int bufid  = data[index+1];
    int cnt    = buflen-1;
    int start  = index+2;
    //std::cout << buflen << " " << bufid << " " << cnt << std::endl;

    for( int j=0; j<buflen; j++ )
      {
	
	//ofs << setw(11) << hex << data[index+j] << " ";
	ofs << setw(11) << data[index+j] << " ";
	
	auto bco_temp = (data[index+j]>>16) & 0x7F;
	auto bco_full_temp = (data[index] & 0xFFFF0000) >> 16;
	//cout << bco_full_temp << " ";
	
      }
    ofs << endl;
    //cout << endl;
    
    //    for( int i=0; i<size / sizeof(UInt_t); i++ )
    if( false )
    {
	
	cout << setw(10) << right << index << " "
	     << setw(5) << data[index] << " "
	     << setw(5) << bufid << " "
	     << data[ start ]
	     << endl;

      }
    
    // Fake the old fmt logic
    int word1 = 0xFFFFFFFF;
    int word2 = 0xFFFFFFFF;
    if(bufid==100)
      word1=100;    
    else if(bufid==101){
      word1=0xFFFFFFFF;
      word2=0xFFFFFFFF;
    }    
    else if(bufid==102)
      word1=102;
    
#endif

    if(word1==0xFFFFFFFF && word2==0xFFFFFFFF){
      if(bufid==101){
	// This record is a time stamp
	// Format of timestamp record is
	//   data[0] = word count, excluding this word and checksum
	//   data[1] = 0xFFFFFFFF
	//   data[2] = 0xFFFFFFFF
	//   data[3] = time in clock ticks for cpu
	//   data[4] = CLOCKS_PER_SEC taken from the system value
	//   data[5] = checksum for buffer
	std::cout << "Buffer " << bufcnt
		  << ": Timestamp" << " " << data[index] << ", "
		  << data[index+1] << " "
		  << data[index+2] << " "
		  << data[index+3] << " "
		  << data[index+4] << " "
		  << data[index+5] << " "
		  << std::endl;
#ifdef OLD
	index=start+cnt+2;
#else
	index=start+cnt;
#endif
	//index = start + cnt + 2;
	continue;
	
      }

      continue;
    } // end of if(word1==0xFFFFFFFF && word2==0xFFFFFFFF){
    else if(word1==100){
      // This buffer is a configuration record
      //std::cout << "Buffer " << bufcnt << ": Configuration " << std::endl;

      index += 2;
      int runno = data[index++];

      //std::cout << "    Run " << runno << std::endl;

      unsigned short* p = (unsigned short*)&data[index];

      int n = 0;
      for( n=0; n<9; ){
	unsigned short chipid = p[n++];
	unsigned short masks[8];

	for(int m=0;m<8; m++,n++ )
	  masks[m] = p[n];

	//std::cout << "    Chip " << chipid << std::endl;
	//std::cout << "      enable masks ";

	/*
	for(int m=0; m<8; m++ )
	  std::cout << "0x" << std::hex << masks[m] <<  " ";

	std::cout << std::dec << std::endl;
	*/
	
      }

      unsigned short* p2 = &p[n];
      for( n=0; n<16; n++){
	std::cout << "      Reg " << n << ": " << (int)p2[n] << std::endl;
	continue;
      }
#ifdef OLD
      index = start + cnt + 2;
#else
      index = start + cnt;
#endif

      continue;
    } // end of if( word1 == 100 )
    else{

      // Format of record is
      //   data[0] = # of data words
      //   data[1..n] = data words
      //   data[n+1] = checksum for buffer
      /*
      if(bufcnt<10)
	std::cout << "Buffer " << bufcnt << ": Data record, "
		  <<  "nwords = " << cnt << " checksum = "
		  <<  "0x" << std::hex << data[index+cnt+1] << std::dec << std::endl;
      */
      
      int checksum=0;

#ifdef OLD
      for(index++; index<start+cnt+1; index++ )
#else
	for(index+=2; index<start+cnt-1; index++ )
#endif
	  {
	    if((index+1)*sizeof(unsigned int)>size){
	      // Safety belt against a partially-written buffer (it will have the full length
	      // field, but the whole buffer hasn't been read in).  This can happen, for instance,
	      // if we are reading a file that is actively being written.
	      //std::cout << "Partial buffer detected, bailing" << std::endl;
	      break;
	    }

	    // ^= means XOR
	    checksum ^= data[index];

	    int rawchip = -1;

	    // Check whether this word holds an event#, a full 16-bit BCO# or hit data:
	    if( (data[index]&0xFFFF) == 1 ){
	      event_fem = (data[index] & 0xFFFF0000) >> 16;
	      //cout  <<  "event_fem = "  <<  event_fem  << endl;
	      continue;

	    }
	    else if( (data[index]&0xFFFF) == 2 ){
	      bco_full = (data[index] & 0xFFFF0000) >> 16;
	      //cout  <<  "bco_full = "  <<  bco_full  <<  endl;
	      continue;
	    }
	    else{
	      //check if this is a FEM_ID (bottom 12 bits = 0) or a data word:
	      if( (data[index]&0xfff) == 0 ){
		fem_id = (data[index] & 0xf000) >> 12;
		continue;
	      }
	      else{
		rawchip = (data[index]>>3) & 0x3F;

		if( rawchip < 27 )
		  chip_id = rawchip;
		else
		  chip_id = rawchip - 26;

		fpga_id=0;

		if( fem_id == 1 ){
		  if( rawchip < 27 )
		    module = 1;
		  else
		    module = 2;
		}
		else if( fem_id == 2 ){
		  if( rawchip < 27 )
		    module = 3;
		  else
		    module = 4;
		}
		else if( fem_id == 4 ){
		  if( rawchip < 27 )
		    module = 7;
		  else
		    module = 8;
		}
		else if( fem_id == 8 ){
		  if( rawchip < 27 )
		    module = 5;
		  else
		    module = 6;
		}

		mchip   = 0;
		ampl    = (data[index]>>24) & 0x7F;
		bco     = (data[index]>>16) & 0x7F;
		chan_id = (data[index]>> 9) & 0x7F; //((data[index] & 0x200) >>3) | ((data[index] & 0xFC00)>>10); //data[index]>>9) & 0x7F; //
		adc     = (data[index] & 0x07);

		nhits[chan_id][ampl]++;
	      } // if( (data[index]&0xfff) == 0 ){
	    } // if( (data[index]&0xFFFF) == 1 ){
	    
	    int noise; //,inoise,ihealthy;

	    if( ampl != 0 )
	      noise = 1;
	    else if( chip_id == 0 || chip_id > 26 )
	      noise = 1;
	    else if( chan_id == 0 || chan_id > 126 )
	      noise = 1;
	    else if( fem_id != 4 || module != 7 )
	      noise = 1;
	    else
	      noise = 0;

	    if( noise != 0 )
	      inoise++;
	    else if( noise == 0 )
	      ihealthy++;
	    

	    if( event_fem != old_event_fem ){
	      if( old_event_fem >= 0 ){
		for( int ichip=0; ichip<8; ichip++ ){
		  if( chiphit[ichip] != 0 )
		    nchip_event++;
		  
		}
		
	      } // end of if( old_event_fem >= 0 )
	      
	      nhit_event =1;
	      nchip_event=0;

	      for(int ichip=0;ichip<8;ichip++)
		chiphit[ichip]=0;
	      
	      if( (chip_id<9) && (chip_id!=0) )
		chiphit[chip_id-1]++;
	      
	      old_event_fem = event_fem;
	      old_bco       = bco;
	      old_bco_full  = bco_full;
	    } // end of if( event_fem != old_event_fem )
	    else{
	      nhit_event++;
	      
	      if( (chip_id<9) && (chip_id!=0) )
		chiphit[chip_id-1]++;
	    }

	    ievent++;
	    tree->Fill();
	    //Note:  we seem to get some odd chip_ids out of the new DAQ VHDL code
	    //after the event gets larger than some value.  Need to understand this:

	  }  //for loop on data[index]

      if ( checksum != data[index] )
	{
	  //std::cout << "WARNING: bad checksum = "
	  //<< std::hex << checksum << std::dec << std::endl;
	  icheck_sum_false++;
	}
      
#ifdef OLD
      index = start+cnt+2;
#else
      index = start+cnt;
#endif

    }  // if block on record type

    //if( index > 0  && bufid == 100 )
    //    if( ievent > 0 )
    //break;

    break;
 
  }  //Loop over data

  ofs.close();
  cout << "Decoded file: " << output_dat << endl;
  cout << setw(30) << "invalid check sum = " << icheck_sum_false
       << " -> " << 1. * icheck_sum_false / tree->GetEntries() * 100 << "%" << endl;
  cout << setw(30) << "inoise   = " << inoise   
       << " -> " << 1. * inoise / tree->GetEntries() * 100 << "%" << endl;
  cout << setw(30) << "ihealthy = " << ihealthy 
       << " -> " << 1. * ihealthy / tree->GetEntries() * 100 << "%" << endl;

  
  /*
  char* date = new char[30];
  strncpy( date, fname, 24 );
  date[24] = '\0';
  cout << "date     = " << date << endl;

  char* filename = new char[30];
  sprintf(filename, "%s.root", date );
  cout << "filename = " << filename << endl;
*/


  // save TTree into a root file
  const char* filename = output.c_str();
  TFile *file = new TFile( filename, "RECREATE" );
  //hits->Write();
  tree->Write();
  file->Close();

  return filename;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void DrawPlots( string root_file, int usemod ){

  gStyle->SetPalette(1);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  //gStyle->SetOptTitle(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);

  
  TFile* tf = new TFile( root_file.c_str(), "READ" );
  auto tree = (TTree*)tf->Get( "tree" );

  
  //////////
  int chip_order[26]={26,25,24,23,22,21,20,19,18,17,16,15,14,
                      13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  stringstream cut_base;
  cut_base << "ampl<70 " << "&& module==" << usemod << " && chan_id<127 && chip_id<27";
  //cut_base << "ampl==0 && chip_id<28 && module==6 && chan_id<128 && fem_id==8";

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Amplitude vs ADC for each chip
  //////////////////////////////////////////////////////////////////////////////////////////////////
  TCanvas *c1=new TCanvas("c1","amplitude vs ADC",0, 0, 1625,250);

  TH3D* hist_ampl_adc_chip = new TH3D( "ampl_adc_chip", "Amplitude vs ADC vs Chip ID;Amplitude;ADC;Chip ID",
				       70, 0, 70, 8, 0, 8, 26, 0, 26 );
  string expression_ampl_adc_chip = (string)"chip_id:adc:ampl>>" + hist_ampl_adc_chip->GetName();
  tree->Draw( expression_ampl_adc_chip.c_str(), cut_base.str().c_str(), "goff" );

  TH2D* hist_ampl_adc[26];
  c1->Divide(13,2);
  for(int i=0;i<26;i++){
    c1->cd( i+1 );
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.08);
    gPad->SetRightMargin(0.03);
    gPad->SetLeftMargin(0.08);
    
    hist_ampl_adc_chip->GetZaxis()->SetRangeUser( chip_order[i], chip_order[i]+1 );
    
    hist_ampl_adc[i] = (TH2D*)hist_ampl_adc_chip->Project3D( Form("yx_chip%d", chip_order[i]) );
    hist_ampl_adc[i]->SetTitle( Form("chip_id==%d", chip_order[i]) );
    hist_ampl_adc[i]->Draw( "box" );

    // configuration of titles and labels for axes
    hist_ampl_adc[i]->GetXaxis()->SetTitleSize( 0.1 );
    hist_ampl_adc[i]->GetXaxis()->SetLabelSize( 0.06 );
    hist_ampl_adc[i]->GetYaxis()->SetTitleSize( 0.1 );
    hist_ampl_adc[i]->GetYaxis()->SetLabelSize( 0.08 );

    gPad->Update();
    auto pave_title = (TPaveText*)gPad->GetPrimitive( "title" );
    pave_title->SetTextSize( 0.1 );
    pave_title->SetTextAlign( 23 );
    pave_title->Draw();

  }


  // save the current canvas into a pdf file
  string output_adc_vs_ampl = root_file.substr(0, root_file.find_last_of( ".root" ) - 4)
    + "_adcvsampl.pdf";
  c1->Print( output_adc_vs_ampl.c_str() );

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Amplitude vs Channel for each chip
  //////////////////////////////////////////////////////////////////////////////////////////////////
  TCanvas *c2 = new TCanvas( "c2", "channel_id vs amplitude", 0, 300, 1625, 250 );
  c2->Divide(13 ,2 );

  // 3D hist ( amplitude vs channel vs chip )  --> 2D hist of a chip ( amplitude vs channel) by making a profile on z-axis
  TH3D* hist_ampl_ch_chip = new TH3D( "ampl_ch_chip", "Amplitude vs Channel vs Chip;Amplitude;Channel;Chip",
				      70, 0, 70, 128/4, 0, 128, 26, 0, 26 ); // channel bins are reduced by a factor of 4
  string expression_ampl_ch_chip = (string)"chip_id:chan_id:ampl>>" + hist_ampl_ch_chip->GetName();
  tree->Draw( expression_ampl_ch_chip.c_str(), cut_base.str().c_str(), "goff" );

  for(int i=0; i<26; i++){ // loop over all chips
    c2->cd(i+1);
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.08);
    gPad->SetRightMargin(0.03);
    gPad->SetLeftMargin(0.08);

    // change the range of z-axis (chip) to select only this chip
    hist_ampl_ch_chip->GetZaxis()->SetRange( chip_order[i]-1, chip_order[i] );

    // make projection hist (2D)
    auto hist_ampl_ch = (TH2D*)hist_ampl_ch_chip->Project3D( Form( "xy%d", chip_order[i]) );
    hist_ampl_ch->SetTitle( Form("chip_id==%d", chip_order[i]) );

    // zoom the range of y-axis for better view
    for( int j=127; j>0; j-- ){ // loop over all bins of y-axis
      int entries = hist_ampl_ch->ProjectionX( Form("ampl_of_ch%d", j), j, j+1, "goff")->GetEntries(); // get a number of events in j <= y <= y+1
      //cout << i << "\t" << chip_order[i] << "\t" << j << "\t" << entries << endl;
      if( entries != 0 ){ // if non-zero event was found, that's the maximum value
	hist_ampl_ch->GetYaxis()->SetRangeUser( 0, j+1 ); 
	break;
      }

      //hist_ampl_ch->GetXaxis()->SetTitleSize( 0.08 );
      hist_ampl_ch->GetXaxis()->SetLabelSize( 0.08 ); // zoom the labal for x-axis
      //hist_ampl_ch->GetYaxis()->SetTitleSize( 0.08 );
      hist_ampl_ch->GetYaxis()->SetLabelSize( 0.08 ); // zoom the labal for y-axis
    }
	  
    hist_ampl_ch->Draw( "colz" );
    gPad->Update();
    auto pave_title = (TPaveText*)gPad->GetPrimitive( "title" );
    pave_title->SetTextSize( 0.1 );
    pave_title->SetTextAlign( 23 );
    pave_title->Draw();
  }
  
  
  string output_ampl_vs_chan = root_file.substr(0, root_file.find_last_of( ".root" ) - 4)
    + "_amplvschan.pdf";
  c2->Print( output_ampl_vs_chan.c_str() );

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Channel distribution for each chip
  //////////////////////////////////////////////////////////////////////////////////////////////////

  TCanvas *c3 = new TCanvas("c3", "entry vs channel_id", 0, 600, 1625, 250 );
  TH2D* hist_ch_chip = new TH2D( "ch_chip", "Channel vs Chip;Channel;Chip", 130, -1, 129, 26, 0, 26 );
  
  string expression_ch_chip = (string)"chip_id:chan_id>>" + hist_ch_chip->GetName();
  tree->Draw( expression_ch_chip.c_str(), cut_base.str().c_str(), "goff" );

  c3->Divide(13 ,2 );
  for( int i=0; i<26; i++ ){
    c3->cd( i+1 );

    auto hist_ch = hist_ch_chip->ProjectionX( Form("ch_chip%d", chip_order[i]), chip_order[i]+1, chip_order[i]+1, "goff");
    hist_ch->SetTitle( Form("chip_id==%d", chip_order[i]) );
    
    hist_ch->Draw();
    hist_ch->GetXaxis()->SetLabelSize( 0.08 );
    hist_ch->GetYaxis()->SetLabelSize( 0.1 );
    gPad->SetLogy( true );

    gPad->Update();
    auto pave_title = (TPaveText*)gPad->GetPrimitive( "title" );
    pave_title->SetTextSize( 0.1 );
    pave_title->SetTextAlign( 23 );
    pave_title->Draw();
  }

  

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Hit map ( chip vs channel )
  //////////////////////////////////////////////////////////////////////////////////////////////////
  TCanvas *c4 = new TCanvas("c4","hit_map", 0, 900, 1625, 250 );

  //
  //
  //
  //               chip_id
  //                01  02  03  04  05  06  07  08  09  10  11  12  13   chip_id
  //              +---+---+---+---+---+---+---+---+---+---+---+---+---+
  // 256    c   1 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // 255    h   2 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // ...    a ... |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // 130    n 127 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // 129      128 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  //        i --- +---+---+---+---+---+---+---+---+---+---+---+---+---+
  // 128    d 128 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // 127      127 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // ...      ... |   |   |   |   |   |   |   |   |   |   |   |   |   |
  //   2        2 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  //   1        1 |   |   |   |   |   |   |   |   |   |   |   |   |   |
  // y-axis       +---+---+---+---+---+---+---+---+---+---+---+---+---+
  //                14  15  16  17  18  19  20  21  22  23  24  25  26  chip_id
  //         x-axis  1   2   3   4   5   6   7   8   9  10  11  12  13


  gStyle->SetPalette();
  TH2D* hitmap = new TH2D( "hitmap", "Hit map; chip_id; chan_id", 13, 1, 14, 257/32, 0, 257 );
  string expression = (string)"(1-int(chip_id/14)) * 256"  // 
    + "- pow( -1, int(chip_id/14) ) * chan_id"    // Basicall, y-axis is chan_id, but the direction should be inversed for chip_id>=14
    + ": chip_id "                                 // Basically, x-axis is chip_id
    + "- int( chip_id/14) * 13"                    // but if chip_id > 14, subtract 13 from chip_id ( chip_id/14 = 0 for chip_id<14, and = 1 for 14<=chip_id<28 )
    + ">> hitmap";                                 // the name of hist to be filled
   
  tree->Draw( expression.c_str(), cut_base.str().c_str(), "goff" );
  hitmap->Draw( "colz" );

  string output_hitmap = root_file.substr(0, root_file.find_last_of( ".root" ) - 4)
    + "_hitmap.pdf";
  c4->Print( output_hitmap.c_str() );

  return;
}

int getFileSize(std::ifstream& f){
  f.seekg(0,std::ifstream::end);
  if(f.fail()){
    std::ifstream::iostate state=f.rdstate();
    std::cout  <<  "error seeking to end, read state = "  <<  state  <<  std::endl;
    if(state & std::ios::eofbit)  std::cout  <<  " eof bit set"  <<  std::endl;
    if(state & std::ios::failbit) std::cout  <<  " fail bit set"  <<  std::endl;
    if(state & std::ios::badbit)  std::cout  <<  " bad bit set"  <<  std::endl;
    return 0;
  }
  int size=f.tellg();
  if(size<0){
    std::ifstream::iostate state=f.rdstate();
    std::cout  <<  "error in tellg, read state = "  <<  state  <<  std::endl;
    if(state & std::ios::eofbit)  std::cout  <<  " eof bit set"  <<  std::endl;
    if(state & std::ios::failbit) std::cout  <<  " fail bit set"  <<  std::endl;
    if(state & std::ios::badbit)  std::cout  <<  " bad bit set"  <<  std::endl;
    return 0;
  }

  // Return the file stream to the start of the file
  f.seekg(0,std::ifstream::beg);

  return size;
}
