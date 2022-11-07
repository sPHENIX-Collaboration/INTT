#define __CINT__

#include "NoisyChannelList.hh"
#include "functions.hh"


int test()
{

  /*
  TF1* f = new TF1( "f", "gaus(0) + gaus(3)", -10, 10 );
  f->SetParameters( 10, 0, 1, 1, 0, 3 );
  */

  TF1* f = new TF1( "f", "gaus(0) + gaus(3)", -10, 10 );
  f->SetNpx( 1e4 );
  //  f->SetParameters( 10, 0, 1, 1e-9, 0 , 1e9 );
  f->SetParameter(0, 2.351e4 );
  f->SetParameter(1, 0.3706 );
  f->SetParameter(2, 0.3167 );
  f->SetParameter(3, 2.257 );
  f->SetParameter(4, f->GetParameter(1) );
  f->SetParameter(5, 7.047 );

  double xmin, xmax;
  Get3SigmaRange( f, xmin, xmax );

  f->Draw();
  TLine* line = new TLine();
  line->DrawLine( xmin, 0, xmin, f->GetParameter(0) );
  line->DrawLine( xmax, 0, xmax, f->GetParameter(0) );
  
  return 0;
}

/*
int test()
{

  NoisyChannelList* ncl = new NoisyChannelList( "documents/2021_test_beam/run_list - Noisy_ch_list.tsv" );
  ncl->Print();

  for( int run=21; run<90; run++ )
    {
      if( ncl->GetCut( run ) != "" )
	{
	  //cout << "Run" << run << " " << ncl->GetCut( run ) << endl;
	  cout << "Run" << run << " " << ncl->GetCutWithChipColumn( run ) << endl;
	}
    }
  // cout << ncl->GetCut( 21 ) << endl;
  // cout << ncl->GetCutWithChipColumn( 21 ) << endl;
  return 0;
}
*/
