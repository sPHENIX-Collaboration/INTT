#include "event.hh"

using namespace std;
///////////////////////////////////////////////////////////////
// Public                                                    //
///////////////////////////////////////////////////////////////
event::event() :
  line3d_parameters(6),
  fit_estimator( -1 ),
  line_best_xy( new TF1("f_xy", "pol1", 0, 1) ),
  line_best_zy( new TF1("f_zy", "pol1", 0, 1) ),
  line_xy_fit1d( new TF1("f_xy_fit1d", "pol1", 0, 1) ),
  line_zy_fit1d( new TF1("f_zy_fit1d", "pol1", 0, 1) )
{
  
};

event::~event()
{
  this->Clear();

}

void event::AddHit( double x, double y, double z, int cluster_size_arg, double error_phi_arg, double error_z_arg, int adc )
{
  cluster_size.push_back( cluster_size_arg );
  ROOT::Math::XYZVector vec( x, y, z );
  
  positions.push_back( vec );
  error_phi.push_back( error_phi_arg );
  error_z.push_back( error_z_arg );
  adcs.push_back( adc );  
}


void event::Clear()
{
  this->ClearVector( positions );
  this->ClearVector( cluster_size );
  this->ClearVector( error_phi );
  this->ClearVector( error_z );
  this->ClearVector( adcs );
  this->ClearVector( line3d_parameters );

  event_num = 0;
  hit_num = 0;
  fit_estimator = -1;

  line_best_xy->SetParameters( -999, -999 );
  line_best_zy->SetParameters( -999, -999 );

  line_xy_fit1d->SetParameters( -999, -999 );
  line_zy_fit1d->SetParameters( -999, -999 );
}

void event::Process()
{
  hit_num = positions.size();
  this->Fit();
}

///////////////////////////////////////////////////////////////
// Private                                                   //
///////////////////////////////////////////////////////////////
void event::Fit()
{
  g_all_hits_ = new TGraph2DErrors();
  TGraphErrors* g_all_hits_xy_ = new TGraphErrors();
  TGraphErrors* g_all_hits_zy_ = new TGraphErrors();
  //  for( const auto& hit : positions )
  for( int i=0; i<positions.size(); i++ )
    {
      const auto hit = positions[i];

      // TGraph2DErrors
      g_all_hits_->SetPoint( g_all_hits_->GetN(), hit.X(), hit.Y(), hit.Z() );
      g_all_hits_->SetPointError( g_all_hits_->GetN()-1, error_phi[i], error_phi[i], error_z[i] );

      // TGraphErrors, x-y
      g_all_hits_xy_->SetPoint( g_all_hits_xy_->GetN(), hit.X(), hit.Y() );
      g_all_hits_xy_->SetPointError( g_all_hits_xy_->GetN()-1, error_phi[i], error_phi[i] );
      
      // TGraphErrors, z-y
      g_all_hits_zy_->SetPoint( g_all_hits_zy_->GetN(), hit.Z(), hit.Y() );
      g_all_hits_zy_->SetPointError( g_all_hits_zy_->GetN()-1, error_z[i], error_phi[i] );
      
    }

  double sphenix_radius = 500; // 5 m = 500 cm
  
  // Fitting can be done if more than 1 point are given
  if( positions.size() > 1 )
    {
      line_xy_fit1d = new TF1( "line_xy_1d", "pol1", -sphenix_radius, sphenix_radius );
      this->Fit1D( g_all_hits_xy_, line_xy_fit1d );
      
      line_zy_fit1d = new TF1( "line_zy_1d", "pol1", -sphenix_radius, sphenix_radius );
      this->Fit1D( g_all_hits_zy_, line_zy_fit1d );
      
      this->Fit3D( g_all_hits_ );
    }
  
}

void event::Fit1D( TGraphErrors* g, TF1* f )
{
  f->SetParameters( 0, 1.0 ); // intercept and slope
  g->Fit( f,  "QF"  ); // Quiet mode, Fitting with the liner fitter.
  //g->Print();
  //cout << f->GetParameter(0) << "\t" << f->GetParameter(1) << endl;
  
}

void event::Fit3D( TGraph2DErrors* g )
{
  ROOT::Fit::Fitter  fitter;
  const unsigned int kParameter_num = 6;
  
  // make the functor objet
  SumDistance2 sdist( g );
  ROOT::Math::Functor fcn(sdist, kParameter_num );
  
  // set the function and the initial parameter values
  double pStart[ kParameter_num ] = { 0, 1, 0, 1, 0, 1 };
  fitter.SetFCN( fcn, pStart );

  // set step sizes different than default ones (0.3 times parameter values)
  for (int i = 0; i < kParameter_num; ++i)
    fitter.Config().ParSettings(i).SetStepSize( 0.0001 );

  bool ok = fitter.FitFCN();
  if (!ok) {
    Error("line3Dfit","Line3D Fit failed");
    return;
  }

  const ROOT::Fit::FitResult & result = fitter.Result();
  fit_estimator = result.MinFcnValue();
  // std::cout << "Total final distance square " << result.MinFcnValue() << std::endl;
  // result.Print( std::cout );
  // get fit parameters
  const double * parFit = result.GetParams();

  for( int i=0; i<kParameter_num; i++ )
    {
      //      cout << setw(10) << setprecision(3) << parFit[i] << "\t";
      line3d_parameters.push_back( parFit[i] );
    }

  // cout << setw(10) << setprecision(3) << -parFit[0] * parFit[2] / parFit[1] << "\t"
  //      << setw(10) << setprecision(3) << parFit[3] / parFit[1] << "\t"
  //      << setw(10) << setprecision(3) <<  -parFit[2] * parFit[4] / parFit[5] << "\t"    
  //      << setw(10) << setprecision(3) << parFit[3] / parFit[5]
  //      << endl;

  double sphenix_radius = 500; // 5 m = 500 cm
  line_best_xy		= new TF1( "line_xy", "pol1", -sphenix_radius, sphenix_radius );
  double slope_xy	= parFit[3] / parFit[1] ;
  double intercept_xy	= parFit[2] - parFit[0] * slope_xy;
  line_best_xy->SetParameters( intercept_xy, slope_xy );

  line_best_zy		= new TF1( "line_zy", "pol1", -sphenix_radius, sphenix_radius );
  double slope_zy	= parFit[3] / parFit[5] ;
  double intercept_zy	= parFit[2] - parFit[4] * slope_zy;
  line_best_zy->SetParameters( intercept_zy, slope_zy );
  
}

template < class T >
void event::ClearVector( vector < T > &vec )
{
  vec.erase( vec.begin(), vec.end() );
}



// define the parametric line equation
void FittingFunction_line(double t, const double *p, double &x, double &y, double &z)
{
   // a parametric line is define from 6 parameters but 4 are independent
   // x0,y0,z0,x1,y1,z1 which are the coordinates of two points on the line
   // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1;
   x = p[0] + p[1] * t;
   y = p[2] + p[3] * t;
   z = p[4] + p[5] * t;
   //z = t;
}

