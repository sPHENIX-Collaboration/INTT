#include "InttTrkrHitQA.h"

using namespace std;

//____________________________________________________________________________..
InttTrkrHitQA::InttTrkrHitQA(const std::string &name, bool is_official ):
  SubsysReco(name), is_official_(is_official)
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
  // INTT raw hit
  string node_name_inttrawhit = "INTTRAWHIT";
  node_inttrawhit_map_ =
    findNode::getClass<InttRawHitContainer>(topNode, node_name_inttrawhit);
  
  if (!node_inttrawhit_map_)
    {
      cerr << PHWHERE << node_name_inttrawhit << " node is missing." << endl;
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

vector < std::pair < uint16_t, int > > InttTrkrHitQA::GetBcoEventCounter()
{

  vector < pair < InttNameSpace::Online_s,  unsigned int > > online_hits = this->GetHits(  node_trkrhitset_map_->getHitSets() );
  vector < InttRawHit* > raw_hits = this->GetRawHitsWithoutClone();

  vector < std::pair < uint16_t, int > > rtn;

  
  // if( raw_hits.size() > 20 )
  //   return rtn;
  InttDacMap* dac_map = new InttDacMap();
  
  int counter = 0;
  for( int i=0; i<online_hits.size(); i++ )
    {
      ////////////////////////////////////////////
      // struct Online_s                        //
      //   int lyr, int ldr,  int arm           //
      //   int chp, int chn                     //
      ////////////////////////////////////////////
      auto online_hit = online_hits[i].first;
      auto dac = online_hits[i].second;

      ////////////////////////////////////////////
      // struct RawData_s:                      //
      //   int felix_server, int felix_channel  //
      //   int chip,         int channel,       //
      ////////////////////////////////////////////
      // InttNameSpace::RawData_s
      auto raw_data  = InttNameSpace::ToRawData( online_hit );
      
      ////////////////////////////////////////////
      // struct Offline_s                       //
      //   int layer,     int ladder_phi,       //
      //   int ladder_z,  int strip_x,          //
      //   int strip_y,                         //
      ////////////////////////////////////////////
      // InttNameSpace::Offline_s
      auto offline_hit = InttNameSpace::ToOffline( online_hit );

      // consistency confirmed, no need anymore
      // if( online_hit.chp != raw_data.chip
      // 	  || online_hit.chn != raw_data.channel )
      // 	{
      
      // 	  cout << setw(4) << online_hit.chp
      // 	       << setw(5) << online_hit.chn
      // 	       << endl;
	  
      // 	  cout << setw(4) << raw_data.chip
      // 	       << setw(5) << raw_data.channel
      // 	       << " | "
      // 	       << setw(3) << raw_data.felix_server
      // 	       << setw(5) << raw_data.felix_channel
      // 	       << endl;
	  
      // 	  cout << "---------------------------------------------------" << endl;
      // 	}

      // cout << "TrkrHit: "
      // 	   << setw(4) << i
      // 	   << ": " 
      // 	   << setw(3) << raw_data.felix_server
      // 	   << setw(5) << raw_data.felix_channel
      // 	   << setw(4) << raw_data.chip
      // 	   << setw(6) << raw_data.channel
      // 	   << setw(4) << dac
      // 	   << " vs ";
	//	   << endl;
      vector < int > used_index( raw_hits.size() );
      
      for( int j=0; j<raw_hits.size(); j++ )
	{
	  
	  auto raw_hit = raw_hits[j];

	  int chan_raw		= raw_hit->get_channel_id();
	  // first selections
	  if( chan_raw != raw_data.channel )
	    continue;
	  
	  // uint16_t InttRawHit::get_chip_id
	  int chip_raw		= raw_hit->get_chip_id();    
	  if(chip_raw  > InttQa::kChip_num)
	    chip_raw = chip_raw - InttQa::kChip_num;

	  chip_raw--; // to change from 1-base to 0-base
	  if( chip_raw != raw_data.chip )
	    continue;

	  int felix_raw		= raw_hit->get_packetid() - InttQa::kFirst_pid;
	  int felix_ch_raw	= raw_hit->get_fee();
	  bool is_same = (felix_raw == raw_data.felix_server
			  || felix_ch_raw == raw_data.felix_channel );
			  // || adc_raw == adc );

	  if( is_same == false )
	    continue;

	  auto adc_raw		= raw_hit->get_adc();
	  /*
	    virtual unsigned short GetDAC(const uint& felix_server,
                                const uint& felix_channel,
                                const uint& chip,
                                const uint& channel,
                                const uint& adc);
	  */
	  auto dac_raw = dac_map->GetDAC( felix_raw, felix_ch_raw, chip_raw, chan_raw, adc_raw );
	  if( dac_raw != dac )
	    continue;
	  
	  auto bco_raw		= raw_hit->get_FPHX_BCO();
	  int event_counter_raw	= raw_hit->get_event_counter(); // uint32_t IttRawHit::get_event_counter()
	  pair < uint16_t, int > bco_event_counter( bco_raw, event_counter_raw );
	  rtn.push_back( bco_event_counter );
	  counter++;
	  //if( felix_raw == raw_data.felix_server )
	  //	  if( felix_ch_raw == raw_data.felix_channel && felix_ch_raw != 0 )
	    // cout << "RawHit: "
	    // 	 << setw(4) << j
	    // 	 << ": " 
	    // 	 << setw(3) << felix_raw
	    // 	 << setw(5) << felix_ch_raw
	    // 	 << setw(4) << chip_raw
	    // 	 << setw(6) << chan_raw
	    // 	 << setw(4) << adc_raw
	    // 	 << endl;
	  		
	  // cout << "( "
	  //      << setw(3) << bco_raw << ", " << setw(1) << adc_raw << "-> " << dac
	  //      << "), ";
	  //used_index.push_back( j );
	  //raw_hits.erase( raw_hits.begin() + j );
	  //break;
	}
      //      cout << endl;
      
    }


  // cout << counter << " / " << online_hits.size() << " found their corresponding raw hit while there are "
  //      << raw_hits.size() << " raw hits in this event." 
  //      << endl;
  return rtn;
}


bool InttTrkrHitQA::IsSame( InttRawHit* hit1, InttRawHit* hit2 )
{

  if( hit1->get_word() != hit2->get_word() ) // only this might be enough
    return false;
  else if( hit1->get_bco() != hit2->get_bco() )
    return false;
  else if( hit1->get_FPHX_BCO() != hit2->get_FPHX_BCO() )
    return false;
  else if( hit1->get_channel_id() != hit2->get_channel_id() )
    return false;
  else if( hit1->get_chip_id() != hit2->get_chip_id() )
    return false;
  else if( hit1->get_fee() != hit2->get_fee() )
    return false;
  else if( hit1->get_packetid() != hit2->get_packetid() )
    return false;
  else if( hit1->get_adc() != hit2->get_adc() )
    return false;
  else if( hit1->get_full_FPHX() != hit2->get_full_FPHX() )
    return false;
  // else if( hit1->get_full_ROC() != hit2->get_full_ROC() ) // maybe no value is assigned?
  //   return false;
  // else if( hit1->get_amplitude()!= hit2->get_amplitude()) // maybe no value is assigned?
  //   return false;

  // hits pass all comparison are the same
  return true;
};

vector < InttRawHit* > InttTrkrHitQA::GetRawHits()
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

vector < InttRawHit* > InttTrkrHitQA::GetRawHitsWithoutClone()
{

  auto hits = this->GetRawHits();
  vector < InttRawHit* > rtn;

  for( int i=0; i<hits.size(); i++ )
    {

      bool is_same_found = false;
      for( int j=i+1; j<hits.size(); j++ )
        {

          is_same_found = is_same_found || this->IsSame( hits[i], hits[j] );
          if( is_same_found == true )
            {
              break;
            }

        } // end of for( hit_j )

      // if this hit is still unique (same hit is not found), keep it
      if( is_same_found == false )
        rtn.push_back( hits[i] );

    } // end of for( hit_i )

  return rtn;
}

vector < pair < InttNameSpace::Online_s,  unsigned int > >
InttTrkrHitQA::GetHits( TrkrHitSetContainer::ConstRange hitsets )
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

  vector < pair < InttNameSpace::Online_s, unsigned int > > hits_information;
  auto current = hitsets.first;
  while( current != hitsets.second )
    {
      auto hitset = (*current).second;

      //TrkrDefs::printBits( (*current).first );

      auto layer       = TrkrDefs::getLayer( (*current).first );
      // layer: 0, 1, 2   : MVTX
      //        3, 4, 5, 6: INTT
      //        6<        : TPC

      // Take hits from only INTT
      if( !(3<= layer && layer <= 6) )
	continue;
      
      auto detector_id = TrkrDefs::getTrkrId( (*current).first );
      auto phi         = InttDefs::getLadderPhiId( (*current).first ); //TrkrDefs::getPhiElement( (*current).first );
      auto z           = InttDefs::getLadderZId( (*current).first ); // TrkrDefs::getZElement( (*current).first );
      // cout << "-----> "
      // 	   << int(detector_id) << "\t" << int(layer) << "\t"
      // 	   << int(phi) << "\t" << int(z) ;
      // 	//<< endl;
      // cout << " + " << (*current).first << "\t"
      // 	   << hitset->size()
      // 	   << endl;

      auto hits = hitset->getHits(); // ConstRange = std::pair< ConstIterator, ConstIterator >
      auto current2 = hits.first; // ConstIterator = Map::const_iterator, here, Map = std::map < TrkrDefs::hitkey, TrkrHit* >, This Map is different from the Map for TrkrHitSetContainer!!!
      while( current2 != hits.second )
	{
	  auto key = (*current2).first;
	  auto hit = (*current2).second;

	  auto col = InttDefs::getCol( key );
	  auto row = InttDefs::getRow( key );

	  InttNameSpace::Offline_s offline_hit;
	  offline_hit.layer = layer;
	  offline_hit.ladder_phi = phi;
	  offline_hit.ladder_z = z;
	  offline_hit.strip_x = row;
	  offline_hit.strip_y = col;

	  InttNameSpace::Online_s online_hit = InttNameSpace::ToOnline( offline_hit );
	  
	  auto hit_info = pair < InttNameSpace::Online_s, unsigned int >( online_hit, hit->getAdc() );
	  hits_information.push_back( hit_info );
	  // cout << "\t\tHit " << key << "\t"
	  //      << col << "\t"
	  //      << row << "\t"
	  //      << hit->getAdc() << "\t|  "
	  //      << online_hit.ldr << "\t"
	  //      << online_hit.arm
	  //      << endl;
	  
	  current2++;
	}
      
      current++;
    }

  return hits_information;
}

void InttTrkrHitQA::DrawHists()
{
  TCanvas* c = new TCanvas( "canvas", "title", 1780, 800); //1600, 1200 );
  c->Print( (output_pdf_+"[").c_str() );
  gPad->SetGrid( true, true );
  gStyle->SetOptStat( 111111 );

  ////////////////////////////////////////////////////////
  // Event counter vs FELIX
  /*
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
  */
  
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
  // multiplisity
  gPad->SetLogy( true );
  InttQa::OptimizeRange( hist_nhit_ );
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
  //  TH2INTT* intt_cross_section = new TH2INTT();
  gPad->SetLogy( false );
  intt_cross_section_->Draw( "colz0" );
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
  
  //////////////////////////////////////////////////////////////////
  // Normalization using the last event counter                   //
  //////////////////////////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num; felix++ )
    {
      hist_fee_chip_chan_[ felix ]			->Scale( 1.0 / event_counter_by_myself_ );
      //hist_fee_bco_full_event_counter_[ felix ]		->Scale( 1.0 / event_counter_by_myself_ );
      //hist_fee_bco_full_event_counter_diff_[ felix ]	->Scale( 1.0 / event_counter_by_myself_ );
    }
  
  hist_nhit_		->Scale( 1.0 / event_counter_by_myself_ ); 
  hist_adc_		->Scale( 1.0 / event_counter_by_myself_ );
  hist_bco_		->Scale( 1.0 / event_counter_by_myself_ ); 
  hist_bco_full_	->Scale( 1.0 / event_counter_by_myself_ );

  for( int barrel=0; barrel<2; barrel++ )
    {
      for( int layer=0; layer<2; layer++ )
	{

	  int ladder_num = 12;
	  if( barrel == 1 )
	    ladder_num = 16;
	  
	  for( int ladder=0; ladder<ladder_num; ladder++ )
	    {

	      for( int side=0; side<2; side++ )
		{
		  auto val = intt_cross_section_->GetLadderIContent( barrel, layer, ladder, side );
		  intt_cross_section_->SetLadderIContent( barrel, layer, ladder, side, val / event_counter_by_myself_ );
		}
	    }
	}
    }

  /*
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
  */
  
}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////

void InttTrkrHitQA::SetOutputDir( string dir )
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

  // If an user sets the run number to recoConsts, it works. Can I assume it?
  recoConsts *rc = recoConsts::instance();
  run_num_ = rc->get_IntFlag( "RUNNUMBER" );

  this->SetOutputDir();
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

  intt_cross_section_ = new TH2INTT();

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

  event_counter_by_myself_++;

  //cout << "#Raw hit: " << node_inttrawhit_map_->get_nhits() << endl;
  // cout << "TrkrHitSetContainer size: " << node_trkrhitset_map_->size() << endl;

  //auto hits = 
  auto hits = GetHits( node_trkrhitset_map_->getHitSets() ); // get TrkrHitSet of INTT only, this is ConstRange = std::pair< ConstIterator, ConstIterator >

  auto bco_event_counter_pair = this->GetBcoEventCounter();
  
  hist_nhit_->Fill( hits.size() );

  auto bco_full = node_intteventheader_map_->get_bco_full();
  hist_bco_full_->Fill( bco_full );
  
  for( int i=0; i<hits.size(); i++ )
    {
      ////////////////////////////////////////////
      // struct Online_s                        //
      //   int lyr, int ldr,  int arm           //
      //   int chp, int chn                     //
      ////////////////////////////////////////////
      auto online_hit = hits[i].first;
      auto adc = hits[i].second; // int the units of DAC (15, 30, ....) but not ADC (0, 1, 2, ...)
      
      ////////////////////////////////////////////
      // struct RawData_s:                      //
      //   int felix_server, int felix_channel  //
      //   int chip,         int channel,       //
      //----------------------------------------//
      //  chip == Online_s::chp                 //
      //  channel == online_s::chn  (0-255)     //
      ////////////////////////////////////////////
      // InttNameSpace::RawData_s
      auto raw_data  = InttNameSpace::ToRawData( online_hit );
      
      ////////////////////////////////////////////
      // struct Offline_s                       //
      //   int layer,     int ladder_phi,       //
      //   int ladder_z (0-3),                  //
      //   int strip_x :                        //
      //   int strip_y : chip column            //
      ////////////////////////////////////////////
      // InttNameSpace::Offline_s
      auto offline_hit = InttNameSpace::ToOffline( online_hit );

      //////////////////////////////////////////////////////////
      // 1D histograms                                        //
      //////////////////////////////////////////////////////////
      hist_pid_->Fill( raw_data.felix_server );
      hist_fee_chip_chan_[ raw_data.felix_server ]
	->Fill( raw_data.felix_channel, raw_data.chip, raw_data.channel );
      hist_adc_->Fill( adc );
      //hist_bco_->Fill( hit->get_FPHX_BCO() );

      //////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////
      //void TH2INTT::SetLadderIContent(int barrel_id, int layer_id, int ladder_id, int side, double content)
      int barrel = ( offline_hit.layer <= 4 ? 0 : 1 ); // 0: innter barrel, 1: outer, maybe
      int layer = ( offline_hit.layer % 2 == 1 ? 0 : 1 ); // 0: inner layer, 1: outer layer, maybe
      int side =  ( offline_hit.ladder_z <= 1 ? 0 : 1 ); // 0: south, 1; north
      // cout << setw(3) << offline_hit.layer
      // 	   << setw(3) << barrel
      // 	   << setw(3) << layer
      // 	   << setw(4) << offline_hit.ladder_phi
      // 	   << setw(3) << side
      // 	   << setw(3) << offline_hit.ladder_z
      // 	   << endl;
      
      auto val = intt_cross_section_->GetLadderIContent( barrel,
							 layer,
							 offline_hit.ladder_phi,
							 side
							 );
      intt_cross_section_->SetLadderIContent( barrel,
					      layer,
					      offline_hit.ladder_phi,
					      side,
					      (++val)
					      );
    }

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
  cout << "InttTrkrHitQA" << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << "  - Output  (PDF): " << output_pdf_ << endl;
  cout << "  - Output  (txt): " << output_txt_ << endl;
  cout << string( width, '-' ) << endl;
  // this->PrintLine( "Class", "InttTrkrHitQA" );
//   this->DumpPrintBuffer();
}
