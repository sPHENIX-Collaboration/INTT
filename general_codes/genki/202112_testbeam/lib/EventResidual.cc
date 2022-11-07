#include "EventResidual.hh"

EventResidual::EventResidual( int num, double chan0, double chan1, double chan2 )
{
  double x[3];
  x[0] = 0;
  x[1] = 1;
  x[2] = 2;
  
  chan_id_[0] = chan0;
  chan_id_[1] = chan1;
  chan_id_[2] = chan2;
  
  g_ = new TGraph(3, x, chan_id_ );    
  GraphSetting( g_, kBlack, 2, 7 );
  
  Init();
};

EventResidual::EventResidual( TGraph* g )
{
  g_ = g;
  Init();
};


void EventResidual::Print()
{
  cout << "Event&Residual: ";
  
  for( int i=0; i<g_->GetN(); i++ )
    {
      double x, y;
      g_->GetPoint(i, x, y );
      cout << "(" << x << ", " << setw(4) << y << "), ";
    }
  
  cout << "  --->  ";
  
  for( int i=0; i<3; i++ )
    {
      cout << setw(7) << setprecision(3) << this->GetResidual(i) << ", ";
    }
  
  cout << "\t";
  cout << "y = "
       << setw(5) << setprecision(3) << track_->GetParameter(1) << " * x + "
       << setw(5) << setprecision(3) << track_->GetParameter(0) << " ";
  
  cout << setw(8) << setprecision(5) << this->GetResidual(0) - this->GetResidual(1) << "\t"
       << setw(8) << setprecision(5) << this->GetResidual(0) - this->GetResidual(2) << "\t";
  
  cout << endl;
};


// private functions
void EventResidual::Init()
{
  track_ = new TF1( "f", "pol1", 0, 2 );
  g_->Fit( track_, "QC" ); // C: no chi-square calc to save time
};
