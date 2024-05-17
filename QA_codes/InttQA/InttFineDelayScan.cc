#include "InttFineDelayScan.h"

using namespace std;

//____________________________________________________________________________..
InttFineDelayScan::InttFineDelayScan(const std::string &name, bool is_official ):
  SubsysReco(name),
  is_official_( is_official )
{

}

//____________________________________________________________________________..
InttFineDelayScan::~InttFineDelayScan()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////
int InttFineDelayScan::GetNodes(PHCompositeNode *topNode)
{
  /////////////////////////////////////////////////////////////////////////
  // INTT event header

  string node_name_intteventheader = "INTTEVENTHEADER";
  node_intteventheader_map_ =
    findNode::getClass<InttEventInfo>(topNode, node_name_intteventheader);
  
  if (!node_intteventheader_map_)
    {
      //cerr << PHWHERE << node_name_intteventheader << " node is missing." << endl;
      intt_header_found_ = false;
      //return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  intt_header_found_ = true;
  
  /////////////////////////////////////////////////////////////////////////
  // INTT raw hit
  string node_name_inttrawhit = "INTTRAWHIT";
  node_inttrawhit_map_ =
    findNode::getClass<InttRawHitContainer>(topNode, node_name_inttrawhit);
  
  if (!node_inttrawhit_map_)
    {
      cerr << PHWHERE << node_name_inttrawhit << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  return 0;
}

vector < InttRawHit* > InttFineDelayScan::GetHits()
{
  
  vector < InttRawHit* > hits;
  auto raw_hit_num = node_inttrawhit_map_->get_nhits();
  for (unsigned int i = 0; i < raw_hit_num; i++)
    {
      auto hit = node_inttrawhit_map_->get_hit(i);
      hits.push_back( hit );
    }
  
  return hits;
}

void InttFineDelayScan::DrawRatioToPad( TCanvas* c,
					int pad_num,
					string title_ratio,
					vector < TH1D* > hists_ratio,
					double ymax )
{
  ////////////////////////////////////////////////////////
  // BCO diff, #hit for even / (even + odd) bins, all of all of them
  c->cd( pad_num );
  
  double margin_plot_stat = 0.01;
  double xmin_area = 0.7 + margin_plot_stat;
  double xmax_area = 1.0 - margin_plot_stat;
  double ymin_area = margin_plot_stat;
  double ymax_area = 1.0 - margin_plot_stat;
  auto row_num = (hists_ratio.size() < 4 ? hists_ratio.size() : 4);
  auto column_num = hists_ratio.size() / row_num + ( !(hists_ratio.size() % row_num) == 0 );
  double height_stat = ( ymax_area - ymin_area ) / row_num;
  double width_stat = ( xmax_area - xmin_area ) / column_num;

  
  if( ymax < 0 )
    {
      for( auto& hist : hists_ratio )
	{
	  auto val = hist->GetBinContent( hist->GetMaximumBin() );
	  if( val > ymax )
	    ymax = val;
	  
	}
      
    }
  

  for( int i=0; i<hists_ratio.size(); i++ )
    {
      hists_ratio[i]->SetTitle( title_ratio.c_str() );
      InttQa::HistConfig( hists_ratio[i], i );
      
      hists_ratio[i]->GetYaxis()->SetRangeUser( 0, ymax );
      
      string option = "";
      if( i > 0 )
	option = "sames";

      hists_ratio[i]->Draw( option.c_str());
      //hist_sidelobe_ratio_odd_->Draw( "sames" );
      double xmin = xmin_area + width_stat * (i/row_num);
      double xmax = xmin + width_stat;
      double ymax = ymax_area - height_stat * (i%row_num);
      double ymin = ymax - height_stat;
      InttQa::DrawStats( hists_ratio[i], xmin, ymin, xmax, ymax );
    }
}

void InttFineDelayScan::DrawBcoDiffRatioPair( TCanvas* c,
					      string title_bco_full, 
					      vector < TH3I* > hists_bco_diff,
					      string title_ratio,
					      vector < TH1D* > hists_ratio,
					      bool inherit_ymax )
{
  /*!
    @breif
    @param c
    @param hists_bco_diff
    @param hists_ratio
    @details
  +-----------------------------------------------------------------------+
  |                                                         stats boxes   |
  |      +---------------------------------------------------+---------+  |
  |    E |                                                   |    |    |  |
  |    N |                                   |               |    |    |  |
  |    T |                                   |               |----|----|  |
  |    R |                                   |               |    |    |  |
  |    I |    _______________________________|_              |    |    |  |
  |    E |   |   hists_bco_diff                |             |----|----|  |
  |    S |   |                                 |             |    |    |  |
  |      | __|                                 |____________ |    |    |  |
  |      +---------------------------------------------------+---------+  |
  |                                   BCO diff (0 - +128 or -128 - +128)  |
  |				                                          |
  |      +---------------------------------------------------+---------+  |
  |    E |                                                   |    |    |  |
  |    N |                                              _    |    |    |  |
  |    T |                                             | |   |----|----|  |
  |    R |                                             | |   |    |    |  |
  |    I |                                             | |   |    |    |  |
  |    E |                                            /  |   |----|----|  |
  |    S |                                    _______/   |   |    |    |  |
  |      | __________________________________/hists_ratio|_  |    |    |  |
  |      +---------------------------------------------------+---------+  |
  |      BCO diff(odd/even) /{ BCO diff(odd) + BCO diff(even) }           |
  |                                                                       |
  +-----------------------------------------------------------------------+
  */
  c->cd( 2 ) ;
  double ymax = -1.0;
  if( inherit_ymax == true )
    {
      gPad->Update(); // Don't delete it. It's needed for gPad->GetUymax()
      ymax = gPad->GetUymax();
    }
  
  c->Clear();
  
  ////////////////////////////////////////////////////////
  // canvas configuration
  c->Divide( 1, 2 );
  c->cd(1)->SetMargin( 0.1, 0.3, 0.1, 0.1 );
  gPad->SetGrid( true, true );
  
  c->cd(2)->SetMargin( 0.1, 0.3, 0.1, 0.1 );
  gPad->SetGrid( true, true );
  
  ////////////////////////////////////////////////////////
  // BCO diff
  c->cd( 1 );
  gPad->SetLogy( true );
  for( int i=0; i<hists_bco_diff.size();i++ )
    {
      // if( i == 1 )
      // 	continue;
      
      //hist_fee_chip_bco_diff[ i ]->Fill( felix_ch, chip, bco_diff );
      auto hist = hists_bco_diff[ i ]->ProjectionZ();
      hist->SetTitle( title_bco_full.c_str() );

      InttQa::HistConfig( hist, i );

      string option = "";
      if( i > 0 )
	option = "sames";

      hist->Draw( option.c_str());
      double xwidth = 0.15 - 0.001;
      double ywidth = 0.2;
      double xmin = 0.701 + xwidth * (i<4 ? 0 : 1) ;
      //double ymin = 0.9 - 0.15 * (i<4 ? i : i-4 );
      double ymax = 0.9 - ywidth * (i<4 ? i : i-4 );
      InttQa::DrawStats( hist,
		       xmin,
		       ymax - ywidth,
		       xmin + xwidth,
		       ymax ); // ymin + 0.15 );
      
    }
  
  this->DrawRatioToPad( c, 2, title_ratio, hists_ratio, ymax );
  string page_title = "Title:" + title_ratio;
  c->Print( output_pdf_.c_str(), page_title.c_str() );
  
}

void InttFineDelayScan::DrawRatioPair( TCanvas* c,
				       string title_ratio_upper,
				       vector < TH1D* > hists_ratio_upper,
				       string title_ratio_lower,
				       vector < TH1D* > hists_ratio_lower,
				       bool inherit_ymax )
{
  /*!
    @breif
    @param title_ratio_upper
    @param hists_ratio_upper
    @param title_ratio_lower
    @param hists_ratio_lower
    @details
  +-----------------------------------------------------------------------+
  |                                                         stats boxes   |
  |      +---------------------------------------------------+---------+  |
  |    E |                                                   |    |    |  |
  |    N |                                              _    |    |    |  |
  |    T |                                             | |   |----|----|  |
  |    R |                                             | |   |    |    |  |
  |    I |                                             | |   |    |    |  |
  |    E |                                            /  |   |----|----|  |
  |    S |                                    _______/   |   |    |    |  |
  |      | __________________________________/hists_ratio|_  |    |    |  |
  |      +---------------------------------------------------+---------+  |
  |      BCO diff(odd/even) /{ BCO diff(odd) + BCO diff(even) }           |
  |                                                                       |
  |      +---------------------------------------------------+---------+  |
  |    E |                                                   |    |    |  |
  |    N |                                              _    |    |    |  |
  |    T |                                             | |   |----|----|  |
  |    R |                                             | |   |    |    |  |
  |    I |                                             | |   |    |    |  |
  |    E |                                            /  |   |----|----|  |
  |    S |                                    _______/   |   |    |    |  |
  |      | __________________________________/hists_ratio|_  |    |    |  |
  |      +---------------------------------------------------+---------+  |
  |      BCO diff(odd/even) /{ BCO diff(odd) + BCO diff(even) }           |
  |                                                                       |
  +-----------------------------------------------------------------------+
  */
  c->cd( 2 ) ;
  double ymax = -1.0;
  if( inherit_ymax == true )
    {
      gPad->Update(); // Don't delete it. It's needed for gPad->GetUymax()
      ymax = gPad->GetUymax();
    }
  
  c->Clear();
  c->Divide( 1, 2 );

  cout << title_ratio_upper << "\t" << hists_ratio_upper.size() << endl;
  cout << title_ratio_lower << "\t" << hists_ratio_lower.size() << endl;
  
  ////////////////////////////////////////////////////////
  // upper pad
  c->cd(1)->SetMargin( 0.1, 0.3, 0.1, 0.1 );
  gPad->SetGrid( true, true );
  this->DrawRatioToPad( c, 1, title_ratio_upper, hists_ratio_upper, ymax );

  // lower pad
  c->cd(2)->SetMargin( 0.1, 0.3, 0.1, 0.1 );
  gPad->SetGrid( true, true );
  this->DrawRatioToPad( c, 2, title_ratio_lower, hists_ratio_lower, ymax );
  
  ////////////////////////////////////////////////////////
  // output
  string page_title = "Title:" + title_ratio_upper;
  c->Print( output_pdf_.c_str(), page_title.c_str() );  
}


void InttFineDelayScan::DrawHists()
{
  TCanvas* c = new TCanvas( "canvas", "title", 1600, 1200 );
  c->Print( (output_pdf_+"[").c_str() );
  //c->SetMargin( 0.1, 0.3, 0.1, 0.1); // L, R, B, T
  c->Divide( 1, 2 );
  gStyle->SetGridColor( kGray );

  c->cd(1)->SetMargin( 0.1, 0.3, 0.1, 0.1 );
  gPad->SetGrid( true, true );
  
  c->cd(2)->SetMargin( 0.1, 0.3, 0.1, 0.1 );
  gPad->SetGrid( true, true );
  
  string page_title_last = "last page";  
  gPad->SetGrid( true, true );
  gStyle->SetOptStat( 111111 );


  /////////////////////////////////////////
  // BCO diff, ratio (even)
  /////////////////////////////////////////
  this->DrawBcoDiffRatioPair( c,
			      "BCO full difference",
			      vector < TH3I* >{ begin(hist_fee_chip_bco_diff_), end(hist_fee_chip_bco_diff_) },
			      "Ratio of even bins of all FELIXes",
			      vector < TH1D* >{ hist_sidelobe_ratio_even_ }
			      );
  
  /////////////////////////////////////////
  // BCO diff, ratio (even), each FELIX  //
  /////////////////////////////////////////
  vector < TH1D* > hists_ratio_even_;
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      auto hist = hist_fee_chip_sidelobe_ratio_even_[ felix ]->ProjectionZ(); // ratio of all hits in a FELIX
      hists_ratio_even_.push_back( hist );

    }
  this->DrawBcoDiffRatioPair( c,
			      "BCO full difference",
			      vector < TH3I* >{ begin(hist_fee_chip_bco_diff_), end(hist_fee_chip_bco_diff_) },
			      "Ratio of even bins of each FELIX",
			      hists_ratio_even_,
			      true
			      );
  
  
  ////////////////////////////////////////////////
  // BCO diff, ratio (even), ROCs in each FELIX //
  ////////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num; felix++ )
    {

      string pad_title_base = "intt" + to_string( felix );
      string roc_upper = "RC-" + to_string( 2 * (felix%4) ) + (felix<4 ? "S" : "N");
      string roc_lower = "RC-" + to_string( 2 * (felix%4) +1 ) + (felix<4 ? "S" : "N");
            
      vector < TH1D* > hists;
      TH1D* hist_roc_upper = hist_fee_chip_sidelobe_ratio_even_[ felix ]
	->ProjectionZ( (pad_title_base + "_" + roc_upper).c_str(), 1, 7 );
      //hist_roc_upper->SetName( ( pad_title_base + "_" + roc_upper ).c_str() );
      hists.push_back( hist_roc_upper );
      
      TH1D* hist_roc_lower = hist_fee_chip_sidelobe_ratio_even_[ felix ]
	->ProjectionZ( ( pad_title_base + "_" + roc_lower ).c_str(), 7, 13 );
      //      hist_roc_lower->SetName( ( pad_title_base + "_" + roc_lower ).c_str() );
      
      for( int felix_ch=0; felix_ch<InttQa::kFee_num; felix_ch++ )
	{
	  string name = pad_title_base + "_FELIX_ch" + to_string( felix_ch );

	  auto hist = hist_fee_chip_sidelobe_ratio_even_[ felix ]
	    ->ProjectionZ( name.c_str(),
			   felix_ch+1, felix_ch+1
			   ); // ratio of all hits in a FELIX
	  hists.push_back( hist );

	  if( felix_ch == 6 )
	    hists.push_back( hist_roc_lower );

	}

      //hists.insert( hists.begin(), hist_al );

      this->DrawRatioPair( c,
			   (pad_title_base + " " + roc_upper ).c_str(),
			   vector < TH1D* >{ begin( hists ), begin(hists) + 7 + 1 },
			   (pad_title_base + " " + roc_lower ).c_str(),
			   vector < TH1D* >{ begin( hists ) + 7 + 1 , end(hists)},
			   false
			   );
    }
  /////////////////////////////////////////
  // BCO diff, ratio (odd)
  /////////////////////////////////////////
  this->DrawBcoDiffRatioPair( c,
			      "BCO full difference",
			      vector < TH3I* >{ begin(hist_fee_chip_bco_diff_), end(hist_fee_chip_bco_diff_) },
			      "Ratio of odd bins of all FELIXes",
			      vector < TH1D* >{ hist_sidelobe_ratio_odd_ }
			      );

  /////////////////////////////////////////
  // BCO diff raw, ratio (even)
  /////////////////////////////////////////
  this->DrawBcoDiffRatioPair( c,
			      "Raw BCO difference",
			      vector < TH3I* >{ begin(hist_fee_chip_bco_diff_raw_), end(hist_fee_chip_bco_diff_raw_) },
			      "Ratio of even bins of all FELIXes using raw BCO difference",
			      vector < TH1D* >{ hist_sidelobe_ratio_raw_even_ }
			      );
  
  /////////////////////////////////////////
  // BCO diff raw, ratio (odd)
  /////////////////////////////////////////
  this->DrawBcoDiffRatioPair( c,
			      "Raw BCO full difference",
			      vector < TH3I* >{ begin(hist_fee_chip_bco_diff_raw_), end(hist_fee_chip_bco_diff_raw_) },
			      "Ratio of odd bins of all FELIXes using raw BCO difference",
			      vector < TH1D* >{ hist_sidelobe_ratio_raw_odd_ }
			      );
    
  

  //  c->Print( output_pdf_.c_str(), "Title:Sidelobe ratio: even bins, Each FELIX" );

  
  ////////////////////////////////////////////////////////
  // correlation for test
  gPad->SetLogy( false );
  hist_correlation_->Draw( "colz" );
  
  c->Print( output_pdf_.c_str(), "Title:Sidelobe ratio: even bins, Each FELIX" );

  /*
  ////////////////////////////////////////////////////////
  // BCO dist
  gPad->SetLogy( false );
  hist_bco_->Draw( "" );
  InttQa::DrawStats( hist_bco_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:BCO dist" );

  ////////////////////////////////////////////////////////
  // BCO full dist
  gPad->SetLogy( false );
  hist_bco_full_->Draw( "" );
  InttQa::DrawStats( hist_bco_full_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:BCO full dist" );

  c->Clear();
  */

  c->Print( (output_pdf_+"]").c_str(), page_title_last.c_str() );
}


void InttFineDelayScan::ProcessHists()
{
  
  /////////////////////////////////////////////////////
  // 3D, Ladder vs BCO full vs Event counter (felix) //
  /////////////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {

      for( int felix_ch=0; felix_ch<InttQa::kFee_num; felix_ch++ )
	{

	  for( int chip=0; chip<InttQa::kChip_num; chip++ )
	    {

	      string name = "intt" + to_string( felix )
		+ "_FELIX_ch" + to_string( felix_ch )
		+ "_chip" + to_string( chip );
	      auto hist = hist_fee_chip_bco_diff_[ felix ]->ProjectionZ( name.c_str(),
									 felix_ch+1, felix_ch+1,
									 chip+1, chip+1 );
	      int counter[ 2 ] = {0};
	      for( int bin=1; bin<hist->GetNbinsX()+1; bin++ )
		{

		  // bin 1, 3, ... = BCO diff 0, 2, 4, ... -> odd
		  // bin 2, 4, ... = BCO diff 1, 3, 5, ... -> even
		  auto content = hist->GetBinContent( bin );
		  counter[ (bin-1)%2 ] += content;
		}
	      
	      double sidelobe_ratio_even = counter[1] / hist->Integral();
	      hist_fee_chip_sidelobe_ratio_even_[ felix ] 
		->Fill( felix_ch, chip, sidelobe_ratio_even );
	      
	      double sidelobe_ratio_odd = counter[0] / hist->Integral();
	      hist_fee_chip_sidelobe_ratio_odd_[ felix ] 
		->Fill( felix_ch, chip, sidelobe_ratio_odd );

	      // histgram integrated over all FELIX
	      int bin = hist_sidelobe_ratio_even_
		->Fill( sidelobe_ratio_even );

	      hist_sidelobe_ratio_odd_
		->Fill( sidelobe_ratio_odd );

	      // for debugging
	      hist_correlation_->Fill( sidelobe_ratio_even, sidelobe_ratio_odd );
	    } // end of for( chip )
	} // end of for( fee, i.e. felix_ch )
    } // end of for( felix )

  /////////////////////////////////////////////////////////
  // 3D, Ladder vs BCO full raw vs Event counter (felix) //
  /////////////////////////////////////////////////////////

  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {

      for( int felix_ch=0; felix_ch<InttQa::kFee_num; felix_ch++ )
	{

	  for( int chip=0; chip<InttQa::kChip_num; chip++ )
	    {

	      string name = "";
	      auto hist = hist_fee_chip_bco_diff_raw_[ felix ]->ProjectionZ( name.c_str(),
									     felix_ch+1, felix_ch+1,
									     chip+1, chip+1 );
	      int counter[ 2 ] = {0};
	      for( int bin=1; bin<hist->GetNbinsX()+1; bin++ )
		{

		  // bin 1, 3, ... = BCO diff 0, 2, 4, ... -> odd
		  // bin 2, 4, ... = BCO diff 1, 3, 5, ... -> even
		  auto content = hist->GetBinContent( bin );
		  counter[ (bin-1)%2 ] += content;
		}

	      double sidelobe_ratio_raw_even = counter[1] / hist->Integral();
	      hist_fee_chip_sidelobe_ratio_raw_even_[ felix ] 
		->Fill( felix_ch, chip, sidelobe_ratio_raw_even );
	      
	      double sidelobe_ratio_raw_odd = counter[0] / hist->Integral();
	      hist_fee_chip_sidelobe_ratio_raw_odd_[ felix ] 
		->Fill( felix_ch, chip, sidelobe_ratio_raw_odd );

	      // histgram integrated over all FELIX
	      hist_sidelobe_ratio_raw_even_
		->Fill( sidelobe_ratio_raw_even );
	      
	      hist_sidelobe_ratio_raw_odd_
		->Fill( sidelobe_ratio_raw_odd );

	    } // end of for( chip )
	} // end of for( fee, i.e. felix_ch )
    } // end of for( felix )

}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////

void InttFineDelayScan::SetOutputDir( string dir )
{
  if( dir != "" )
    {
      output_dir_ = dir;
    }

  string run_num_str = string( 8 - to_string(run_num_).size(), '0' ) + to_string( run_num_ );
  string suffix_official_private = "_official";
  if( is_official_ == false )
    suffix_official_private = "_special";
  
  output_root_ = output_dir_ + to_string( year_ ) + "/root/" + output_basename_ + run_num_str + suffix_official_private + ".root";
  output_pdf_  = output_dir_ + to_string( year_ ) + "/plots/" + output_basename_ + run_num_str + suffix_official_private + ".pdf";
  output_txt_  = output_dir_ + to_string( year_ ) + "/txt/" + output_basename_ + run_num_str + suffix_official_private + ".txt";
			      
}

/////////////////////////////////////////////////////////////////////////
// Fun4All stuff
/////////////////////////////////////////////////////////////////////////

int InttFineDelayScan::Init(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScan::InitRun(PHCompositeNode *topNode)
{
  // I want to get the run number here... 
  // auto status = this->GetNodes( topNode );
  // if (status == Fun4AllReturnCodes::ABORTEVENT)
  //   return Fun4AllReturnCodes::ABORTEVENT;

  // If an user sets the run number to recoConsts, it works. Can I assume it?
  recoConsts *rc = recoConsts::instance();
  run_num_ = rc->get_IntFlag( "RUNNUMBER" );

  this->SetOutputDir();  
  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );

  /////////////////////////////////////////////
  // Init 3D hists                           //
  /////////////////////////////////////////////
  string name_all = "All_FELIXes";
  double ratio_min = -0.1;
  double ratio_max = 1.1;
  double ratio_bin_num = (ratio_max - ratio_min ) * 200;
  
  //////////////////////////////////////////////////////
  // 1D hists for BCO diff
  string name_sidelobe_ratio_even_ = name_all + "_bco_full_event_counter";
  string title_sidelobe_ratio_even_ = name_all + ";BCOdiff_{even}/(BCOdiff_{odd} + BCOdiff_{even});Entries";
  hist_sidelobe_ratio_even_
    = new TH1D ( name_sidelobe_ratio_even_.c_str(),
		 title_sidelobe_ratio_even_.c_str(),
		 ratio_bin_num, ratio_min, ratio_max );
  
  string name_sidelobe_ratio_odd_ = name_all + "_bco_full_oddt_counter";
  string title_sidelobe_ratio_odd_ = name_all + ";BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{odd});Entries";
  hist_sidelobe_ratio_odd_
    = new TH1D ( name_sidelobe_ratio_odd_.c_str(),
		 title_sidelobe_ratio_odd_.c_str(),
		 ratio_bin_num, ratio_min, ratio_max );

  //////////////////////////////////////////////////////
  // 1D hists for BCO diff raw
  string name_sidelobe_ratio_raw_even_ = name_all + "_bco_full_raw_event_counter";
  string title_sidelobe_ratio_raw_even_ = name_all + ";BCOdiff_{even}/(BCOdiff_{odd} + BCOdiff_{even});Entries";
  hist_sidelobe_ratio_raw_even_
    = new TH1D ( name_sidelobe_ratio_raw_even_.c_str(),
		 title_sidelobe_ratio_raw_even_.c_str(),
		 ratio_bin_num, ratio_min, ratio_max );
  
  string name_sidelobe_ratio_raw_odd_ = name_all + "_bco_full_raw_oddt_counter";
  string title_sidelobe_ratio_raw_odd_ = name_all + ";BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{odd});Entries";
  hist_sidelobe_ratio_raw_odd_
    = new TH1D ( name_sidelobe_ratio_raw_odd_.c_str(),
		 title_sidelobe_ratio_raw_odd_.c_str(),
		 ratio_bin_num, ratio_min, ratio_max );

  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      string name = "intt" + to_string( felix );

      //////////////////////////////////////////////////////
      // hists for BCO diff

      // BCO diff
      string name_fee_chip_bco_diff = name + "_fee_chip_bco_diff";
      string title_fee_chip_bco_diff = name + ";FELIX_CH;Chip; BCOfull%0x7f - BCO";
      hist_fee_chip_bco_diff_[ felix ]
	 = new TH3I( name_fee_chip_bco_diff.c_str(), title_fee_chip_bco_diff.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     InttQa::kBco_max, 0, InttQa::kBco_max );

      // BCO diff ratio (even)
      string name_fee_chip_sidelobe_ratio_even = name + "_bco_full_event_counter";
      string title_fee_chip_sidelobe_ratio_even = name + ";FELIX_CH;Chip;BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{even})";
      hist_fee_chip_sidelobe_ratio_even_[ felix ]
	= new TH3I ( name_fee_chip_sidelobe_ratio_even.c_str(),
		     title_fee_chip_sidelobe_ratio_even.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     ratio_bin_num, ratio_min, ratio_max );
		     
      // BCO diff ratio (odd)
      string name_fee_chip_sidelobe_ratio_odd = name + "_bco_full_odd_counter";
      string title_fee_chip_sidelobe_ratio_odd = name + ";FELIX_CH;Chip;BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{odd})";
      hist_fee_chip_sidelobe_ratio_odd_[ felix ]
	= new TH3I ( name_fee_chip_sidelobe_ratio_odd.c_str(),
		     title_fee_chip_sidelobe_ratio_odd.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     ratio_bin_num, ratio_min, ratio_max );

      //////////////////////////////////////////////////////
      // hists for BCO diff raw
      // BCO diff raw
      string name_fee_chip_bco_diff_raw = name + "_fee_chip_bco_diff_raw";
      string title_fee_chip_bco_diff_raw = name + ";FELIX_CH;Chip; BCOfull%0x7f - BCO (raw)";
      hist_fee_chip_bco_diff_raw_[ felix ]
	 = new TH3I( name_fee_chip_bco_diff_raw.c_str(), title_fee_chip_bco_diff_raw.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     2 * InttQa::kBco_max, -InttQa::kBco_max, InttQa::kBco_max );

      // BCO diff raw ratio (even)
      string name_fee_chip_sidelobe_ratio_raw_even = name + "_bco_full_raw_event_counter";
      string title_fee_chip_sidelobe_ratio_raw_even = name + ";FELIX_CH;Chip;BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{even})";
      hist_fee_chip_sidelobe_ratio_raw_even_[ felix ]
	= new TH3I ( name_fee_chip_sidelobe_ratio_raw_even.c_str(),
		     title_fee_chip_sidelobe_ratio_raw_even.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     ratio_bin_num, ratio_min, ratio_max );
		     
      // BCO diff raw ratio (odd)
      string name_fee_chip_sidelobe_ratio_raw_odd = name + "_bco_full_raw_odd_counter";
      string title_fee_chip_sidelobe_ratio_raw_odd = name + ";FELIX_CH;Chip;BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{odd})";
      hist_fee_chip_sidelobe_ratio_raw_odd_[ felix ]
	= new TH3I ( name_fee_chip_sidelobe_ratio_raw_odd.c_str(),
		     title_fee_chip_sidelobe_ratio_raw_odd.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     ratio_bin_num, ratio_min, ratio_max );
		     
      //TH3I* hist_fee_chip_sidelobe_ratioodd_[ InttQa::kFelix_num ]; // BCOdiff_{even} / (BCOdiff_{odd} + BCOdiff_{even} )
    }

  hist_correlation_ = new TH2D( "even_odd_correlation", "even_odd_correlation;even;odd",
				ratio_bin_num, 0.0, ratio_max,
				ratio_bin_num, 0.0, ratio_max );


  hist_bco_      = new TH1I( "bco", "BCO distribution;BCO;Entries", 128, 0, 128 );
  InttQa::HistConfig( hist_bco_ );
  
  hist_bco_full_ = new TH1D( "bco_full", "BCO full distribution;BCO full;Entries",
			     100, 0, TMath::Power( 2, 40 ) );
  InttQa::HistConfig( hist_bco_full_ );

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScan::process_event(PHCompositeNode *topNode)
{

  // Get nodes to access data
  auto status = this->GetNodes(topNode);

  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  auto raw_hit_num = node_inttrawhit_map_->get_nhits();  
  if( raw_hit_num == 0 )
    return Fun4AllReturnCodes::EVENT_OK;
    
  uint64_t bco_full;
  if( intt_header_found_ == true )
    {
      bco_full = node_intteventheader_map_->get_bco_full();
    }
  else
    {
      bco_full = (node_inttrawhit_map_->get_hit( 0 )->get_bco());
    }
  
  hist_bco_full_->Fill( bco_full );

  auto hits = this->GetHits();
  
  // loop over all raw hits
  bool found = false;
  for (unsigned int i = 0; i < hits.size(); i++)
  {
    auto hit = hits[i];

    int felix = hit->get_packetid() - InttQa::kFirst_pid;
    int felix_ch = hit->get_fee();
    int chip = hit->get_chip_id();
    int chan = hit->get_channel_id();
    auto bco = hit->get_FPHX_BCO();
    
    int bco_diff = (bco_full & 0x7f ) - bco;

    // fill BCO diff before adding 128. It's called BCO diff raw
    hist_fee_chip_bco_diff_raw_[ felix ]->Fill( felix_ch, chip, bco_diff );
    
    if( bco_diff < 0 )
      bco_diff += 128;
    
    // The raw Chip ID can be more than 26
    if (chip > InttQa::kChip_num)
    {
      chip = chip - InttQa::kChip_num;
    }

    hist_fee_chip_bco_diff_[ felix ]->Fill( felix_ch, chip, bco_diff );
    //    hist_bco_->Fill(hit->get_FPHX_BCO());

   if( false )
    {
      found = true;
      cout << setw(6) << i << " "
	//<< setw(13) << event_counter_ref
           << setw(10) << hit->get_event_counter() << " "
           << setw(4) << "felix " << hit->get_packetid() << " " // Packet ID
           << setw(3) << hit->get_fee() << " "                  // FELIX CH (module)
           << setw(4) << chip << " "                            // chip_id
           << setw(4) << hit->get_channel_id() << " "           // chan_id
           << setw(3) << hit->get_adc() << " "                  // adc
           << setw(3) << hit->get_FPHX_BCO() << " "             // bco
           << setw(12) << hit->get_bco() << " "                 // bco_full
           << setw(3) << (hit->get_bco() & 0x7F) - hit->get_FPHX_BCO() << " "
           << endl;
    }
  } // end of for( hits )

  if( found )
    {
	cout << setw(6) << "index" << " "
	     << setw(30) << "Eve Cnter Ref" << " "
	     << setw(10) << "Eve Cnter" << " "
	     << setw(4) << "felix pid "
	     << setw(3) << "fee" << " "        // FELIX CH (module)
	     << setw(4) << "chip" << " "
	     << setw(4) << "chan" << " "
	     << setw(3) << "ADC" << " " 
	     << setw(3) << "BCO" << " " 
	     << setw(12) << "BCO full" << " "
	     << setw(3) << "BCO diff" << " "
	     << endl;

    }

  //this->process_event_clone_hit( topNode );
  return Fun4AllReturnCodes::EVENT_OK;
}


int InttFineDelayScan::ResetEvent(PHCompositeNode *topNode)
{

  intt_header_found_ = false;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScan::EndRun(const int runnumber)
{

  this->ProcessHists();
  
  // // Draw hits and save them into a PDF file.
  this->DrawHists();
  
  // write 3D hists into a ROOT file

  // write 1D hists into a ROOT file
  tf_output_->WriteTObject( hist_bco_, hist_bco_->GetName() );
  tf_output_->WriteTObject( hist_bco_full_, hist_bco_full_->GetName() );

  for( int felix=0; felix<InttQa::kFelix_num; felix++ )
    {
      tf_output_->WriteTObject( hist_fee_chip_sidelobe_ratio_even_[ felix ],
				hist_fee_chip_sidelobe_ratio_even_[ felix ]->GetName() );
      
      tf_output_->WriteTObject( hist_fee_chip_sidelobe_ratio_odd_[ felix ],
				hist_fee_chip_sidelobe_ratio_odd_[ felix ]->GetName() );
      
    }

  // Close the ROOT file
  tf_output_->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScan::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScan::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttFineDelayScan::Print(const std::string &what) const
{
  int width = 100;
  cout << string( width, '-' ) << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << "  - Output  (PDF): " << output_pdf_ << endl;
  cout << "  - Output  (txt): " << output_txt_ << endl;
  cout << string( width, '-' ) << endl;
  // this->PrintLine( "Class", "InttFineDelayScan" );
//   this->DumpPrintBuffer();
}
