#define __CINT__

#ifdef __linux__
#include "/home/genki/soft/MyLibrary/include/MyLibrary.hh"

#elif __APPLE__
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"

#endif

/*!
  @file bco_phase.cc
  @brief BCO phase distribution is made from the parameter trend, and draw it
*/

/*!
  @func double TDC2Ns( double tdc )
  @brief It converts TDC ch to time (ns). 
  @details The calibration was done by Y. Namimoto, and reported at the INTT Japanese meeting in Nov/17/2021.
  y (ch) = 6.1587 x - 61.45 (ns), here y = ax + b  --> x = (y-b)/a, therefore x = (tdc + 61.45) / 6.1587.
*/
double TDC2Ns( double tdc )
{
  return (tdc + 61.45) / 6.1587;
}

int bco_phase( string data_arg = "" )
{
  // read the ROOT file
  string camac_name = "camac_tdc3";
  string result_dir = "results/ELPH/bco/";
  string data = result_dir + "trend_over_runs/" + camac_name + ".root";

  if( data_arg != "" )
    data = data_arg;
  
  TFile* tf = new TFile( data.c_str(), "READ" );

  
  // Make canvas, and draw the hist on it
  string output = result_dir;
  if( data_arg == "" )
    output += "bco_phase.pdf";
  else
    output += GetBaseName( data ) + ".pdf";

  string output_root = Replace( output, ".pdf", ".root" );
  TFile* tf_output = new TFile( output_root.c_str(), "RECREATE" );
  
  TCanvas* c = GetCanvas( output, "3:2", false );

  TH1D* hist;
  if( data_arg == "" )
    {
      TH2D* hist2d = (TH2D*)tf->Get( camac_name.c_str() );
      //  hist2d->Draw( "colz" );
      
      // Make projection to get BCO phase distribution. The first 55 bins are ignored because the distribution looks different
      hist = hist2d->ProjectionY( "hist", 56, -1, "" );
    }
  else
    {
      
      hist = (TH1D*)tf->Get( "BCO_phase" );
      hist->SetName( "hist" );
    }
  
  hist->SetTitle( "BCO phase;TDC ch;Entries" );
  // Make x-axis in ns

  // the first and the last bin which contains non-0 entries
  double tdc_min = 0, tdc_max = 0;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    if( hist->GetBinContent(i) != 0 && i != 1)
      {
	tdc_min = hist->GetBinCenter(i);
	break;
      }

  for( int i=hist->GetNbinsX()+1; i>-1; i-- )
    if( hist->GetBinContent(i) != 0 )
      {
	tdc_max = hist->GetBinCenter(i);
	break;
      }

  double ns_min = TDC2Ns( tdc_min );
  double ns_max = TDC2Ns( tdc_max );

  double ns_first = ns_min - ns_min;
  double ns_last = ns_max - ns_min;

  // Y position of the time-axis
  double pos_y = GetMaxBinContent( hist );
  
  TGaxis* axis_run = new TGaxis( tdc_min, pos_y, tdc_max, pos_y, ns_first, ns_last, 10 + 2 * 100 + 2 * 10000, "+-" );
  axis_run->SetTitle( "time (ns)" );
  axis_run->SetLineWidth( 2 );
  axis_run->SetLineColor( kRed );
  axis_run->SetLabelColor( axis_run->GetLineColor() );
  axis_run->SetTitleColor( axis_run->GetLineColor() );
  axis_run->SetLabelOffset( 0.02 );
  axis_run->SetTitleOffset( -0.5 );
    
  hist->Draw( "HISTE" );
  hist->GetXaxis()->SetRangeUser( tdc_min - 50, tdc_max + 50);
  axis_run->Draw( "same" );

  // save the canvas
  c->Print( c->GetName() );

  tf_output->WriteTObject( hist, hist->GetName() );
  tf_output->WriteTObject( axis_run, "time_axis" );
  tf_output->Close();
  return 0;
}
