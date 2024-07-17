#include "InttRawHitQA.h"

using namespace std;

//____________________________________________________________________________..
InttRawHitQA::InttRawHitQA(const std::string &name, bool is_official ):
  SubsysReco(name),
  is_official_( is_official )
{

}

//____________________________________________________________________________..
InttRawHitQA::~InttRawHitQA()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////
int InttRawHitQA::GetNodes(PHCompositeNode *topNode)
{
  /////////////////////////////////////////////////////////////////////////
  // INTT event header

  if( is_official_ == false )
    {
      /*
	string node_name_intteventheader = "INTTEVENTHEADER";
      node_intteventheader_map_ =
	findNode::getClass<InttEventInfo>(topNode, node_name_intteventheader);
      
      if (!node_intteventheader_map_)
	{
	  cerr << PHWHERE << node_name_intteventheader << " node is missing." << endl;
	  intt_header_found_ = false;
	  //return Fun4AllReturnCodes::ABORTEVENT;
	}
  
      intt_header_found_ = true;
      */
    }
  else
    {
      intt_header_found_ = false;
    }
  
  intt_header_found_ = false;
  
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

vector < InttRawHit* > InttRawHitQA::GetHits()
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

/*
void InttRawHitQA::PrintLine( string head, string contents, string separator )
{
  printed_contents_.push_back( head + separator + " " + contents );
}

void InttRawHitQA::DumpPrintBuffer()
{
  if( printed_contents_.size() == 0 )
  return;

  vector < int > line_lengths;
  for( auto& line : printed_contents_ )
    line_lengths.push_back( line.size() );

  auto max_length = *max_element( line_lengths.begin(), line_lengths.end() );
  cout << "+-" << string( max_length , '-' ) << "-+\n";
  for( auto& line : printed_contents_ )
    {
      cout << "| " + line + string( max_length - line.size(), ' ' ) << " |\n";
    }
  cout << "+-" << string( max_length , '-' ) << "-+\n";
  
  printed_contents_.clear();
}
*/


void InttRawHitQA::DrawHists()
{
  TCanvas* c = new TCanvas( "canvas", "title", 1600, 1200 );
  c->Print( (output_pdf_+"[").c_str() );
  string page_title_last = "last page";
  
  gPad->SetGrid( true, true );
  gStyle->SetOptStat( 111111 );
  gStyle->SetOptFit( true );
  gStyle->SetGridColor( kGray );

  ////////////////////////////////////////////////////////
  // Event counter vs FELIX
  c->SetMargin( 0.1, 0.2, 0.1, 0.1); // L, R, B, T
  gPad->SetLogy( false );
  InttQa::HistsConfig( InttQa::kFelix_num, hist_event_counter_ );
  
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      auto hist = hist_event_counter_[ felix ];
      int bin_width = hist->GetBinWidth( 1 );

      string title = "Event Counter;Event Counter;Entries/" + to_string(bin_width) + "events";
      hist->SetTitle( title.c_str() );
      
      string option = "HISTE";
      if( felix > 0 )
	option = "HISTE sames";
      auto ranges = InttQa::OptimizeRange( hist );
      hist->Draw( option.c_str() );

      double xmin = 0.801 + 0.099 * (felix<4 ? 0 : 1) ;
      double ymin = 0.6 - 0.15 * (felix<4 ? felix : felix-4 );
      InttQa::DrawStats( hist,
			 xmin,
			 ymin,
			 xmin + 0.099,
			 ymin + 0.15 );
      
    }
  
  c->BuildLegend( 0.801, 0.6 + 0.125, 0.999, 0.99, "Event counter" );
  c->Print( output_pdf_.c_str(), "Title:Event counter dist" );
  c->Clear();
  c->SetMargin( 0.1, 0.1, 0.1, 0.1); // L, R, B, T

  ////////////////////////////////////////////////////////
  // FELIX dist
  gPad->SetLogy( true );
  hist_pid_->Draw( "HISTE TEXT0" );
  InttQa::DrawStats( hist_pid_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:FELIX dist" );

  ////////////////////////////////////////////////////////
  // FELIX ch dist for each FELIX
  c->Clear();
  c->Divide( 4, 2 );
  vector < double > contents;
  for( int felix=0; felix<InttQa::kFelix_num; felix++ )
    {
      auto hist = hist_fee_chip_chan_[ felix ]->ProjectionX();
      for( int i=1; i<hist->GetNbinsX()+1; i++ )
	if(  hist->GetBinContent(i) > 0 )
	  contents.push_back( hist->GetBinContent(i) );
    }
  
  auto y_min = *min_element( contents.begin(), contents.end() );
  auto y_max = *max_element( contents.begin(), contents.end() ) * 1.1;
  
  for( int felix=0; felix<InttQa::kFelix_num; felix++ )
    {
      auto pad = c->cd( felix+1 );
      pad->SetMargin( 0.125, 0.15, 0.075, 0.1 );
      gPad->SetGrid( true, true );
      
      auto hist = hist_fee_chip_chan_[ felix ]->ProjectionX();
      hist->SetTitle( ("intt" + to_string(felix) + ";FELIX CH;Entries/event").c_str() );
      
      InttQa::HistConfig( hist );
      gPad->SetLogy( true );
      hist->GetYaxis()->SetMoreLogLabels( true );
      hist->GetYaxis()->SetRangeUser( y_min, y_max );
      
      hist->Draw( "HISTE TREXT0" );
      InttQa::DrawStats( hist, 0.7, 0.775, 0.99, 0.99 );
      
      //for( int felix_ch=0; felix_ch<InttQa::kFelix_ch_num; felix_ch++ )

    }
  c->Print( output_pdf_.c_str(), "Title:FELIX dist" );
  c->Clear();
  c->SetMargin( 0.1, 0.1, 0.1, 0.1); // L, R, B, T

  ////////////////////////////////////////////////////////
  // #hit dist
  gPad->SetLogy( true );
  InttQa::OptimizeRange( hist_nhit_ );
  hist_nhit_->Draw( "HISTE" );
  InttQa::DrawStats( hist_nhit_, 0.7, 0.6, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:Hit num per event" );

  // #hit dist
  gPad->SetLogy( true );
  InttQa::OptimizeRange( hist_nhit_south_ );
  hist_nhit_south_->Draw( "HISTE" );
  InttQa::DrawStats( hist_nhit_south_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:# Hit South" );

  // #hit dist
  gPad->SetLogy( true );
  InttQa::OptimizeRange( hist_nhit_north_ );
  hist_nhit_north_->Draw( "HISTE" );
  InttQa::DrawStats( hist_nhit_north_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:# Hit North" );
  
  ////////////////////////////////////////////////////////
  // ADC dist
  gPad->SetLogy( true );
  hist_adc_->Draw( "HISTE TEXT0" );
  InttQa::DrawStats( hist_adc_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:ADC dist" );

  ////////////////////////////////////////////////////////
  // BCO dist
  gPad->SetLogy( false );
  hist_bco_->Draw( "HISTE" );
  InttQa::DrawStats( hist_bco_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:BCO dist" );

  ////////////////////////////////////////////////////////
  // BCO full dist
  gPad->SetLogy( false );
  hist_bco_full_->Draw( "HISTE" );
  InttQa::DrawStats( hist_bco_full_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str(), "Title:BCO full dist" );

  ////////////////////////////////////////////////////////
  // Event counter diff vs FELIX
  c->SetMargin( 0.1, 0.2, 0.1, 0.1); // L, R, B, T
  gPad->SetLogy( false );
  InttQa::HistsConfig( InttQa::kFelix_num, hist_event_counter_diff_ );
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      auto hist = hist_event_counter_diff_[ felix ];

      string option = "";
      if( felix > 0 )
	option = "sames";
      InttQa::OptimizeRange( hist );
      hist->Draw( option.c_str() );
      double xmin = 0.801 + 0.099 * (felix<4 ? 0 : 1) ;
      double ymin = 0.6 - 0.15 * (felix<4 ? felix : felix-4 );
      InttQa::DrawStats( hist,
			 xmin,
			 ymin,
			 xmin + 0.099,
			 ymin + 0.15 );
    }
  
  c->BuildLegend( 0.801, 0.6 + 0.125, 0.999, 0.99, "Event counter difference" );
  c->Print( output_pdf_.c_str(), "Title:#DeltaEvent counter dist" );
  c->Clear();
  c->SetMargin( 0.1, 0.1, 0.1, 0.1); // L, R, B, T
  
  ////////////////////////////////////////////////////////
  // Hitmap
  c->SetMargin( 0.1, 0.1, 0.1, 0.1); // L, R, B, T
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      c->Clear();
      //      c->SetCanvasSize( 800, 1200 );
      c->Divide( 1, InttQa::kFee_num, 0.01, 0 );

      for( int ladder=0; ladder<InttQa::kFee_num; ladder++ )
	{

	  auto frame = c->cd( InttQa::kFee_num - ladder );
	  //auto frame = c->cd( ladder+1 );
	  frame->SetMargin( 0.1, 0.15, 0.1, 0.02 );
	  gPad->SetGrid( true, true );
	  
	  string title = ";;CH" + to_string( ladder );
	  hist_hitmap_[ felix ][ ladder ]->SetTitle( title.c_str() );
	  hist_hitmap_[ felix ][ ladder ]->SetLineColor( kBlack );
	  hist_hitmap_[ felix ][ ladder ]->GetXaxis()->SetLabelSize( 0.1 );
	  
	  hist_hitmap_[ felix ][ ladder ]->GetYaxis()->CenterTitle();
	  hist_hitmap_[ felix ][ ladder ]->GetYaxis()->SetTitleSize( 0.3 );
	  hist_hitmap_[ felix ][ ladder ]->GetYaxis()->SetTitleOffset( 0.15 );
	  hist_hitmap_[ felix ][ ladder ]->GetYaxis()->SetLabelSize( 0.15 );
	  
	  hist_hitmap_[ felix ][ ladder ]->Draw( "colz" );
	  
	  InttQa::DrawStats( hist_hitmap_[ felix ][ ladder ], 0.9, 0, 1.0, 1.0 );
	  // InttQa::DrawPaletteAxis( hist_hitmap_[ felix ][ ladder ], 0.86, 0.0, 0.88, 1.0, 0.07 );
	  
	}

      string page_title = "Title:Hitmap of intt" + to_string( felix );
      c->Print( output_pdf_.c_str(), page_title.c_str() );

      page_title_last = page_title;
    }
  c->Clear();
  
  c->Print( (output_pdf_+"]").c_str(), page_title_last.c_str() );
}


void InttRawHitQA::ProcessHists()
{

  //////////////////////////////////////////////////////////////////
  // Normalization using the last event counter                   //
  //////////////////////////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num; felix++ )
    {
      hist_fee_chip_chan_[ felix ]			->Scale( 1.0 / event_counter_by_myself_ );
      hist_fee_bco_full_event_counter_[ felix ]		->Scale( 1.0 / event_counter_by_myself_ );
      hist_fee_bco_full_event_counter_diff_[ felix ]	->Scale( 1.0 / event_counter_by_myself_ );
    }
  
  hist_nhit_		->Scale( 1.0 / event_counter_by_myself_ ); 
  hist_nhit_south_	->Scale( 1.0 / event_counter_by_myself_ ); 
  hist_nhit_north_	->Scale( 1.0 / event_counter_by_myself_ ); 
  hist_adc_		->Scale( 1.0 / event_counter_by_myself_ );
  hist_bco_		->Scale( 1.0 / event_counter_by_myself_ ); 
  hist_bco_full_	->Scale( 1.0 / event_counter_by_myself_ );

  //////////////////////////////////////////////////////////////////
  // pid dist                                                     //
  //////////////////////////////////////////////////////////////////
  for( int i=0; i<InttQa::kFelix_num; i++ )
    {
      //cout << hist_fee_chip_chan_[i]->GetEntries() << endl;
      hist_pid_->SetBinContent( i+1, hist_fee_chip_chan_[i]->GetEntries() );
    }
  hist_pid_->Scale( 1.0 / event_counter_by_myself_ );

  //  TH2I* hist_hitmap[ InttQa::kFelix_num ][ InttQa::kFee_num ];
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {

      for( int ladder=0; ladder<InttQa::kFee_num; ladder++ )
	{
	  hist_fee_chip_chan_[ felix ]->GetXaxis()->SetRange( ladder+1, ladder+1 );
	  //	  hist_fee_chip_chan_woclonehit_[ felix ]->GetXaxis()->SetRange( ladder+1, ladder+1 );
	  //hist_fee_chip_chan_[ felix ]->GetXaxis()->SetRange( 2, 5 );
	  // TProfile2D* Project3DProfile(Option_t* option = "xy") const     // *MENU*

	  string name = "yz_" + to_string( ladder );
	  hist_hitmap_[ felix ][ ladder ] = hist_fee_chip_chan_[ felix ]->Project3DProfile( name.c_str() );
	  hist_fee_chip_chan_[ felix ]->GetXaxis()->SetRange(); // set to the whole range

	}

    }

  //////////////////////////////////////////////////////////////////
  // 3D, Ladder vs BCO full vs Event counter (felix)              //
  //////////////////////////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      hist_event_counter_[ felix ]
	= hist_fee_bco_full_event_counter_[ felix ]->ProjectionZ( "_pz" );
	//	= dynamic_cast<TH1D*>(hist_fee_bco_full_event_counter_[ felix ]->ProjectionZ( "_pz" )->Rebin( 100000 ));

      hist_event_counter_diff_[ felix ]
	= hist_fee_bco_full_event_counter_diff_[ felix ]->ProjectionZ( "_pz" );
    }

  
}

void InttRawHitQA::WriteResults2Txt()
{
  ofstream output( output_txt_.c_str(), ios::app );
  if( output.fail() )
    {
      cerr << output_txt_ << " cannot be opened." << endl;
      cerr << "Nothing written..." << endl;
      return;
    }

  output << 1 ;
  output.close();

  cout << output_txt_ << endl;
}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////

void InttRawHitQA::SetOutputDir( string dir )
{
  if( dir != "" )
    {
      output_dir_ = dir;
    }

  string run_num_str = string( 8 - to_string(run_num_).size(), '0' ) + to_string( run_num_ );
  string suffix_official_private = "_official";
  if( is_official_ == false )
    suffix_official_private = "_special";
  
  output_root_ = output_dir_ + to_string( year_ ) + "/root/"
    + output_basename_ + run_num_str + suffix_official_private + ".root";

  output_pdf_  = output_dir_ + to_string( year_ ) + "/plots/"
    + output_basename_ + run_num_str + suffix_official_private + ".pdf";

  output_txt_  = output_dir_ + to_string( year_ ) + "/txt/"
    + output_basename_ + run_num_str + suffix_official_private + ".txt";
			      
}

/////////////////////////////////////////////////////////////////////////
// Fun4All stuff
/////////////////////////////////////////////////////////////////////////

int InttRawHitQA::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::InitRun(PHCompositeNode *topNode)
{

  // I want to get the run number here... 
  // auto status = this->GetNodes( topNode );
  // if (status == Fun4AllReturnCodes::ABORTEVENT)
  //   return Fun4AllReturnCodes::ABORTEVENT;

  // If an user sets the run number to recoConsts, it works. Can I assume it?
  recoConsts *rc = recoConsts::instance();
  run_num_ = rc->get_IntFlag( "RUNNUMBER" );

  pid_ref_ = 3001; // how can I get the smallest pid from INTT?
  
  this->SetOutputDir();  
  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );

  /////////////////////////////////////////////
  // Init 3D hists                           //
  /////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      string name = "intt" + to_string( felix );
      string title = name + ";FELIX CH;Chip;Channel;Entries/event";
      hist_fee_chip_chan_[ felix ]
	= new TH3D( name.c_str(), title.c_str(),
		    InttQa::kFee_num, 0, InttQa::kFee_num,
		    InttQa::kChip_num, 1, InttQa::kChip_num+1,
		    InttQa::kChan_num, 0, InttQa::kChan_num );

      // string name_woclonehit = name + "_woclonehit";
      // string title_woclonehit = title + ";woclonehit";
      // hist_fee_chip_chan_woclonehit_[felix] = new TH3D(name_woclonehit.c_str(), title_woclonehit.c_str(),
      //                                                InttQa::kFee_num, 0, InttQa::kFee_num,
      //                                                InttQa::kChip_num, 1, InttQa::kChip_num + 1,
      //                                                InttQa::kChan_num, 0, InttQa::kChan_num);

      string name_bco_event = name + "_ladder_bco_full_event_counter";
      string title_bco_event = name + ";FELIX_CH;BCO full;Event Counter;Entries/event";
      hist_fee_bco_full_event_counter_[ felix ]
	= new TH3D( name_bco_event.c_str(), title_bco_event.c_str(),
		    InttQa::kFee_num, 0, InttQa::kFee_num,
		    100, 0, TMath::Power(2, 40),
		    1e4, 0, 1e7 );
      
      string name_bco_event_diff = name + "_ladder_bco_full_event_counter_diff";
      string title_bco_event_diff = name + ";FELIX_CH;#Delta BCO full;#Delta Event Counter;Entries/event";

      int max = 1000;
      hist_fee_bco_full_event_counter_diff_[ felix ]
	= new TH3D( name_bco_event_diff.c_str(), title_bco_event_diff.c_str(),
		    InttQa::kFee_num, 0, InttQa::kFee_num,
		    2 * max / 100, -max, max,
		    2 * max / 100, -max, max );
    }

  /////////////////////////////////////////////
  // Init 2D hists                           //
  /////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {

      for( int ladder=0; ladder<InttQa::kFee_num; ladder++ )
	{
	  string name = "intt" + to_string( felix ) + "_" + to_string( ladder );
	  string title = name + ";Chip;Channel;Entries/event";
	  //hist_hitmap_[ felix ][ ladder ] = new TH2I( name.c_str(), title.c_str(),
	  hist_hitmap_[ felix ][ ladder ] = new TProfile2D( name.c_str(), title.c_str(),
							    InttQa::kChip_num, 1, InttQa::kChip_num,
							    InttQa::kChan_num, 0, InttQa::kChan_num );
	}
    }

  /////////////////////////////////////////////
  // Init 1D hists                           //
  /////////////////////////////////////////////
  hist_nhit_ = new TH1D( "nhit", "#INTTRAWHIT per event;#hit;Entries/event", 1e4, 0, 1e4 );
  InttQa::HistConfig( hist_nhit_ );
 
  hist_nhit_south_ = new TH1D( "nhit_south", "#INTTRAWHIT South;event;#hit/event", 1e4, 0, 1e7 );
  InttQa::HistConfig( hist_nhit_south_ );
  
  hist_nhit_north_ = new TH1D( "nhit_north", "#INTTRAWHIT North;event;#hit/event", 1e4, 0, 1e7 );
  InttQa::HistConfig( hist_nhit_north_ );

  hist_pid_      = new TH1D ( "pid", "Packet ID distribution;pid;Entries/event", InttQa::kFelix_num, InttQa::kFirst_pid, InttQa::kFirst_pid + InttQa::kFelix_num );
  InttQa::HistConfig( hist_pid_ );
  
  hist_adc_      = new TH1D( "adc", "ADC distribution;ADC;Entries/event", 8, 0, 8 );
  InttQa::HistConfig( hist_adc_ );
  
  hist_bco_      = new TH1D( "bco", "BCO distribution;BCO;Entries/event",
			     InttQa::kBco_max+10, -5, InttQa::kBco_max+5 );
  InttQa::HistConfig( hist_bco_ );
  
  hist_bco_full_ = new TH1D( "bco_full", "BCO full distribution;BCO full;Entries/event",
			     100, 0, TMath::Power( 2, 40 ) );
  InttQa::HistConfig( hist_bco_full_ );

  ////////////////////////////////////////////////////////////
  // g_event_hit_num_ = new TGraph();
  // g_event_hit_num_->SetName( "g_event_hit_num" );
  
  // g_event_hit_num_strange_peak_ = new TGraph();
  // g_event_hit_num_strange_peak_->SetName( "g_event_hit_num_strange_peak" );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::process_event(PHCompositeNode *topNode)
{

  //////////////////////////////////////////////////////////////////
  // node processes                                               //
  //////////////////////////////////////////////////////////////////
  // Get nodes to access data
  auto status = this->GetNodes(topNode);

  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  //////////////////////////////////////////////////////////////////
  // Get vector < InttRawHit* >                                   //
  //////////////////////////////////////////////////////////////////
  auto hits = this->GetHits();
  
  auto raw_hit_num = hits.size();
  hist_nhit_->Fill( raw_hit_num );
  
  // if no raw hit is found, skip this event
  if( raw_hit_num == 0 )
    return Fun4AllReturnCodes::EVENT_OK;

  event_counter_by_myself_++;
  
  //////////////////////////////////////////////////////////////////
  // processes for each event                                     //
  //////////////////////////////////////////////////////////////////
  uint64_t bco_full;
  if( intt_header_found_ == true )
    {
      //bco_full = node_intteventheader_map_->get_bco_full();
    }
  else
    {
      bco_full = (node_inttrawhit_map_->get_hit( 0 )->get_bco());
    }
  hist_bco_full_->Fill( bco_full );
  
  //////////////////////////////////////////////////////////////////
  // primary raw hit sweep to get some reference values           //
  //////////////////////////////////////////////////////////////////
  uint32_t event_counter_ref = node_inttrawhit_map_->get_hit(0)->get_event_counter();

  /*
  uint32_t event_counter_ref = 100000000; // 100 M
  for (unsigned int i = 0; i < raw_hit_num; i++)
  {
  auto hit = node_inttrawhit_map_->get_hit(i);

    // Because *min_element crashed the Fun4All process, I have to do it...
    if( hit->get_event_counter() < event_counter_ref )
      {
	event_counter_ref = hit->get_event_counter();
      }
      
  }
  */
  
  //////////////////////////////////////////////////////////////////
  // processes for each raw hit                                   //
  //////////////////////////////////////////////////////////////////
  // loop over all raw hits
  bool found = false;
  for (unsigned int i = 0; i < hits.size(); i++)
  {
    auto hit = hits[i];

    int felix		= hit->get_packetid() - InttQa::kFirst_pid;
    int felix_ch	= hit->get_fee();

    // uint16_t InttRawHit::get_chip_id
    int chip		= hit->get_chip_id();    
    if(chip  > InttQa::kChip_num)
      chip = chip - InttQa::kChip_num;
    
    int chan		= hit->get_channel_id();
    auto adc		= hit->get_adc();
    auto bco		= hit->get_FPHX_BCO();
    int event_counter	= hit->get_event_counter(); // uint32_t IttRawHit::get_event_counter()
    if( is_first_event_ == true )
      {
	event_counter = 0;
      }
    else if( event_counter - previous_event_counter_ > 1000 )
      {
	event_counter = -1; // it means bad
      }
    else
      {
	//cout << event_counter << endl;
	last_event_counter_ = event_counter;
      }

    // cout << hit->get_event_counter() << "\t"
    // 	 << event_counter << "\t"
    // 	 << last_event_counter_ << "\t"
    // 	 << previous_event_counter_ << endl;

    int bco_diff = 0;
    if( (bco_full & 0x7f )  > bco )
      bco_diff = int(bco_full & 0x7f ) - bco;
    else
      bco_diff = int(bco_full & 0x7f ) + (128 - bco);
        
    //////////////////////////////////////////////////////////////////
    // Filling hists                                                //
    //////////////////////////////////////////////////////////////////

    hist_fee_chip_chan_[felix]->Fill( felix_ch, chip, chan );
    hist_adc_->Fill(hit->get_adc());
    hist_bco_->Fill(hit->get_FPHX_BCO());

    hist_fee_bco_full_event_counter_[felix]
        ->Fill(felix_ch, // chip, chan );
               hit->get_bco(),
               event_counter);

    hist_fee_bco_full_event_counter_diff_[felix]
        ->Fill(felix_ch,
               hit->get_bco() - bco_full,
               event_counter - event_counter_ref);

    if (felix < 4)
      hist_nhit_south_->Fill(event_counter);
    else
      hist_nhit_north_->Fill(event_counter);

    if( found )
      {
	cout << setw(6) << "index" << " "
	  //	     << setw(30) << "Eve Cnter Ref" << " "
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

    if( false )
    //if (event_counter - event_counter_ref != 0)
    //if( !found )
    {
      found = false;
      cout << setw(6) << i << " "
	//<< setw(13) << event_counter_ref
	
           << setw(10) << hit->get_event_counter() << " "
           << setw(10) << event_counter << " "
           << setw(4) << "felix " << hit->get_packetid() << " " // Packet ID
           << setw(3) << hit->get_fee() << " "                  // FELIX CH (module)
           << setw(4) << chip << " "                            // chip_id
           << setw(4) << hit->get_channel_id() << " "           // chan_id
           << setw(3) << hit->get_adc() << " "                  // adc
           << setw(3) << hit->get_FPHX_BCO() << " "             // bco
           << setw(12) << hit->get_bco() << " "                 // bco_full
           << setw(3) << bco_diff
           << endl;
    }
  }

  //this->process_event_clone_hit( topNode );
  is_first_event_ = false;

  if( last_event_counter_ - previous_event_counter_ < 1000 )
    previous_event_counter_ = last_event_counter_; // in the case of reasonable event counter
  else
    previous_event_counter_ = -1; // in the case of a crazy event counter
  
  //cout << "-------------------------------------------------" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

// int InttRawHitQA::process_event_clone_hit(  PHCompositeNode* topNode )
// {

//   auto raw_hit_num = node_inttrawhit_map_->get_nhits();
//   auto bco_full = node_intteventheader_map_->get_bco_full();

//   if( raw_hit_num == 0 )
//     return Fun4AllReturnCodes::EVENT_OK;
    
//   uint32_t event_counter = node_inttrawhit_map_->get_hit( 0 )->get_event_counter();
//   int hit_num = 0;
//   int hit_num_strange_peak = 0 ;
  
//   for (unsigned int i = 0; i < raw_hit_num; i++)
//   {
//     auto hit = node_inttrawhit_map_->get_hit(i);
//     int felix = hit->get_packetid() - InttQa::kFirst_pid;
//     int felix_ch = hit->get_fee();
//     int chip = hit->get_chip_id();
//     int chan = hit->get_channel_id();
//     auto adc = hit->get_adc();
//     auto bco = hit->get_FPHX_BCO();
    
//     int bco_diff = (bco_full & 0x7f ) - bco;
//     if( bco_diff < 0 )
//       bco_diff += 128;

//     // if (IsCloneHit_[felix][felix_ch][chip-1][chan]) // case of clone hit
//     //   {
//     // 	// loop for clone hit info. 
//     //   }
//     // else // case of non-clone hit
//     //   {
//     // 	// loop for something related to clone hit QA
//     // 	// In this loop, hit without clone hit will be used.
//     // 	hist_fee_chip_chan_woclonehit_[felix]->Fill(felix_ch, chip, chan);
//     // 	IsCloneHit_[felix][felix_ch][chip-1][chan] = true;
	
//     // 	if( adc>0)
//     // 	  {
//     // 	    hit_num++;
	    
//     // 	    if( 39 < bco_diff && bco_diff < 45 )
//     // 	      hit_num_strange_peak++;
//     // 	  }	
//     //   }
//   }
  
//   return Fun4AllReturnCodes::EVENT_OK;
// }

int InttRawHitQA::ResetEvent(PHCompositeNode *topNode)
{

  // Intitialize for Clone hit counter
  // cout << "before memset" << endl;
  //  memset(IsCloneHit_, false, sizeof(IsCloneHit_));

  intt_header_found_ = false;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::EndRun(const int runnumber)
{
  
  this->ProcessHists();
  
  // // Draw hits and save them into a PDF file.
  this->DrawHists();
  
  // write 3D hists into a ROOT file
  for( auto& hist : hist_fee_chip_chan_ )
    {
      tf_output_->WriteTObject( hist, hist->GetName() );
    }

    // for (auto &hist : hist_fee_chip_chan_woclonehit_)
    // {
    //   tf_output_->WriteTObject(hist, hist->GetName());
    // }

  // write 1D hists into a ROOT file
  tf_output_->WriteTObject( hist_nhit_, hist_nhit_->GetName() );
  tf_output_->WriteTObject( hist_nhit_south_, hist_nhit_south_->GetName() );
  tf_output_->WriteTObject( hist_nhit_north_, hist_nhit_north_->GetName() );
  tf_output_->WriteTObject( hist_pid_, hist_pid_->GetName() );
  tf_output_->WriteTObject( hist_adc_, hist_adc_->GetName() );
  tf_output_->WriteTObject( hist_bco_, hist_bco_->GetName() );
  tf_output_->WriteTObject( hist_bco_full_, hist_bco_full_->GetName() );

  for( auto& hist : hist_fee_bco_full_event_counter_ )
    tf_output_->WriteTObject( hist, hist->GetName() );

  // for( auto hist : hist_event_counter_ )
  //   tf_output_->WriteTObject( hist, hist->GetName() );

  // Close the ROOT file

  this->WriteResults2Txt();
  tf_output_->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

void InttRawHitQA::Print(const std::string &what) const
{
  int width = 100;
  cout << string( width, '-' ) << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << "  - Output  (PDF): " << output_pdf_ << endl;
  cout << "  - Output  (txt): " << output_txt_ << endl;
  cout << string( width, '-' ) << endl;
  // this->PrintLine( "Class", "InttRawHitQA" );
//   this->DumpPrintBuffer();
}
