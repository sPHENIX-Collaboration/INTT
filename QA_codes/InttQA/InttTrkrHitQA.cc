#include "InttTrkrHitQA.h"

using namespace std;

//____________________________________________________________________________..
InttTrkrHitQA::InttTrkrHitQA(const std::string &name):
 SubsysReco(name)
{

  
}

//____________________________________________________________________________..
InttTrkrHitQA::~InttTrkrHitQA()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////
int InttTrkrHitQA::GetNodes(PHCompositeNode *topNode)
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
  // Trkr hit
  // TRKR_HITSET (IO,TrkrHitSetContainerv1)
  
  string node_name_trkr_hitset = "TRKR_HITSET";
  // node_inttrawhit_map_ =
  node_trkrhitset_map_ = 
    findNode::getClass<TrkrHitSetContainer>(topNode, node_name_trkr_hitset);
  
  if (!node_trkrhitset_map_)
    {
      cerr << PHWHERE << node_name_trkr_hitset << " node is missing." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  return 0;
}

void InttTrkrHitQA::GetHits( TrkrHitSetContainer::ConstRange hitsets )
{

  /*!

    @param ConstRange hitsets This is ConstRange = std::pair< ConstIterator, ConstIterator >
  */
  // hitsets.first, hitsets.second: ConstIterator = Map::const_iterator, here Map = std::map < TrkrDefs::hitsetkey, TrkrHitSet* >
  // (*hitsets.first), (*hitsets.second): Map
  // (*hitsets.first).first, .second: TrkrDefs::hitsetkey, TrkrHitSet*
  // (*hitsets.second).first, .second: TrkrDefs::hitsetkey, TrkrHitSet* 
  //cout << (*hitsets.first).first << "\t" << (*hitsets.second).first << endl;
  //  cout << "\t" << TrkrDefs::printBits( (*hitsets.first).first ) << endl;
  // TrkrDefs::printBits( (*hitsets.first).first );
  // TrkrDefs::printBits( (*hitsets.second).first );

  auto current = hitsets.first;
  while( current != hitsets.second )
    {
      auto hitset = (*current).second;

      TrkrDefs::printBits( (*current).first );

      auto layer       = TrkrDefs::getLayer( (*current).first );
      auto detector_id = TrkrDefs::getTrkrId( (*current).first );
      auto phi         = InttDefs::getLadderPhiId( (*current).first ); //TrkrDefs::getPhiElement( (*current).first );
      auto z           = InttDefs::getLadderZId( (*current).first ); // TrkrDefs::getZElement( (*current).first );
      cout << "-----> "
	   << int(detector_id) << "\t" << int(layer) << "\t"
	   << int(phi) << "\t" << int(z) 
	   << endl;
      cout << " + " << (*current).first << "\t"
	   << hitset->size()
	   << endl;

      auto hits = hitset->getHits(); // ConstRange = std::pair< ConstIterator, ConstIterator >
      auto current2 = hits.first; // ConstIterator = Map::const_iterator, here, Map = std::map < TrkrDefs::hitkey, TrkrHit* >, This Map is different from the Map for TrkrHitSetContainer!!!
      while( current2 != hits.second )
	{
	  auto key = (*current2).first;
	  auto hit = (*current2).second;

	  auto col = InttDefs::getCol( key );
	  auto row = InttDefs::getRow( key );
	  cout << "\t\tHit " << key << "\t"
	       << col << "\t"
	       << row << "\t"
	       << hit->getAdc()
	       << endl;

	  /*
	  Offline_s offline_hit;
	  offline_hit.layer = layer;
	  offline_hit.ladder_phi = phi;
	  offline_hit.ladder_z = z;
	  offline_hit.strip_x = col;
	  offline_hit.strip_y = row;
	  */
	  
	  current2++;
	}
      
      current++;
    }

}

void InttTrkrHitQA::DrawHists()
{
  TCanvas* c = new TCanvas( "canvas", "title", 1780, 800); //1600, 1200 );
  c->Print( (output_pdf_+"[").c_str() );
  gPad->SetGrid( true, true );
  gStyle->SetOptStat( 111111 );

  ////////////////////////////////////////////////////////
  // multiplisity
  gPad->SetLogy( true );
  hist_nhit_->Draw();
  InttQa::DrawStats( hist_nhit_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );

  ////////////////////////////////////////////////////////
  // #hit per FELIX
  gPad->SetLogy( true );
  hist_pid_->Draw( "HIST TEXT0" );
  InttQa::DrawStats( hist_pid_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );
  
  ////////////////////////////////////////////////////////
  // ADC
  gPad->SetLogy( true );
  hist_adc_->Draw( "" );
  InttQa::DrawStats( hist_adc_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );

  ////////////////////////////////////////////////////////
  // Hitmap
  TH2INTT* intt_cross_section = new TH2INTT();
  gPad->SetLogy( false );
  intt_cross_section->Draw( "colz0" );
  c->Print( output_pdf_.c_str() );
  
  ////////////////////////////////////////////////////////
  // BCO
  gPad->SetLogy( true );
  hist_bco_->Draw( "" );
  InttQa::DrawStats( hist_bco_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );

  ////////////////////////////////////////////////////////
  // BCO full
  gPad->SetLogy( false );
  hist_bco_full_->Draw( "" );
  InttQa::DrawStats( hist_bco_full_, 0.9, 0.65, 0.99, 0.9 );
  c->Print( output_pdf_.c_str() );
  
  ////////////////////////////////////////////////////////
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
      
      c->Print( output_pdf_.c_str() );
      
    }
  c->Clear();
  
  c->SetCanvasSize( 800, 800 );
  c->Print( (output_pdf_+"]").c_str() );
}

void InttTrkrHitQA::ProcessHists()
{

  // pid dist
  for( int i=0; i<InttQa::kFelix_num; i++ )
    {
      //cout << hist_fee_chip_chan_[i]->GetEntries() << endl;
      hist_pid_->SetBinContent( i+1, hist_fee_chip_chan_[i]->GetEntries() );
    }


  //  TH2I* hist_hitmap[ InttQa::kFelix_num ][ InttQa::kFee_num ];
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {

      for( int ladder=0; ladder<InttQa::kFee_num; ladder++ )
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

void InttTrkrHitQA::SetOutputDir( string dir )
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

int InttTrkrHitQA::Init(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttTrkrHitQA::InitRun(PHCompositeNode *topNode)
{
  // I want to get the run number here... 
  // auto status = this->GetNodes( topNode );
  // if (status == Fun4AllReturnCodes::ABORTEVENT)
  //   return Fun4AllReturnCodes::ABORTEVENT;

  tf_output_ = new TFile( output_root_.c_str(), "RECREATE" );

  /////////////////////////////////////////////
  // Init 3D hists                           //
  /////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      string name = "intt" + to_string( felix );
      string title = name + ";FELIX CH;Chip;Channel";
      hist_fee_chip_chan_[ felix ] = new TH3I( name.c_str(), title.c_str(),
					       InttQa::kFee_num, 0, InttQa::kFee_num,
					       InttQa::kChip_num, 1, InttQa::kChip_num+1,
					       InttQa::kChan_num, 0, InttQa::kChan_num );
    }

  /////////////////////////////////////////////
  // Init 2D hists                           //
  /////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {

      for( int ladder=0; ladder<InttQa::kFee_num; ladder++ )
	{
	  string name = "intt" + to_string( felix ) + "_" + to_string( ladder );
	  string title = name + ";Chip;Channel";
	  //hist_hitmap_[ felix ][ ladder ] = new TH2I( name.c_str(), title.c_str(),
	  hist_hitmap_[ felix ][ ladder ] = new TProfile2D( name.c_str(), title.c_str(),
							    InttQa::kChip_num, 1, InttQa::kChip_num,
							    InttQa::kChan_num, 0, InttQa::kChan_num );
	}
    }
  
  /////////////////////////////////////////////
  // Init 1D hists                           //
  /////////////////////////////////////////////
  hist_nhit_ = new TH1I( "nhit", "#INTTTRKRHIT per event;#hit;Entries", 1e4, 0, 1e4 );

  hist_pid_      = new TH1I ( "pid", "Packet ID distribution;pid;Entries", InttQa::kFelix_num, InttQa::kFirst_pid, InttQa::kFirst_pid + InttQa::kFelix_num );
  hist_adc_      = new TH1I( "adc", "ADC distribution;BCO;Entries", 8, 0, 8 );
  hist_bco_      = new TH1I( "bco", "BCO distribution;BCO;Entries", 128, 0, 128 );
  hist_bco_full_ = new TH1D( "bco_full", "BCO full distribution;BCO full;Entries",
			     100, 0, TMath::Power( 2, 40 ) );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttTrkrHitQA::process_event(PHCompositeNode *topNode)
{

  // Get nodes to access data
  auto status = this->GetNodes(topNode);

  // If a node is not available, skip this event
  if (status == Fun4AllReturnCodes::ABORTEVENT)
    return Fun4AllReturnCodes::ABORTEVENT;

  if( node_trkrhitset_map_->size() == 0 )
    return Fun4AllReturnCodes::EVENT_OK;
    
  cout << "TrkrHitSetContainer size: " << node_trkrhitset_map_->size() << endl;

  //auto hits = 
  GetHits( node_trkrhitset_map_->getHitSets() ); // get TrkrHitSet of INTT only, this is ConstRange = std::pair< ConstIterator, ConstIterator >

  /*
    auto trkr_hit_num = hits.size();
    hist_nhit_->Fill( trkr_hit_num );
  //cout << "#INTT TRKR HIT:" << trkr_hit_num << endl;
  
  auto bco_full = node_intteventheader_map_->get_bco_full();
  hist_bco_full_->Fill( bco_full );
  
  for (unsigned int i = 0; i < hits.size(); i++)
  {
    auto hit = hits[i];

    int felix_server = hit->get_packetid() - kFirst_pid_;
    int felix_ch = hit->get_fee();
    int chip = hit->get_chip_id();
    int chan = hit->get_channel_id();
    
    // The raw Chip ID can be more than 26
    if( chip > InttQa::kChip_num )
      {
	chip = chip - InttQa::kChip_num;
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

  */
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttTrkrHitQA::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttTrkrHitQA::EndRun(const int runnumber)
{
  
  //this->ProcessHists();
  
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

int InttTrkrHitQA::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttTrkrHitQA::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttTrkrHitQA::Print(const std::string &what) const
{
  int width = 100;
  cout << string( width, '-' ) << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << "  - Output  (PDF): " << output_pdf_ << endl;
  cout << "  - Output  (txt): " << output_txt_ << endl;
  cout << string( width, '-' ) << endl;
  // this->PrintLine( "Class", "InttTrkrHitQA" );
//   this->DumpPrintBuffer();
}
