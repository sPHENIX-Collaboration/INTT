#define __CINT__

//#include "StepMC.hh"
//#include "TrackMC.hh"
//#include "TrackMC.cc"
//#include "StepMC.cc"
#include "DrawHitMap.c"

//R__LOAD_LIBRARY( "lib/libEventa.dylib" );
//R__LOAD_LIBRARY( "lib/libStepMC.dylib" );
//R__LOAD_LIBRARY( "lib/libTrackMC.dylib" );

void DrawStatus( string comments_in_one )
{
  
  gPad->Clear();
  TLatex* tex = new TLatex();

  vector < string > comments;
  stringstream ss( comments_in_one );
  string words;

  while( getline( ss, words, ';' ) )
    comments.push_back( words );

  for( int i=0; i<comments.size(); i++ )
    tex->DrawLatexNDC( 0.1, 0.9 - i * 0.1, comments[i].c_str() );

}

void DrawLog( TrackMC* track )
{
  

}

template < class T >
void FrameSetting( T* object )
{
  object->GetXaxis()->CenterTitle();
  object->GetXaxis()->SetTitleSize( 0.05 );
  object->GetXaxis()->SetTitleOffset( 0.9 );
  object->GetXaxis()->SetLabelSize( 0.06 );
  object->GetXaxis()->SetLabelOffset( 0.03 );

  object->GetYaxis()->CenterTitle();
  object->GetYaxis()->SetTitleSize( 0.05 );
  object->GetYaxis()->SetTitleOffset( 0.9 );
  object->GetYaxis()->SetLabelSize( 0.06 );
  object->GetYaxis()->SetLabelOffset( 0.03 );

}

void SetGoodYRange( TMultiGraph* mg, double xmin, double xmax )
{
  auto list = mg->GetListOfGraphs();
  vector < double > yvalues;

  for( int i=0; i<list->GetEntries(); i++ )
    {
      auto g = (TGraph*)list->At(i);
      for( int j=0; j<g->GetN(); j++ )
	{
	  double x, y;
	  g->GetPoint(j, x, y );
	  if( x < xmin || xmax < x )
	    continue;

	  if( fabs(y) > 20.0 )
	    continue;
	  
	  yvalues.push_back( y );
	}
    }

  
  double min = *min_element( yvalues.begin(), yvalues.end() );
  double max = *max_element( yvalues.begin(), yvalues.end() );
  double range = fabs(min) > fabs(max) ? fabs(min) : fabs(max);
  
  double margin = 1.1;
  TH1* frame = gPad->DrawFrame( xmin, -range * margin, xmax, range * margin );
  FrameSetting( frame );
  frame->SetTitle( mg->GetTitle() );
  frame->GetXaxis()->SetTitle( mg->GetXaxis()->GetTitle() );
  frame->GetYaxis()->SetTitle( mg->GetYaxis()->GetTitle() );

  mg->Draw( "PL" );  
}


void Analysis_RealData( TCanvas* c, TTree* tree_both, int event_num )
{
  vector < int > modules(4);
  modules[0] = 6;
  modules[1] = 5;
  modules[2] = 8;
  modules[3] = 2;

  auto pad = c->cd( 5 );
  pad->Clear();
  
  pad->Divide( 1, 4 );
  for( int i=0; i<modules.size(); i++ )
    {

      string cut = "module == " + to_string( modules[i] );
      cut +=   " && Entry$ == " + to_string( event_num );

      string expression = "chan_id:chip_id";
      //int DrawHitMap(TTree* tree, string cut, string option, int bin_ch) {
      auto hit_map = DrawHitMap( tree_both, cut, "colz" );

      string hist_title = "#Ev " + to_string( event_num ) + ", ladder" + to_string(i) + ";"
	"Chip ID; Channel ID";
      hit_map->SetTitle( hist_title.c_str() );
      FrameSetting( hit_map );
      pad->cd( i + 1 );
      hit_map->Draw( "colz" );

    }


  pad = c->cd(6);
  pad->Clear();


  string expression_module = "";
  for( int i=0; i<modules.size(); i++ )
    {
      expression_module += "(module==" + to_string( modules[i] ) + ")*" + to_string( i ) + "+";
    }
  expression_module = expression_module.substr( 0, expression_module.size() - 1 );
  
  string cut = "Entry$==" + to_string( event_num );
  
  pad->Divide( 3, 1 );

  pad->cd(1);
  string expression_adc = "adc:" + expression_module;
  tree_both->Draw( expression_adc.c_str(), cut.c_str(), "colz" );

  pad->cd(2);
  tree_both->Draw( expression_module.c_str(),cut.c_str(), "HIST" );

  pad->cd(3);
  string expression = "chan_id:" + expression_module;
  
  //cout << expression << endl;
  tree_both->SetMarkerStyle( 20 );
  tree_both->SetMarkerColor( kRed );
  tree_both->Draw( expression.c_str(), cut.c_str(), "P" );
}

void Analysis( TCanvas* c, const vector < TrackMC* >* tracks, const vector < StepMC* >* steps, TTree* tree_both )
{

  /*
  int hit_num = tree_both->Draw( "adc", ((string)"Entry$==" + to_string( (*tracks)[0]->GetEventID()) ).c_str(), "goff" );
  if( hit_num < 12 )
    return;
  */

  if( tracks->size() != 0 )
    cout << "#Event " << (*tracks)[0]->GetEventID() << "\t";
  
  cout << tracks->size() << "\t" << steps->size() << endl;
  //  cout << "#hit: " << hit_num << endl;

  

  
  // TH1D* hist_ep = new TH1D( "hist_ep", "title", 500, 0, 5000 );
  // hist_ep->SetLineColor( kRed );
  
  // TH1D* hist_en = new TH1D( "hist_en", "title", 500, 0, 50 );
  // hist_en->SetLineColor( kBlue );

  cout << "loop over tracks" << endl;
  int counter = 0;
  double zmin = -40, zmax = 40; // used to zoom graphs

  TMultiGraph* mg_xy = new TMultiGraph( "tracks_in_xy", "tracks_in_xy" );
  TMultiGraph* mg_xy_zoomed = new TMultiGraph( "tracks_in_xy_zoomed", "tracks_in_xy_zoomed" );

  TMultiGraph* mg_zx = new TMultiGraph( "tracks_in_zx", "tracks_in_zx" );

  TMultiGraph* mg_zy = new TMultiGraph( "tracks_in_zy", "tracks_in_zy" );

  for( const auto& track : *tracks )
    {
      string name = track->GetName();

      //track->GetGraph()->Print();
      auto g_xy = track->GetGraphOnXY();
      mg_xy->Add( g_xy );

      auto g_xy_zoomed = track->GetGraphOnXY( zmin, zmax );
      if( g_xy_zoomed->GetN() > 0 )
	mg_xy_zoomed->Add( g_xy_zoomed );

      //      g_xy_zoomed->Print();

      auto g_zx = track->GetGraphOnZX();
      mg_zx->Add( g_zx );

      auto g_zy = track->GetGraphOnZY();
      mg_zy->Add( g_zy );

      DrawLog( track );
    }


  //  mg_xy->SetFillStyle( 0 );
  mg_xy->SetTitle( (*tracks)[0]->GetGraphOnXY()->GetTitle() );
  mg_xy->GetXaxis()->SetTitle( (*tracks)[0]->GetGraphOnXY()->GetXaxis()->GetTitle() );
  mg_xy->GetYaxis()->SetTitle( (*tracks)[0]->GetGraphOnXY()->GetYaxis()->GetTitle() );

  mg_zx->SetTitle( (*tracks)[0]->GetGraphOnZX()->GetTitle() );
  mg_zx->GetXaxis()->SetTitle( (*tracks)[0]->GetGraphOnZX()->GetXaxis()->GetTitle() );
  mg_zx->GetYaxis()->SetTitle( (*tracks)[0]->GetGraphOnZX()->GetYaxis()->GetTitle() );

  mg_zy->SetTitle( (*tracks)[0]->GetGraphOnZY()->GetTitle() );
  mg_zy->GetXaxis()->SetTitle( (*tracks)[0]->GetGraphOnZY()->GetXaxis()->GetTitle() );
  mg_zy->GetYaxis()->SetTitle( (*tracks)[0]->GetGraphOnZY()->GetYaxis()->GetTitle() );
  
  FrameSetting( mg_xy );
  FrameSetting( mg_xy_zoomed );
  FrameSetting( mg_zx );
  FrameSetting( mg_zy );

  c->cd(1);
  stringstream status;
  status << "#Event " << (*tracks)[0]->GetEventID() << ";"
    //<< "whole view;"
	 << "#Tracks: " << tracks->size() << ";";
  
  DrawStatus( status.str() );
  
  c->cd(2);
  mg_zy->Draw( "APL" );
  
  c->cd(3);
  mg_xy->Draw( "APL" );

  c->cd(4);
  mg_zx->Draw( "APL" );
  gPad->Update();

  c->cd( 5 );
  Analysis_RealData( c, tree_both, (*tracks)[0]->GetEventID() );
  
  c->Print( c->GetName() );
  
  c->cd(1);
  status << "zoomed ragion: [" << zmin << ", " << zmax << "] mm in z;";
  DrawStatus( status.str() );
  
  c->cd( 2 );
  mg_zy->GetXaxis()->SetRangeUser( zmin, zmax );
  SetGoodYRange( mg_zy, zmin, zmax );
  
  c->cd(3);
  mg_xy_zoomed->Draw( "APL" );

  c->cd( 4 );
  mg_zx->GetXaxis()->SetRangeUser( zmin, zmax );
  SetGoodYRange( mg_zx, zmin, zmax );
  
  //mg_zx->GetYaxis()->SetRangeUser( -20, 20 );
  mg_zx->GetYaxis()->SetRangeUser( -2, 2 );
  gPad->Update();
  c->Print( c->GetName() );

  
}

int ana()
{
  // gInterpreter->GenerateDictionary("Event");
  // gInterpreter->GenerateDictionary("Event*");
  // gInterpreter->GenerateDictionary("vector<Event*>", "vector");
  //gInterpreter->GenerateDictionary("vector<TrackMC*>", "vector");
  // gInterpreter->GenerateDictionary("vector<Event>", "vector");
  //  gSystem->Load( "lib/libTrackMC.dylib" );
  
  TFile* tf =  new TFile( "test.root", "READ" );
  auto tr = (TTree*)tf->Get( "tree" );

  int event_id;
  vector < TrackMC* >* tracks( 0 );
  vector < StepMC* >* steps( 0 );

  tr->SetBranchAddress( "event", &event_id );
  tr->SetBranchAddress( "tracks", &tracks );
  tr->SetBranchAddress( "steps", &steps );

  auto tf_real = new TFile( "ED.root_convert.root", "READ" );
  auto tree_both = (TTree*)tf_real->Get( "tree_both" );
  
  double width = 1800, height = 900;
  TCanvas* c = new TCanvas( "event_display.pdf", "title", width, height );
  c->Print( ((string)c->GetName() + "[").c_str() );
  //  c->Divide( 2, 2 );

  int pad_num = 1;
  TPad* pad_status = new TPad( "pad1", "status",
			       0,
			       300.0 / height + 300.0 / height,
			       300.0 / width,  
			       1.0,
			       -1, 1, -2);
  ///  pad_status->SetFillColorAlpha( kGray, 0.3 );
  pad_status->Draw();
  pad_status->SetNumber( pad_num++ );

  TPad* pad_zy = new TPad( "pad1", "zy",
			   300.0 / width,  
			   300.0 / height + 300.0 / height,
			   300.0 / width + 900 / width,
			   300.0 / height + 300.0 / height + 300.0 / height,
			   -1, -1, -2);
  pad_zy->SetFillColorAlpha( kAzure + 1, 0.01 );
  pad_zy->Draw();
  //  pad_zy->SetMargin( 0, 0, 0, 0 );
  //pad_zy->SetMargin( 0.15, 0.01, 0.15, 0.01 );
  pad_zy->SetNumber( pad_num++ );

  TPad* pad_xy = new TPad( "pad2", "xy",
			   0.0,   
			   300.0 / height,
			   300.0 / width,
			   300.0 / height + 300.0 / height,
			   -1, -1, -2);
  pad_xy->SetFillColorAlpha( kAzure + 1, 0.01 );
  pad_xy->Draw();
  //pad_xy->SetMargin( 0.15, 0.05, 0.1, 0.1 );
  pad_xy->SetNumber( pad_num++ );

  TPad* pad_zx = new TPad( "pad3", "zx",
			   300.0 / width,
			   300.0 / height,
			   300.0 / width + 900 / width,
			   300.0 / height + 300.0 / height,
			   -1, -1, -2);
  pad_zx->SetFillColorAlpha( kAzure + 1, 0.01 );
  pad_zx->Draw();
  //pad_zx->SetMargin( 0.15, 0.05, 0.15, 0.05 );
  pad_zx->SetNumber( pad_num++ );

  TPad* pad_real = new TPad( "pad4", "real",
			    300.0 / width + 900 / width,
			    0.0, 
			    1.0,
			    1.0,
			    -1, -1, -2);
  pad_real->SetFillColorAlpha( kOrange + 1, 0.01 );
  pad_real->Draw();
  pad_real->SetNumber( pad_num++ );

  TPad* pad_real2 = new TPad( "pad5", "real2",
			      0.0, 
			      0.0, 
			      300.0 / width + 900 / width,
			      300.0 / height,
			      -1, -1, -2);
  pad_real2->SetFillColorAlpha( kOrange + 1, 0.01 );
  pad_real2->Draw();
  pad_real2->SetNumber( pad_num++ );

  int entries = tr->GetEntries();
  for( int event_id = 0; event_id<entries; event_id++ )
    {
      // if( event_id < 90 )
      // 	continue;
      
      tr->GetEntry( event_id );
      Analysis( c, tracks, steps, tree_both );

      //if( event_id ==130 )
       	break;
      
    }
  c->Print( ((string)c->GetName() + "]").c_str() );
  
  return 0;
}
