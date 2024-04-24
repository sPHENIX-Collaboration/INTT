#include "InttRawHitQA.h"

using namespace std;

//____________________________________________________________________________..
InttRawHitQA::InttRawHitQA(const std::string &name):
 SubsysReco(name)
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
  string node_name_intteventheader = "INTTEVENTHEADER";
  node_intteventheader_map_ =
    findNode::getClass<InttEventInfo>(topNode, node_name_intteventheader);
  
  if (!node_intteventheader_map_)
    {
      cerr << PHWHERE << node_name_intteventheader << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
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
  gPad->SetGrid( true, true );
  gStyle->SetOptStat( 111111 );

  ////////////////////////////////////////////////////////
  gPad->SetLogy( true );
  hist_nhit_->Draw();
  DrawStats( hist_nhit_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );

  ////////////////////////////////////////////////////////
  gPad->SetLogy( true );
  hist_pid_->Draw( "HIST TEXT0" );
  DrawStats( hist_pid_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );
  
  ////////////////////////////////////////////////////////
  gPad->SetLogy( true );
  hist_adc_->Draw( "" );
  DrawStats( hist_adc_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );

  ////////////////////////////////////////////////////////
  gPad->SetLogy( true );
  hist_bco_->Draw( "" );
  DrawStats( hist_bco_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );

  ////////////////////////////////////////////////////////
  gPad->SetLogy( false );
  hist_bco_full_->Draw( "" );
  DrawStats( hist_bco_full_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );
  
  ////////////////////////////////////////////////////////
  for( int felix=0; felix<kFelix_num_;felix++ )
    {
      c->Clear();
      //      c->SetCanvasSize( 800, 1200 );
      c->Divide( 1, kFee_num_, 0.01, 0 );

      for( int ladder=0; ladder<kFee_num_; ladder++ )
	{

	  auto frame = c->cd( kFee_num_ - ladder );
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
	  
	  DrawStats( hist_hitmap_[ felix ][ ladder ], 0.9, 0, 1.0, 1.0 );
	  // DrawPaletteAxis( hist_hitmap_[ felix ][ ladder ], 0.86, 0.0, 0.88, 1.0, 0.07 );
	  
	}
      
      c->Print( output_pdf_.c_str() );
      
    }
  c->Clear();
  
  c->SetCanvasSize( 800, 800 );
  c->Print( (output_pdf_+"]").c_str() );
}

void InttRawHitQA::ProcessHists()
{

  // pid dist
  for( int i=0; i<kFelix_num_; i++ )
    {
      //cout << hist_fee_chip_chan_[i]->GetEntries() << endl;
      hist_pid_->SetBinContent( i+1, hist_fee_chip_chan_[i]->GetEntries() );
    }


  //  TH2I* hist_hitmap[ kFelix_num_ ][ kFee_num_ ];
  for( int felix=0; felix<kFelix_num_;felix++ )
    {

      for( int ladder=0; ladder<kFee_num_; ladder++ )
	{
	  hist_fee_chip_chan_[ felix ]->GetXaxis()->SetRange( ladder+1, ladder+1 );
	  //hist_fee_chip_chan_[ felix ]->GetXaxis()->SetRange( 2, 5 );
	  // TProfile2D* Project3DProfile(Option_t* option = "xy") const     // *MENU*

	  string name = "yz_" + to_string( ladder );
	  hist_hitmap_[ felix ][ ladder ] = hist_fee_chip_chan_[ felix ]->Project3DProfile( name.c_str() );

	}

    }
}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////

void InttRawHitQA::SetOutputDir( string dir )
{
  output_dir_ = dir;
  string run_num_str = string( 8 - to_string(run_num_).size(), '0' ) + to_string( run_num_ );

  output_root_ = output_dir_ + "root/" + output_basename_ + run_num_str + ".root";
  output_pdf_  = output_dir_ + "plots/" + output_basename_ + run_num_str + ".pdf";
  output_txt_  = output_dir_ + "txt/" + output_basename_ + run_num_str + ".txt";
			      
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

  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );

  /////////////////////////////////////////////
  // Init 3D hists                           //
  /////////////////////////////////////////////
  for( int felix=0; felix<kFelix_num_;felix++ )
    {
      string name = "intt" + to_string( felix );
      string title = name + ";FELIX CH;Chip;Channel";
      hist_fee_chip_chan_[ felix ] = new TH3I( name.c_str(), title.c_str(),
					       kFee_num_, 0, kFee_num_,
					       kChip_num_, 1, kChip_num_+1,
					       kChan_num_, 0, kChan_num_ );
    }

  /////////////////////////////////////////////
  // Init 2D hists                           //
  /////////////////////////////////////////////
  for( int felix=0; felix<kFelix_num_;felix++ )
    {

      for( int ladder=0; ladder<kFee_num_; ladder++ )
	{
	  string name = "intt" + to_string( felix ) + "_" + to_string( ladder );
	  string title = name + ";Chip;Channel";
	  //hist_hitmap_[ felix ][ ladder ] = new TH2I( name.c_str(), title.c_str(),
	  hist_hitmap_[ felix ][ ladder ] = new TProfile2D( name.c_str(), title.c_str(),
							    kChip_num_, 1, kChip_num_,
							    kChan_num_, 0, kChan_num_ );
	}
    }
  
  /////////////////////////////////////////////
  // Init 1D hists                           //
  /////////////////////////////////////////////
  hist_nhit_ = new TH1I( "nhit", "#INTTRAWHIT per event;#hit;Entries", 1e4, 0, 1e4 );

  hist_pid_      = new TH1I ( "pid", "Packet ID distribution;pid;Entries", kFelix_num_, kFirst_pid_, kFirst_pid_ + kFelix_num_ );
  hist_adc_      = new TH1I( "adc", "ADC distribution;BCO;Entries", 8, 0, 8 );
  hist_bco_      = new TH1I( "bco", "BCO distribution;BCO;Entries", 128, 0, 128 );
  hist_bco_full_ = new TH1D( "bco_full", "BCO full distribution;BCO full;Entries",
			     100, 0, TMath::Power( 2, 40 ) );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::process_event(PHCompositeNode *topNode)
{

  // Get nodes to access data
  auto status = this->GetNodes(topNode);

  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  auto raw_hit_num = node_inttrawhit_map_->get_nhits();
  hist_nhit_->Fill( raw_hit_num );
  //cout << "#INTT RAW HIT:" << raw_hit_num << endl;

  auto bco_full = node_intteventheader_map_->get_bco_full();
  hist_bco_full_->Fill( bco_full );
  
  for (unsigned int i = 0; i < raw_hit_num; i++)
  {
    auto hit = node_inttrawhit_map_->get_hit(i);

    int felix_server = hit->get_packetid() - kFirst_pid_;
    int felix_ch = hit->get_fee();
    int chip = hit->get_chip_id();
    int chan = hit->get_channel_id();
    
    // The raw Chip ID can be more than 26
    if( chip > kChip_num_ )
      {
	chip = chip - kChip_num_;
      }

    hist_fee_chip_chan_[ felix_server ]->Fill( felix_ch, chip, chan );
    hist_adc_->Fill( hit->get_adc() );
    hist_bco_->Fill( hit->get_FPHX_BCO() );
    
    if( false )
      {

	cout << setw(5) << i << " "
	     << setw(4) << "felix" << hit->get_packetid() << " "   // Packet ID
	     << setw(2) << hit->get_fee() << " "        // FELIX CH (module)
	     << setw(2) << hit->get_chip_id() << " "    // chip_id
	     << setw(2) << chip << " "    // chip_id
	     << setw(3) << hit->get_channel_id() << " " // chan_id
	     << setw(1) << hit->get_adc() << " "        // adc
	     << setw(3) << hit->get_FPHX_BCO() << " "   // bco
	     << setw(12) << hit->get_bco() << " "       // bco_full
	     << setw(3) << (hit->get_bco() & 0x7F) - hit->get_FPHX_BCO() << " "
	     << endl;
      }
  }


  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttRawHitQA::EndRun(const int runnumber)
{

  this->ProcessHists();
  
  // Draw hits and save them into a PDF file.
  this->DrawHists();
  
  // write 3D hists into a ROOT file
  for( auto& hist : hist_fee_chip_chan_ )
    {
      tf_output_->WriteTObject( hist, hist->GetName() );
    }

  // write 1D hists into a ROOT file
  tf_output_->WriteTObject( hist_nhit_, hist_nhit_->GetName() );
  tf_output_->WriteTObject( hist_pid_, hist_pid_->GetName() );
  tf_output_->WriteTObject( hist_adc_, hist_adc_->GetName() );
  tf_output_->WriteTObject( hist_bco_, hist_bco_->GetName() );
  tf_output_->WriteTObject( hist_bco_full_, hist_bco_full_->GetName() );

  // Close the ROOT file
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
