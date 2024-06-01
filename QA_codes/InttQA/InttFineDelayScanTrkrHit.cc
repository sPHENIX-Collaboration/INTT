#include "InttFineDelayScanTrkrHit.h"

using namespace std;

TrkrHitInfo::TrkrHitInfo( InttNameSpace::Online_s hit, uint16_t bco_fphx, int event_counter, int dac )
  : hit_( hit ), bco_fphx_( bco_fphx ), event_counter_( event_counter ) , dac_( dac )
{};

void TrkrHitInfo::Print()
{

  cout << "TrkrHitInfo: " 
       << setw(2) << GetFelixServer	()
       << setw(3) << GetFelixChannel	()
       << setw(3) << GetChip		()
       << setw(4) << GetChannel	()
       << setw(4) << GetFPHXBco()
       << setw(4) << GetDac()
       << setw(10) << GetEventCounter()
       << endl;

}


  
//____________________________________________________________________________..
InttFineDelayScanTrkrHit::InttFineDelayScanTrkrHit(const string &name, bool is_official ):
  SubsysReco(name),
  is_official_( is_official )
{

}

//____________________________________________________________________________..
InttFineDelayScanTrkrHit::~InttFineDelayScanTrkrHit()
{

}

/////////////////////////////////////////////////////////////////////////
// private
/////////////////////////////////////////////////////////////////////////
int InttFineDelayScanTrkrHit::GetNodes(PHCompositeNode *topNode)
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
vector < InttRawHit* > InttFineDelayScanTrkrHit::GetRawHits()
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

vector < InttRawHit* > InttFineDelayScanTrkrHit::GetRawHitsWithoutClone()
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
InttFineDelayScanTrkrHit::GetTrkrHits( TrkrHitSetContainer::ConstRange hitsets )
{
  /*!
    @param ConstRange hitsets This is ConstRange = pair< ConstIterator, ConstIterator >
  */
  // hitsets.first, hitsets.second: ConstIterator = Map::const_iterator, here Map = map < TrkrDefs::hitsetkey, TrkrHitSet* >
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

vector < TrkrHitInfo* > InttFineDelayScanTrkrHit::GetHitInfo()
{

  vector < InttRawHit* > raw_hits = this->GetRawHitsWithoutClone();
  vector < pair < InttNameSpace::Online_s,  unsigned int > > online_hits = this->GetTrkrHits(  node_trkrhitset_map_->getHitSets() );

  InttDacMap* dac_map = new InttDacMap();
  
  int counter = 0;
  vector < TrkrHitInfo* > info; // to be returned
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
	  //	  rtn.push_back( bco_event_counter );
	  //counter++;

	  TrkrHitInfo* hit_info = new TrkrHitInfo( online_hit, bco_raw, event_counter_raw, dac );
	  //hit_info->Print();
	  info.push_back( hit_info );
	  		
	  // cout << "( "
	  //      << setw(3) << bco_raw << ", " << setw(1) << adc_raw << "-> " << dac
	  //      << "), ";
	}
      
    }

  // cout << counter << " / " << online_hits.size() << " found their corresponding raw hit while there are "
  //      << raw_hits.size() << " raw hits in this event." 
  //      << endl;
  return info;
}

void InttFineDelayScanTrkrHit::DrawRatioToPad( TCanvas* c,
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
      
      string option = "HIST";
      if( i > 0 )
	option = "HIST sames";

      hists_ratio[i]->Draw( option.c_str());
      //hist_sidelobe_ratio_odd_->Draw( "sames" );
      double xmin = xmin_area + width_stat * (i/row_num);
      double xmax = xmin + width_stat;
      double ymax = ymax_area - height_stat * (i%row_num);
      double ymin = ymax - height_stat;
      InttQa::DrawStats( hists_ratio[i], xmin, ymin, xmax, ymax );
    }
}

void InttFineDelayScanTrkrHit::DrawBcoDiffRatioPair( TCanvas* c,
					      string title_bco_full, 
					      vector < TH3D* > hists_bco_diff,
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

      string option = "HIST";
      if( i > 0 )
	option = "HIST sames";

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

void InttFineDelayScanTrkrHit::DrawRatioPair( TCanvas* c,
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

  // cout << title_ratio_upper << "\t" << hists_ratio_upper.size() << endl;
  // cout << title_ratio_lower << "\t" << hists_ratio_lower.size() << endl;
  
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


void InttFineDelayScanTrkrHit::DrawHists()
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
			      vector < TH3D* >{ begin(hist_fee_chip_bco_diff_), end(hist_fee_chip_bco_diff_) },
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
			      vector < TH3D* >{ begin(hist_fee_chip_bco_diff_), end(hist_fee_chip_bco_diff_) },
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
  // BCO , ratio (even)
  /////////////////////////////////////////
  this->DrawBcoDiffRatioPair( c,
			      "BCO_{FPHX}",
			      vector < TH3D* >{ begin(hist_fee_chip_bco_), end(hist_fee_chip_bco_) },
			      "Ratio of even bins of all FELIXes",
			      vector < TH1D* >{ hist_sidelobe_ratio_bco_even_ }
			      );
  
  /////////////////////////////////////////
  // BCO diff, ratio (even), each FELIX  //
  /////////////////////////////////////////
  vector < TH1D* > hists_ratio_bco_even_;
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      auto hist = hist_fee_chip_sidelobe_ratio_bco_even_[ felix ]->ProjectionZ(); // ratio of all hits in a FELIX
      hists_ratio_bco_even_.push_back( hist );

    }
  this->DrawBcoDiffRatioPair( c,
			      "BCO_{FPHX}",
			      vector < TH3D* >{ begin(hist_fee_chip_bco_), end(hist_fee_chip_bco_) },
			      "Ratio of even bins of each FELIX",
			      hists_ratio_bco_even_,
			      true
			      );
  
  
  ////////////////////////////////////////////////
  // BCO, ratio (even), ROCs in each FELIX //
  ////////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num; felix++ )
    {

      string pad_title_base = "intt" + to_string( felix );
      string roc_upper = "RC-" + to_string( 2 * (felix%4) ) + (felix<4 ? "S" : "N");
      string roc_lower = "RC-" + to_string( 2 * (felix%4) +1 ) + (felix<4 ? "S" : "N");
            
      vector < TH1D* > hists;
      TH1D* hist_roc_upper = hist_fee_chip_sidelobe_ratio_bco_even_[ felix ]
	->ProjectionZ( (pad_title_base + "_" + roc_upper).c_str(), 1, 7 );
      //hist_roc_upper->SetName( ( pad_title_base + "_" + roc_upper ).c_str() );
      hists.push_back( hist_roc_upper );
      
      TH1D* hist_roc_lower = hist_fee_chip_sidelobe_ratio_bco_even_[ felix ]
	->ProjectionZ( ( pad_title_base + "_" + roc_lower ).c_str(), 7, 13 );
      //      hist_roc_lower->SetName( ( pad_title_base + "_" + roc_lower ).c_str() );
      
      for( int felix_ch=0; felix_ch<InttQa::kFee_num; felix_ch++ )
	{
	  string name = pad_title_base + "_FELIX_ch" + to_string( felix_ch );

	  auto hist = hist_fee_chip_sidelobe_ratio_bco_even_[ felix ]
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
			   vector < TH1D* >{ begin( hists ), begin( hists ) + 7 + 1 },
			   (pad_title_base + " " + roc_lower ).c_str(),
			   vector < TH1D* >{ begin( hists ) + 7 + 1 , end(hists)},
			   false
			   );
    }

  /////////////////////////////////////////
  // BCO diff raw, ratio (even)
  /////////////////////////////////////////
  this->DrawBcoDiffRatioPair( c,
			      "Raw BCO difference",
			      vector < TH3D* >{ begin(hist_fee_chip_bco_diff_raw_), end(hist_fee_chip_bco_diff_raw_) },
			      "Ratio of even bins of all FELIXes using raw BCO difference",
			      vector < TH1D* >{ hist_sidelobe_ratio_raw_even_ }
			      );
  
  
  ////////////////////////////////////////////////////////
  // correlation for test
  // gPad->SetLogy( false );
  // hist_correlation_->Draw( "colz" );
  
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


bool InttFineDelayScanTrkrHit::IsSame( InttRawHit* hit1, InttRawHit* hit2 )
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

void InttFineDelayScanTrkrHit::ProcessHists()
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
	      
	      // histgram integrated over all FELIX
	      int bin = hist_sidelobe_ratio_even_
		->Fill( sidelobe_ratio_even );

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
	      
	      // histgram integrated over all FELIX
	      hist_sidelobe_ratio_raw_even_
		->Fill( sidelobe_ratio_raw_even );
	      
	    } // end of for( chip )
	} // end of for( fee, i.e. felix_ch )
    } // end of for( felix )

  /////////////////////////////////////////////////////
  // 3D, Ladder vs BCO vs Event counter (felix) //
  /////////////////////////////////////////////////////
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {

      for( int felix_ch=0; felix_ch<InttQa::kFee_num; felix_ch++ )
	{

	  for( int chip=0; chip<InttQa::kChip_num; chip++ )
	    {

	      string name = "intt" + to_string( felix )
		+ "_FELIX_ch" + to_string( felix_ch )
		+ "_chip" + to_string( chip )
		+ "_bco_even";
	      auto hist = hist_fee_chip_bco_[ felix ]->ProjectionZ( name.c_str(),
								    felix_ch+1, felix_ch+1,
								    chip+1, chip+1 );
	      int counter[ 2 ] = {0};
	      for( int bin=1; bin<hist->GetNbinsX()+1; bin++ )
		{

		  // bin 1, 3, ... = BCO 0, 2, 4, ... -> odd
		  // bin 2, 4, ... = BCO 1, 3, 5, ... -> even
		  auto content = hist->GetBinContent( bin );
		  counter[ (bin-1)%2 ] += content;
		}
	      
	      double sidelobe_ratio_even = counter[1] / hist->Integral();
	      hist_fee_chip_sidelobe_ratio_bco_even_[ felix ] 
		->Fill( felix_ch, chip, sidelobe_ratio_even );
	      
	      // histgram integrated over all FELIX
	      int bin = hist_sidelobe_ratio_bco_even_
		->Fill( sidelobe_ratio_even );

	    } // end of for( chip )
	} // end of for( fee, i.e. felix_ch )
    } // end of for( felix )

  // Normlization of hists using #events
  //  hist_fee_chip_bco_diff_all_->Scale( 1.0 / last_event_counter_ );  
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      //hist_fee_chip_chan_[ felix ]->Scale( 1.0 / last_event_counter_ );
    
      hist_fee_chip_bco_diff_[ felix ]->Scale( 1.0 / last_event_counter_ );
  
      hist_fee_chip_bco_diff_raw_[ felix ]->Scale( 1.0 / last_event_counter_ );
    }
}

/////////////////////////////////////////////////////////////////////////
// public
/////////////////////////////////////////////////////////////////////////

void InttFineDelayScanTrkrHit::SetOutputDir( string dir )
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

int InttFineDelayScanTrkrHit::Init(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScanTrkrHit::InitRun(PHCompositeNode *topNode)
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
  // 1D hists for BCO
  string name_sidelobe_ratio_bco_even_ = name_all + "_bco_even_counter";
  string title_sidelobe_ratio_bco_even_ = name_all + ";BCO_{FPHX};Entries/event";
  hist_sidelobe_ratio_bco_even_
    = new TH1D ( name_sidelobe_ratio_bco_even_.c_str(),
		 title_sidelobe_ratio_bco_even_.c_str(),
		 ratio_bin_num, ratio_min, ratio_max );
  
  //////////////////////////////////////////////////////
  // 1D hists for BCO diff
  string name_sidelobe_ratio_even_ = name_all + "_bco_full_even_counter";
  string title_sidelobe_ratio_even_ = name_all + ";BCOdiff_{even}/(BCOdiff_{odd} + BCOdiff_{even});Entries/event";
  hist_sidelobe_ratio_even_
    = new TH1D ( name_sidelobe_ratio_even_.c_str(),
		 title_sidelobe_ratio_even_.c_str(),
		 ratio_bin_num, ratio_min, ratio_max );
  
  //////////////////////////////////////////////////////
  // 1D hists for BCO diff raw
  string name_sidelobe_ratio_raw_even_ = name_all + "_bco_full_raw_even_counter";
  string title_sidelobe_ratio_raw_even_ = name_all + ";BCOdiff_{even}/(BCOdiff_{odd} + BCOdiff_{even});Entries/event";
  hist_sidelobe_ratio_raw_even_
    = new TH1D ( name_sidelobe_ratio_raw_even_.c_str(),
		 title_sidelobe_ratio_raw_even_.c_str(),
		 ratio_bin_num, ratio_min, ratio_max );
  
  for( int felix=0; felix<InttQa::kFelix_num;felix++ )
    {
      string name = "intt" + to_string( felix );

      //////////////////////////////////////////////////////
      // hists for BCO
      string name_fee_chip_bco = name + "_fee_chip_bco";
      string title_fee_chip_bco = name + ";FELIX_CH;Chip;BCO_{FPHX}";
      hist_fee_chip_bco_[ felix ]
	 = new TH3D( name_fee_chip_bco.c_str(), title_fee_chip_bco.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     InttQa::kBco_max, 0, InttQa::kBco_max );

      //////////////////////////////////////////////////////
      // hists for BCO diff

      // BCO diff
      string name_fee_chip_bco_diff = name + "_fee_chip_bco_diff";
      string title_fee_chip_bco_diff = name + ";FELIX_CH;Chip; BCOfull%0x7f - BCO";
      hist_fee_chip_bco_diff_[ felix ]
	 = new TH3D( name_fee_chip_bco_diff.c_str(), title_fee_chip_bco_diff.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     InttQa::kBco_max, 0, InttQa::kBco_max );

      // BCO diff ratio (even)
      string name_fee_chip_sidelobe_ratio_even = name + "_bco_full_even_counter";
      string title_fee_chip_sidelobe_ratio_even = name + ";FELIX_CH;Chip;BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{even})";
      hist_fee_chip_sidelobe_ratio_even_[ felix ]
	= new TH3D ( name_fee_chip_sidelobe_ratio_even.c_str(),
		     title_fee_chip_sidelobe_ratio_even.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     ratio_bin_num, ratio_min, ratio_max );
		     
      //////////////////////////////////////////////////////
      // hists for BCO diff raw
      // BCO diff raw
      string name_fee_chip_bco_diff_raw = name + "_fee_chip_bco_diff_raw";
      string title_fee_chip_bco_diff_raw = name + ";FELIX_CH;Chip; BCOfull%0x7f - BCO (raw)";
      hist_fee_chip_bco_diff_raw_[ felix ]
	 = new TH3D( name_fee_chip_bco_diff_raw.c_str(), title_fee_chip_bco_diff_raw.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     2 * InttQa::kBco_max, -InttQa::kBco_max, InttQa::kBco_max );

      // BCO diff raw ratio (even)
      string name_fee_chip_sidelobe_ratio_raw_even = name + "_bco_full_raw_even_counter";
      string title_fee_chip_sidelobe_ratio_raw_even = name + ";FELIX_CH;Chip;BCOdiff_{odd}/(BCOdiff_{odd} + BCOdiff_{even})";
      hist_fee_chip_sidelobe_ratio_raw_even_[ felix ]
	= new TH3D ( name_fee_chip_sidelobe_ratio_raw_even.c_str(),
		     title_fee_chip_sidelobe_ratio_raw_even.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     ratio_bin_num, ratio_min, ratio_max );
		     
      //////////////////////////////////////////////////////
      // BCO ratio (even)
      string name_fee_chip_sidelobe_ratio_bco_even = name + "_bco_full_bco_even_counter";
      string title_fee_chip_sidelobe_ratio_bco_even = name + ";FELIX_CH;Chip;BCO_{FPHX})";
      hist_fee_chip_sidelobe_ratio_bco_even_[ felix ]
	= new TH3D ( name_fee_chip_sidelobe_ratio_bco_even.c_str(),
		     title_fee_chip_sidelobe_ratio_bco_even.c_str(),
		     InttQa::kFee_num, 0, InttQa::kFee_num,
		     InttQa::kChip_num, 1, InttQa::kChip_num+1,
		     ratio_bin_num, ratio_min, ratio_max );
		     
    }

  hist_bco_      = new TH1I( "bco", "BCO distribution;BCO;Entries/event", 128, 0, 128 );
  InttQa::HistConfig( hist_bco_ );
  
  hist_bco_full_ = new TH1D( "bco_full", "BCO full distribution;BCO full;Entries/event",
			     100, 0, TMath::Power( 2, 40 ) );
  InttQa::HistConfig( hist_bco_full_ );

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScanTrkrHit::process_event(PHCompositeNode *topNode)
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

  //  auto hits = this->GetHits();
  auto hits_info = this->GetHitInfo();
  
  // loop over all raw hits
  bool found = false;
  for (unsigned int i = 0; i < hits_info.size(); i++)
  {
    
    //auto hit = hits[i];
    auto hit_info = hits_info[i];

    int felix = hit_info->GetFelixServer();
    int felix_ch = hit_info->GetFelixChannel();
    int chip =  hit_info->GetChip();
    int chan =  hit_info->GetChannel();
    auto bco =  hit_info->GetFPHXBco();
    int event_counter = hit_info->GetEventCounter();
    int dac = hit_info->GetDac();
    
    int bco_diff = (bco_full & 0x7f ) - bco;
    
    if( bco_diff < 0 )
      bco_diff += 128;
    
    // The raw Chip ID can be more than 26
    if (chip > InttQa::kChip_num)
    {
      chip = chip - InttQa::kChip_num;
    }

    hist_fee_chip_bco_diff_[ felix ]->Fill( felix_ch, chip, bco_diff );

    // BCO info
    hist_fee_chip_bco_[ felix ]->Fill( felix_ch, chip, bco );
    
    // fill BCO diff before adding 128. It's called BCO diff raw
    hist_fee_chip_bco_diff_raw_[ felix ]->Fill( felix_ch, chip, bco_diff );
    //    hist_bco_->Fill(hit->get_FPHX_BCO());

   if( found )
    {
      found = true;
      cout << setw(6) << i << " "
	//<< setw(13) << event_counter_ref
           << setw(10) << event_counter << " "
           << setw(4) << "felix " << felix << " " // Packet ID
           << setw(3) << felix_ch << " "                  // FELIX CH (module)
           << setw(4) << chip << " "                            // chip_id
           << setw(4) << chan << " "           // chan_id
           << setw(3) << dac << " "                  // adc
           << setw(3) << bco << " "             // bco
           << setw(12) << bco_full<< " "                 // bco_full
           << setw(3) << bco_diff  << " "
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

  last_event_counter_ = hits_info[0]->GetEventCounter();
  //this->process_event_clone_hit( topNode );
  
  return Fun4AllReturnCodes::EVENT_OK;
}


int InttFineDelayScanTrkrHit::ResetEvent(PHCompositeNode *topNode)
{

  intt_header_found_ = false;
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScanTrkrHit::EndRun(const int runnumber)
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
      
      tf_output_->WriteTObject( hist_fee_chip_bco_[ felix ],
				hist_fee_chip_bco_[ felix ]->GetName() );

      tf_output_->WriteTObject( hist_fee_chip_bco_diff_[ felix ],
				hist_fee_chip_bco_diff_[ felix ]->GetName() );

      tf_output_->WriteTObject( hist_fee_chip_sidelobe_ratio_even_[ felix ],
				hist_fee_chip_sidelobe_ratio_even_[ felix ]->GetName() );
            
      tf_output_->WriteTObject( hist_fee_chip_sidelobe_ratio_bco_even_[ felix ],
				hist_fee_chip_sidelobe_ratio_bco_even_[ felix ]->GetName() );
            
    }

  // Close the ROOT file
  tf_output_->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScanTrkrHit::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int InttFineDelayScanTrkrHit::Reset(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

void InttFineDelayScanTrkrHit::Print(const string &what) const
{
  int width = 100;
  cout << string( width, '-' ) << endl;
  cout << " InttFineDelayScanTrkrHit" << endl;
  cout << "  - Output (ROOT): " << output_root_ << endl;
  cout << "  - Output  (PDF): " << output_pdf_ << endl;
  cout << "  - Output  (txt): " << output_txt_ << endl;
  cout << string( width, '-' ) << endl;
  // this->PrintLine( "Class", "InttFineDelayScanTrkrHit" );
//   this->DumpPrintBuffer();
}
