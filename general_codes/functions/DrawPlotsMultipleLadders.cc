#pragma once
#include "DrawPlotsMultipleLadders.hh"

/*!
  @fn void DrawPlots( string root_file, int usemod ){
  @brief Some plots are drawn using root_file.
  @param string root_file A path to a ROOT file to be used. Return value of MakeTree is OK.
  @param int usemod ID of the module
  @details Following plots are drawn:
  - Amplitude vs ADC for each chip
  - Amplitude vs Channel for each chip
  - Channel distribution for each chip
  - Hit map (chip vs channel)
  Common cuts for all plots are in cut_base.

  TTree::Draw was performed many times to draw a plot of a chip in ver3 so the macro took time.
  To save the time, histograms with additional dimension of chip_id are made and projected on x-y dimensions.
  For example, a histogram of amplitude vs ADC vs chips is made and projected on amplitude vs ADC dimension with selection of a chip.
  Old methods are remained as they are for the moment.

*/
void DrawPlotsMultipleLadders(string root_file, int usemod, string mode, string plot_type ) {

  // general setting for ROOT
  gStyle->SetPalette(1);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  //gStyle->SetOptTitle(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);

  if( plot_type != "all" )
    gErrorIgnoreLevel = kWarning;

  // open the ROOT file
  TFile* tf = new TFile(root_file.c_str(), "READ"); // c_str() converts string type to const char* type
  if( tf == nullptr ){
    cerr << " Error: " << root_file << " cannot be opened." << endl << " Check whether the path is correct" << endl;
    cerr << " Program stopped" << endl;
    return;
  }
    
  // get TTree in the ROOT file
  auto tree = (TTree*)tf->Get("tree");

  // definition of the order of chips
  // int chip_order[26] = { 26,25,24,23,22,21,20,19,18,17,16,15,14,
  // 			 13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

  // basic cuts to be applied in any case
  stringstream cut_base; // useful class to generate string using non-string and char parameters
  cut_base << "ampl<70 " << "&& module==" << usemod << " && chan_id<128 && chip_id<27";

  stringstream ss;
  ss << usemod;
  
  string output_base = root_file.substr(0, root_file.find_last_of(".root") - 4)
    + "__module" + ss.str() ;

  if( plot_type == "all"  ) // for debugging
    {
      DrawPlots_AmplADC( output_base, tree, cut_base.str(), false );
      DrawPlots_ChAmpl( output_base, tree, cut_base.str(), false );
      DrawPlots_Ch( output_base, tree, cut_base.str(), false, mode );
      DrawPlots_ADC( output_base, tree, cut_base.str(), false, mode );
      return;
    }

  if( mode == "calib" ){ // plots for calibration data
    if( plot_type == "ampl_adc" )
      DrawPlots_AmplADC( output_base, tree, cut_base.str(), false );
    else if( plot_type == "ch_ampl" )
      DrawPlots_ChAmpl( output_base, tree, cut_base.str(), false );
    
  }

  // plots for all data
  if( plot_type == "ch" ) // channel dists
    DrawPlots_Ch( output_base, tree, cut_base.str(), false, mode );
  else if( plot_type == "adc" ) // ADC dists
    DrawPlots_ADC( output_base, tree, cut_base.str(), false, mode );

  cout << root_file << ", module" << usemod << ", " << mode << ", " << plot_type << " done." << endl;
  //tf->Close();
  return;
}


void DrawPlots_AmplADC( string output_base, TTree* tree, string cut, bool reverse_chip_order )
{

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Amplitude vs ADC for each chip, only "calib" mode make sence                                 //
  //////////////////////////////////////////////////////////////////////////////////////////////////
  string canvas_name = string("ampl_adc");
  TCanvas *c = new TCanvas( canvas_name.c_str(), canvas_name.c_str(), 0, 0, 1625, 250);
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Idea to draw histogram of all chips faster:                                                  //
  //     Make a histogram as a function of chip,                                                  //
  //     and then make histograms for each chip by projecting on the axis of chip                 //
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // hist of amplitude vs ADC vs chip
  TH3D* hist_ampl_adc_chip = new TH3D("ampl_adc_chip", "Amplitude vs ADC vs Chip ID;Amplitude;ADC;Chip ID",
				      70, 0, 70, 8, 0, 8, 26, 0, 26);

  // expression for Draw
  string expression_ampl_adc_chip = string("chip_id:adc:ampl>>") + hist_ampl_adc_chip->GetName();

  // draw the distribution and fill the hist with the result
  tree->Draw(expression_ampl_adc_chip.c_str(), cut.c_str(), "goff");

  // 2D hists to contain distribution of amplitude vs ADC for each chip
  TH2D* hist_ampl_adc[26];

  // divide the current canvas into 13x2 pads
  c->Divide(13, 2);

  // loop over all chips to draw each hist
  for (int i = 0; i < 26; i++) {
    // move to the next pad
    c->cd(i + 1);

    int chip_num = GetChipNum( i, reverse_chip_order );
    
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.08);
    gPad->SetRightMargin(0.03);
    gPad->SetLeftMargin(0.08);

    // to specify the range of an axis for TH2::Project3D, SetRange or SetRangeUser is needed. Otherwize you can't select a certain chip
    //hist_ampl_adc_chip->GetZaxis()->SetRangeUser(chip_num, chip_num + 1);
    //hist_ampl_adc_chip->GetZaxis()->SetRangeUser(chip_num, chip_num);
    hist_ampl_adc_chip->GetZaxis()->SetRange(chip_num + 1, chip_num + 1);

    // make projection to get amplitude vs ADC for the chip
    //hist_ampl_adc[i] = (TH2D*)hist_ampl_adc_chip->Project3D(Form("yx_chip%d", chip_num));
    hist_ampl_adc[i] = (TH2D*)hist_ampl_adc_chip->Project3D("YX");
    hist_ampl_adc[i]->SetName(Form("yx_chip%d", chip_num));
    hist_ampl_adc[i]->SetTitle(Form("chip_id==%d", chip_num));
    hist_ampl_adc[i]->Draw("colz");

    // configuration of titles and labels for axes
    hist_ampl_adc[i]->GetXaxis()->SetTitleSize(0.1);
    hist_ampl_adc[i]->GetXaxis()->SetLabelSize(0.06);
    hist_ampl_adc[i]->GetYaxis()->SetTitleSize(0.1);
    hist_ampl_adc[i]->GetYaxis()->SetLabelSize(0.08);

    gPad->Update();

    // draw the title of the histo manually to change the size
    auto pave_title = (TPaveText*)gPad->GetPrimitive("title"); // "auto" means the program guesses what kind of type should be used by itself from right side
    pave_title->SetTextSize(0.1);
    pave_title->SetTextAlign(23);
    pave_title->Draw();

  }

  // save the current canvas into a pdf file
  string output = GetOutputName( output_base, "ampl_adc", ".pdf" );
  c->Print(output.c_str());

  output = GetOutputName( output_base, "ampl_adc", ".root" );
  c->Print(output.c_str());

  return 0;
}

void DrawPlots_ChAmpl( string output_base, TTree* tree, string cut, bool reverse_chip_order ){
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Amplitude vs Channel for each chip, only "calib" mode make sence                             //
  //////////////////////////////////////////////////////////////////////////////////////////////////
  string canvas_name = string("ch_ampl");
  TCanvas *c = new TCanvas( canvas_name.c_str(), canvas_name.c_str(), 0, 300, 1625, 250);
  c->Divide(13, 2);

  // 3D hist ( amplitude vs channel vs chip )  --> 2D hist of a chip ( amplitude vs channel) by making a profile on z-axis
  TH3D* hist_ampl_ch_chip = new TH3D("ampl_ch_chip", "Amplitude vs Channel vs Chip;Amplitude;Channel;Chip",
				     127, 0, 127, 128, 0, 128, 26, 0, 26); // channel bins are reduced by a factor of 4
		
  string expression_ampl_ch_chip = string("chip_id:chan_id:ampl>>") + hist_ampl_ch_chip->GetName();
  tree->Draw(expression_ampl_ch_chip.c_str(), cut.c_str(), "goff");

  for (int i = 0; i < 26; i++) { // loop over all chips
    c->cd(i + 1);
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.08);
    gPad->SetRightMargin(0.03);
    gPad->SetLeftMargin(0.08);

    int chip_num = GetChipNum( i, reverse_chip_order );
    
    // change the range of z-axis (chip) to select only this chip
    //hist_ampl_ch_chip->GetZaxis()->SetRange(chip_num, chip_num + 1);
    hist_ampl_ch_chip->GetZaxis()->SetRange(chip_num + 1, chip_num + 1);

    // make projection hist (2D)
    auto hist_ampl_ch = (TH2D*)hist_ampl_ch_chip->Project3D(Form("xy%d", chip_num));
    hist_ampl_ch->SetTitle(Form("chip_id==%d", chip_num));

    // draw more than 15 on the chip which data number is over 15 at each channel	
    // set range 15 number
    // you have to know if more than 15 data comes

    bool is_warned = false;

    for (int j = 0; j < 128; j++) {
      for (int k = 0; k < 64; k++) {
	int bin_entry = hist_ampl_ch->GetBinContent(j, k);

	if (bin_entry > 15)
	  is_warned = true;
      }
      if (is_warned == true)
	break;
    }
		
    // zoom the range of y-axis for better view
    for (int j = 127; j > 0; j--) { // loop over all bins of y-axis
      int entries = hist_ampl_ch->ProjectionX(Form("ampl_of_ch%d", j), j, j + 1, "goff")->GetEntries(); // get a number of events in j <= y <= y+1
      //cout << i << "\t" << chip_num << "\t" << j << "\t" << entries << endl;
				
      if (entries != 0) { // if non-zero event was found, that's the maximum value
	hist_ampl_ch->GetYaxis()->SetRangeUser(0, j);
	break;
      }

      //hist_ampl_ch->GetXaxis()->SetTitleSize( 0.08 );
      hist_ampl_ch->GetXaxis()->SetLabelSize(0.08); // zoom the labal for x-axis
      //hist_ampl_ch->GetYaxis()->SetTitleSize( 0.08 );
      hist_ampl_ch->GetYaxis()->SetLabelSize(0.08); // zoom the labal for y-axis

      if (j == 64) { // you have to check from 0 to 64 ampl
	hist_ampl_ch->GetYaxis()->SetRangeUser(0, j);
	break;
      }
    }

    hist_ampl_ch->Draw("colz");

    hist_ampl_ch->GetZaxis()->SetRangeUser(0, 15);

    if (is_warned == true) {
      TLatex* warning = new TLatex();
      warning->SetTextSize(0.25);
      warning->DrawLatex(5.0, 40.0, "over 15");
    }
			
    gPad->Update();
    auto pave_title = (TPaveText*)gPad->GetPrimitive("title");
    pave_title->SetTextSize(0.1);
    pave_title->SetTextAlign(23);
    pave_title->Draw();
  }
  
  string output = GetOutputName( output_base, "ch-ampl", ".pdf" );
  c->Print(output.c_str());

  output = GetOutputName( output_base, "ch_ampl", ".root" );
  c->Print(output.c_str());

  return 0;
}
  
void DrawPlots_Ch( string output_base, TTree* tree, string cut, bool reverse_chip_order, string mode ){

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Channel distribution for each chip, any mode is OK                                           //
  //////////////////////////////////////////////////////////////////////////////////////////////////

  string canvas_name = string("ch");
  TCanvas *c = new TCanvas( canvas_name.c_str(), canvas_name.c_str(), 0, 560, 1625, 250);
  TH2D* hist_ch_chip = new TH2D("ch_chip", "Channel vs Chip;Channel;Chip", 130, 0, 130, 26, 0, 26);

  string expression_ch_chip = string("chip_id:chan_id>>") + hist_ch_chip->GetName();
  tree->Draw(expression_ch_chip.c_str(), cut.c_str(), "goff");

  c->Divide(13, 2);
  vector < int > bin_contents_ch;
  for (int i = 0; i < 26; i++) {
    c->cd(i + 1);

    int chip_num = GetChipNum( i, reverse_chip_order );
    auto hist_ch = hist_ch_chip->ProjectionX(Form("ch_chip%d", chip_num), chip_num + 1, chip_num + 1, "goff");
    hist_ch->SetTitle(Form("chip_id==%d", chip_num));
    hist_ch->SetFillColor( hist_ch->GetLineColor() );
    hist_ch->SetFillStyle( 1001 );


    hist_ch->Draw();
    gPad->SetLogy(true);
    hist_ch->GetXaxis()->SetLabelSize(0.08);
    hist_ch->GetYaxis()->SetLabelSize(0.1);
    //gPad->SetLogy(true);

    gPad->Update();
    auto pave_title = (TPaveText*)gPad->GetPrimitive("title");
    pave_title->SetTextSize(0.1);
    pave_title->SetTextAlign(23);
    pave_title->Draw();

    // store all bin content into a vector to set the same range to the all y-axis
    for (int j = 0; j < hist_ch->GetNbinsX(); j++)
      if( hist_ch->GetBinContent(j) != 0 )
	bin_contents_ch.push_back(hist_ch->GetBinContent(j));
  }

  // if non-zero element is in the vector, set range of the y-axis
  if (bin_contents_ch.size() != 0) {

    // get min and max element in the vector 
    int ymin = *min_element(bin_contents_ch.begin(), bin_contents_ch.end());
    int ymax = *max_element(bin_contents_ch.begin(), bin_contents_ch.end());

    // if min and max are the same, add 1 to max to avoid error
    if (ymin == ymax)
      ymax += 1;

    // loop over all chip to set the same range to each y-axis
    for (int i = 0; i < 26; i++)
      {

	c->cd( i + 1 );
	int chip_num = GetChipNum( i, reverse_chip_order );
	// get histogram in this pad
	auto hist = (TH1D*)gPad->GetPrimitive(Form("ch_chip%d", chip_num));
	//hist->GetYaxis()->SetRangeUser(ymin, ymax);

	if( mode == "calib" ){
	  gPad->SetLogy(false);
	  hist->GetYaxis()->SetRangeUser( 0.0, 450.0 ); // it's better to use fixed-range for calibration measurements
	}

      }
  }

  string output = GetOutputName( output_base, "ch", ".pdf" );
  c->Print(output.c_str());

  output = GetOutputName( output_base, "ch", ".root" );
  c->Print(output.c_str());

  return;
}

void DrawPlots_ADC( string output_base, TTree* tree, string cut, bool reverse_chip_order, string mode ){

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // ADC distribution for each chip, any mode is OK                                           //
  //////////////////////////////////////////////////////////////////////////////////////////////////

  string canvas_name = string("adc");
  TCanvas *c = new TCanvas( canvas_name.c_str(), canvas_name.c_str(), 0, 700, 1625, 250);
  TH2D* hist_adc_chip = new TH2D("adc_chip", "ADC vs Chip;Adc;Chip", 8, 0, 8, 26, 0, 26);

  string expression_adc_chip = string("chip_id:adc>>") + hist_adc_chip->GetName();
  tree->Draw(expression_adc_chip.c_str(), cut.c_str(), "goff");

  c->Divide(13, 2);
  vector < int > bin_contents_adc;
  for (int i = 0; i < 26; i++) {
    c->cd(i + 1);

    int chip_num = GetChipNum( i, reverse_chip_order );
    auto hist_adc = hist_adc_chip->ProjectionX(Form("adc_chip%d", chip_num), chip_num + 1, chip_num + 1, "goff");
    hist_adc->SetTitle(Form("chip_id==%d", chip_num));
    hist_adc->SetFillColor( hist_adc->GetLineColor() );
    hist_adc->SetFillStyle( 1001 );


    hist_adc->Draw();
    gPad->SetLogy(true);
    hist_adc->GetXaxis()->SetLabelSize(0.08);
    hist_adc->GetYaxis()->SetLabelSize(0.1);
    //gPad->SetLogy(true);

    gPad->Update();
    auto pave_title = (TPaveText*)gPad->GetPrimitive("title");
    pave_title->SetTextSize(0.1);
    pave_title->SetTextAlign(23);
    pave_title->Draw();

    // store all bin content into a vector to set the same range to the all y-axis
    for (int j = 0; j < hist_adc->GetNbinsX(); j++)
      if( hist_adc->GetBinContent(j) != 0 )
	bin_contents_adc.push_back(hist_adc->GetBinContent(j));
  }

  // if non-zero element is in the vector, set range of the y-axis
  if (bin_contents_adc.size() != 0) {

    // get min and max element in the vector 
    int ymin = *min_element(bin_contents_adc.begin(), bin_contents_adc.end());
    int ymax = *max_element(bin_contents_adc.begin(), bin_contents_adc.end());

    // if min and max are the same, add 1 to max to avoid error
    if (ymin == ymax)
      ymax += 1;

    // loop over all chip to set the same range to each y-axis
    for (int i = 0; i < 26; i++)
      {

	c->cd( i + 1 );

	int chip_num = GetChipNum( i, reverse_chip_order );
	
	// get histogram in this pad
	auto hist = (TH1D*)gPad->GetPrimitive(Form("adc_chip%d", chip_num));
	//hist->GetYaxis()->SetRangeUser(ymin, ymax);

	if( mode == "calib" ){
	  gPad->SetLogy(false);
	  hist->GetYaxis()->SetRangeUser( 0.0, 450.0 ); // it's better to use fixed-range for calibration measurements
	}

      }
  }

  string output = GetOutputName( output_base, "adc", ".pdf" );
  c->Print(output.c_str());

  output = GetOutputName( output_base, "adc", ".root" );
  c->Print(output.c_str());

  return;
}

/*
void DrawPlots_Hitmap( string output_base, TTree* tree, string cut, bool reverse_chip_order ){

  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Hit map ( chip vs channel ), any mode is OK                                                  //
  //////////////////////////////////////////////////////////////////////////////////////////////////
  string canvas_name_c4 = string("hit_map_module") + ss.str().c_str();
  TCanvas *c4 = new TCanvas( canvas_name_c4.c_str(), canvas_name_c4.c_str(), 0, 750, 1625, 250);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                                              //
  //                  26  25  24  23  22  21  20  19  28  17  16  15  14   chip_id                //
  //                +---+---+---+---+---+---+---+---+---+---+---+---+---+                         //
  //   256    c   1 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //   255    h   2 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //   ...    a ... |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //   130    n 126 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //   128      127 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //          i --- +---+---+---+---+---+---+---+---+---+---+---+---+---+                         //
  //   127    d 127 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //   126      126 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //   ...      ... |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //     2        2 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //     1        1 |   |   |   |   |   |   |   |   |   |   |   |   |   |                         //
  //   y-axis       +---+---+---+---+---+---+---+---+---+---+---+---+---+                         //
  //                  13  12  11  10   9   8   7   6   5   4   3   2   1  chip_id                 //
  //           x-axis  1   2   3   4   5   6   7   8   9  10  11  12  13                          //
  //                                                                                              //
  //                       Schematic figure of what will be drawn                                 //
  //////////////////////////////////////////////////////////////////////////////////////////////////

  TH2D* hitmap = new TH2D("hitmap", "Hit map; chip_id; chan_id", 13, 0, 13, 257, 0, 257);

  // Super long expression to draw a hit map in one execution:
  //   int(chip_id / 14) * 256+ pow(-1, int(chip_id / 14)) * chan_id: -chip_id+ (1 + int(chip_id / 14)) * 13"
  // here
  //    int(chip_id / 14) : = 0 (chip=0, 1, ..., 13) or = 1 (chip=14, ..., 27)
  //                x-axis: -chip_id+ (1 + int(chip_id / 14)) * 13  
  //                         X value should start from 13 (for chip1) and value decreases as chip number increases basically.
  //                         For chips #14 or later, another offset of 13 (26 in total) is needed.
  //                         The first term -chip_id is the depending term on chip ID.
  //                         The second term is an offset to draw hist from right to left.
  //
  //                y-axis: int(chip_id / 14) * 256+ pow(-1, int(chip_id / 14)) * chan_id
  //                        Y value is bit complicated...
  //                        For chip from 1 to 13, a channel ID is simply the same as Y coordinate (ch_{i} = y_{i}).
  //                        For chip from 14 to 26, relation between a channel ID ch_{i}  and Y coordinate y_{i} is ch_{i} = 256 - y_{i} (i=0, 1, ...).
  //                        The first term is an offset, 0 for chip 0-13 and 256 for chip 14-27.
  //                        The second term is the depending term on the channel ID. Sign is + for chip 0-13 and - for chip 14-27.


  string expression = string("int(chip_id / 14) * 256") // it's offsets for y-axis.
    + "+ pow(-1, int(chip_id / 14)) * chan_id"        // Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
    + ": -chip_id"                                   // Basically, x value increases as chip_id decreases (chip13 at x=0, chip12 at x=-1, ...)
    + "+ (1 + int(chip_id / 14)) * 13"               // In addition to above, offset is needed. 
    + ">> hitmap";
	
  tree->Draw(expression.c_str(), cut_base.str().c_str(), "goff");
  hitmap->Draw("colz");

  hitmap->GetXaxis()->SetNdivisions(15);
  hitmap->GetXaxis()->SetLabelSize(0.08);
  hitmap->GetXaxis()->SetTitleSize(0.08);
  hitmap->GetYaxis()->SetLabelSize(0.08);

  // get all bin contents to set max and min value of this hist
  vector < double > bin_contents;
  for (int i = 0; i < hitmap->GetNbinsX(); i++) {
    for (int j = 0; j < hitmap->GetNbinsY(); j++) {
      double value = hitmap->GetBinContent(i, j);

      // sore non-zero value
      if (value != 0)
	bin_contents.push_back(value);
    }
  }

  // if non-zero element is in the vector, set the range of the z-axis 
  if( bin_contents.size() != 0){

    // get the min and max value in the vector 
    int zmin = *min_element(bin_contents.begin(), bin_contents.end());
    int zmax = *max_element(bin_contents.begin(), bin_contents.end());

    // if the min and the max are the same, add 1 to max to avoid error
    if (zmin == zmax)
      zmax += 1;

    hitmap->GetZaxis()->SetRangeUser(zmin, zmax);
  }

  // draw chip_id on the hist
  for (int i = 0; i < 26; i++) {
    TLatex* tex = new TLatex();
    double x = i < 13 ? 12.25 - i : 12.25 - i + 13;
    double y = i < 13 ? 64 : 192;
    tex->DrawLatex(x, y, Form("Chip%d", i + 1));
  }

  string output_hitmap = root_file.substr(0, root_file.find_last_of(".root") - 4)
    + "_hitmap_module" + ss.str().c_str() + ".pdf";
  c4->Print(output_hitmap.c_str());

  return;
}
*/
