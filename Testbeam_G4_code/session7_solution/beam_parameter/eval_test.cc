/*
  @brief Test of the interpolation of the beam momentum spread
 */
int eval_test(  string data = "Au_20um.root" )
{

  // draw the original data graph
  TFile* tf = new TFile( data.c_str(), "READ" );
  TGraph* g = (TGraph*)tf->Get( "momentum_vs_spread" );
  g->SetMarkerStyle( 20 );
  g->Draw( "AP" );

  // make an interpolated graph
  TGraph* g_eval = new TGraph();
  for( int i=0; i<120; i++ )
    {
      double energy = i * 10; // MeV
      double spread = g->Eval( energy );

      g_eval->SetPoint( g_eval->GetN (), energy, spread );

    }
  
  g_eval->SetMarkerStyle( 21 );
  g_eval->SetMarkerColorAlpha( kRed, 0.2 );

  // if the alpha channel doesn't work:
  // g_eval->SetMarkerColor( kRed );
  // g_eval->SetMarkerSize( 0.5 );

  g_eval->Draw( "P" );
  return 0;
}
