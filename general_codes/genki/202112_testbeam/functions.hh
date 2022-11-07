#pragma once

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

TGaxis* GetTimeAxisFromTDC( TH1* hist )
{

  // the first and the last bin which contains non-0 entries
  double tdc_min = 0, tdc_max = 0;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    if( hist->GetBinContent(i) != 0 )
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

  return axis_run;
}
