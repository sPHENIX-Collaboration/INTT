#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

int beam_parameter_20220207()
{

  string data = "repo/build/ED.root";
  gStyle->SetOptStat( true );
  gStyle->SetOptFit( true );
  TFile* tf = new TFile( data.c_str(), "READ" );

  auto tr = (TTree*)tf->Get( "Beam_angle" );
  //  tr->Print();

  int pid = -11; // e+? beam particle anyway

  string expression_x = "Beam_X";
  string expression_y = "Beam_Y";
  string expression_r = string("sqrt( pow( ") + expression_x + ", 2) + pow( " + expression_y + ",  2))";

  string expression = "";
  string expression_px = "Beam_Px";
  string expression_py = "Beam_Py";
  string expression_pz = "Beam_Pz";
  string expression_pt = string("sqrt( pow( ") + expression_px + ", 2) + pow(" + expression_py + ",  2))";
  string expression_p = string("sqrt(") + "pow(" + expression_pt + ", 2) + pow( " + expression_pz + ", 2)" + ")";

  string expression_e = "Beam_energy";
  string cut = "";
  
  string output = "results/ELPH/single_use/beam_parameter_20220207.pdf";
  TFile* tf_output = new TFile( Replace( output, ".pdf", ".root" ).c_str(), "RECREATE" );
  
  TCanvas* c = GetCanvas( output, "square", false );
  c->Divide( 3, 3 );

  ////////////////////////////////////
  // Plan for canvas
  // +--------+--------+--------+
  // |        |        |        | 
  // | 1( x ) | 2(x y) | 3( y ) |
  // |        |        |        | 
  // +--------+--------+--------+
  // |        |        |        | 
  // | 4(x z) | 5(xyz) | 6(y z) |
  // |        |        |        | 
  // +--------+--------+--------+
  // |        |        |        | 
  // | 7(misc)| 8( z ) | 9(misc)|
  // |        |        |        | 
  // +--------+--------+--------+
  //

  c->Print( ((string)c->GetName()+"[").c_str() );

  // px vs py vs pz /////////////////////////////////////////////////////////////////
  TH3D* hist_px_py_pz = new TH3D( "px_vs_py_vs_pz",
				 "px vs py vs pz; p_{x} (MeV/c);p_{y} (MeV/c);p_{z} (MeV/c);Entires",
				  200, -20, 20, // px
				  200, -20, 20, // py
				  2000, 700, 1100 ); //px
  expression = expression_pz + ":"
    + expression_py + ":"
    + expression_px +
    ">>" + hist_px_py_pz->GetName();

  tr->Draw( expression.c_str(), cut.c_str(), "goff" );


  // px vs py vs pz
  c->cd( 5 );
  hist_px_py_pz->Draw();

  // px vs py 
  auto hist_px_py = (TH2D*)hist_px_py_pz->Project3D( "yx" );
  c->cd( 2 );
  hist_px_py->Draw( "colz" );
  DrawStats( hist_px_py, 0.75, 0.75, 1.0, 1.0 );
  DrawPaletteAxis( hist_px_py, 0.901, 0.1, 0.93, 0.7, 0.05 );

  // px vs py -> px
  auto hist_px = (TH1D*)hist_px_py->ProjectionX();
  c->cd( 1 );
  hist_px->Draw();
  DrawStats( hist_px, 0.7, 0.7, 1.0, 0.95 );
  
  // px vs py -> py
  auto hist_py = (TH1D*)hist_px_py->ProjectionY();
  c->cd( 3 );
  hist_py->Draw();
  DrawStats( hist_py, 0.7, 0.7, 1.0, 0.95 );

  auto hist_px_pz = (TH2D*)hist_px_py_pz->Project3D( "zx" );
  c->cd( 4 );
  hist_px_pz->Draw( "colz" );
  DrawStats( hist_px_pz, 0.75, 0.75, 1.0, 1.0 );
  DrawPaletteAxis( hist_px_pz, 0.901, 0.1, 0.93, 0.7, 0.05 );

  auto hist_pz = (TH1D*)hist_px_pz->ProjectionY();
  c->cd(8);
  hist_pz->Draw();
  DrawStats( hist_pz, 0.15, 0.55, 0.45, 0.85 );
    
  auto hist_py_pz = (TH2D*)hist_px_py_pz->Project3D( "zy" );
  c->cd( 6 );
  hist_py_pz->Draw( "colz" );
  DrawStats( hist_py_pz, 0.75, 0.75, 1.0, 1.0 );
  DrawPaletteAxis( hist_py_pz, 0.901, 0.1, 0.93, 0.7, 0.05 );

  // misc
  auto pad7 = c->cd(7);
  TH1D* hist_p = new TH1D( "total_momentum", "Total momentum;Total momentum (MeV/c);Entries",
			   200,
			   hist_pz->GetMean() * 0.9,
			   hist_pz->GetMean() * 1.1 );
			   
  expression = expression_p + ">>" + hist_p->GetName();
  tr->Draw( expression.c_str(), cut.c_str(), "" );
  DrawStats( hist_p, 0.15, 0.55, 0.45, 0.85 );
  
  c->cd( 9 );
  TH1D* hist_pt = new TH1D( "transverse_momentum", "Transverse momentum;Transverse momentum (MeV/c);Entries",
			    200, 0, 20 );
  expression = expression_pt + ">>" + hist_pt->GetName();
  tr->Draw( expression.c_str(), cut.c_str(), "" );
  DrawStats( hist_pt, 0.7, 0.7, 1.0, 0.95 );
  
  c->Print( c->GetName() );
  c->Clear();

  // p vs x vs y /////////////////////////////////////////////////////////////////
  c->Divide( 3, 3 );
  TH3D* hist_p_x_y = new TH3D( "p_vs_x_vs_y",
				 "p vs x vs y; total momentum (MeV/c);x (mm); y (mm);Entires",
			       //			       6000, 0, 1200, //p
			       2000, 700, 1100, //p
			       200, -50, 50, // x
			       200, -50, 50 ); // y
  expression = expression_y + ":"
    + expression_x + ":"
    + expression_p +
    ">>" + hist_p_x_y->GetName();
  
  tr->Draw( expression.c_str(), cut.c_str(), "goff" );

  // p vs x vs y
  c->cd( 5 );
  hist_p_x_y->Draw();

  // p vs x 
  auto hist_p_x = (TH2D*)hist_p_x_y->Project3D( "yx" );
  c->cd( 2 );
  hist_p_x->Draw( "colz" );
  DrawStats( hist_p_x, 0.75, 0.75, 1.0, 1.0 );
  DrawPaletteAxis( hist_p_x, 0.901, 0.1, 0.93, 0.7, 0.05 );

  // p vs x -> p
  hist_p = (TH1D*)hist_p_x->ProjectionX();
  c->cd( 1 );
  hist_p->Draw();
  DrawStats( hist_p, 0.75, 0.75, 1.0, 1.0 );
  
  // p vs x -> x
  auto hist_x = (TH1D*)hist_p_x->ProjectionY();
  c->cd( 3 );
  hist_x->Draw();
  DrawStats( hist_x, 0.75, 0.75, 1.0, 1.0 );

  auto hist_p_y = (TH2D*)hist_p_x_y->Project3D( "zx" );
  c->cd( 4 );
  hist_p_y->Draw( "colz" );
  DrawStats( hist_p_y, 0.75, 0.75, 1.0, 1.0 );
  DrawPaletteAxis( hist_p_y, 0.901, 0.1, 0.93, 0.7, 0.05 );

  auto hist_y = (TH1D*)hist_p_y->ProjectionY();
  c->cd(8);
  hist_y->Draw();
  DrawStats( hist_y, 0.75, 0.75, 1.0, 1.0 );
    
  auto hist_x_y = (TH2D*)hist_p_x_y->Project3D( "zy" );
  c->cd( 6 );
  hist_x_y->Draw( "colz" );
  DrawStats( hist_x_y, 0.75, 0.75, 1.0, 1.0 );
  DrawPaletteAxis( hist_x_y, 0.901, 0.1, 0.93, 0.7, 0.05 );

  c->cd( 9 );
  TH1D* hist_r = new TH1D( "r", "Radial distance from the origin;Radial distance (mm);Entries",
			    200, 0, 40 );
  expression = expression_r + ">>" + hist_r->GetName();
  tr->Draw( expression.c_str(), cut.c_str(), "" );
  DrawStats( hist_r, 0.7, 0.7, 1.0, 0.95 );

  c->Print( c->GetName() );

  c->Print( ((string)c->GetName() +"]").c_str() );

  tf_output->WriteTObject( hist_px_py_pz, hist_px_py_pz->GetName() );
  tf_output->WriteTObject( hist_p_x_y, hist_p_x_y->GetName() );
  tf_output->Close();

  tf->Close();
  return 0;
}
