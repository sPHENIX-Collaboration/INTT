/*!
@file DrawHitMap.c
@date 2020/Oct/06
@author G. Nukazuka (RBRC, genki.nukazuka@riken.jp)
@brief A hit map of INTT test bench is drawn using TTree given through argument. Some global variables to avoid Warning are also here.
@details How To use:
A. Loading in ROOT console:
root [<n] // You already have a TTree
root [n] .L DrawHitMap.c
root [n] DrawHitMap( tree )

B. Executing in ROOT console directly:
root [<n] // You already have a TTree
root[n] .x DrawHitMap.c( tree )

*/


/*!
@fn void void DrawHitMap(Tree* tree, string expression, string cut, string option, int bin_ch)
@brief Some plots are drawn using root_file.
@param tree A pointer of the TTree that you want to use
@param cut  Cuts to be used. For example, "module==6 && chip_id<27 && ampl==0 && fem_id == 8 && ..."
@param option Options for TTree::Draw. Normaly, "colz" is OK. You may use "TEXT" option as well to see a number of event in a bin
@param bin_ch A number of bin for channels. It's up to 257 because Y-axis up to 257 is meaningful.

@details Hit map (chip vs channel)
*/


int DrawHitMap(TTree* tree, string cut = "", string option = "colz", int bin_ch = 257);

int dhm_counter = 0;

int DrawHitMap(TTree* tree, string cut, string option, int bin_ch) {

	// if bin_ch is out of range, show warning and quit
	if (bin_ch < 1 || 257 < bin_ch) {
		cerr << "  int DrawHitMap(TTree* tree, string cut, string option, int bin_ch)" << endl;
		cerr << "  bin_ch = " << bin_ch << "." << endl;
		cerr << "  The parameter should be between 1 to 257." << endl;
		cerr << "  Nothing is done." << endl;
		return -1;
	}

	// general setting for ROOT
	gStyle->SetPalette(kBird);
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
	gStyle->SetFrameBorderMode(0);
	gStyle->SetCanvasColor(0);
	gStyle->SetCanvasBorderMode(0);
	gStyle->SetPadColor(0);
	gStyle->SetPadBorderMode(0);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Hit map ( chip vs channel )                                                                  //
	//////////////////////////////////////////////////////////////////////////////////////////////////
	stringstream canvas_name;
	canvas_name << "DrawHitMap_" << dhm_counter;

	double position_y = dhm_counter%5 * 250 + 25;
	TCanvas *c4 = new TCanvas(canvas_name.str().c_str(), "hit_map", 100, position_y, 1625, 250);

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
	stringstream hist_name;
	hist_name << "DrawHitMap_hist" << dhm_counter;
	TH2D* hitmap = new TH2D( hist_name.str().c_str(), "Hit map; chip_id; chan_id", 13, 0, 13, bin_ch, 0, 257);

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

	/*	string expression = string("int(chip_id / 14) * 256") // it's offsets for y-axis.
		+ "+ pow(-1, int(chip_id / 14)) * chan_id"        // Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
		+ ": -chip_id"                                   // Basically, x value increases as chip_id decreases (chip13 at x=0, chip12 at x=-1, ...)
		+ "+ (1 + int(chip_id / 14)) * 13"               // In addition to above, offset is needed. 
		+ ">> " + hist_name.str();
	*/

	string expression = string("int(chip_id / 14) * 256") // it's offsets for y-axis.
	  + "+ pow(-1, int(chip_id / 14)) * chan_id"        // Y value increases(decreases) as chan_id decreases(increases) for chips1-13(14-26)
	  + ": chip_id - (int(chip_id / 14)) * 13 -1"               // In addition to above, offset is needed. 
	  + ">> " + hist_name.str();

	cout << expression << endl;
	tree->Draw(expression.c_str(), cut.c_str(), "goff");
	hitmap->Draw( option.c_str() );

	// setting for JPS2021 spring /////////////////////////////////////////
	hitmap->GetZaxis()->SetRangeUser(0, 22 );
	hitmap->GetYaxis()->CenterTitle();
	hitmap->GetYaxis()->SetTitleSize( 0.1 );
	hitmap->GetYaxis()->SetTitleOffset( 0.2 );

	hitmap->GetXaxis()->SetNdivisions(15);
	hitmap->GetXaxis()->SetLabelSize(0.0);
	hitmap->GetXaxis()->SetTitleSize(0.0);
	hitmap->GetYaxis()->SetLabelSize(0.08);

	// draw chip_id on the hist
	for (int i = 0; i < 26; i++) {
		TLatex* tex = new TLatex();
		//double x = i < 13 ? 12.25 - i : 12.25 - i + 13;
		//double y = i < 13 ? 64 : 192;

		double x = i < 13 ? 0.25 + i : 0.25 + i - 13;
		double y = i < 13 ? 64 : 192;
		tex->SetTextSize( 0.07 );
		tex->DrawLatex(x, y, Form("Chip%d", i + 1));
	}

	dhm_counter++;
	return 0;
}
