#include <vector>

#include <TF1.h>
#include <TF2.h>
#include <TH1.h>
#include <TGraphErrors.h>
#include <TGraph2DErrors.h>
#include <TMath.h>
#include <TGraph2D.h>
#include <Math/Functor.h>
#include <Math/Vector3D.h>
//#include <Math/Vector3Dfwd.h>
#include <TPolyLine3D.h>
#include <Fit/Fitter.h>

void FittingFunction_line(double t, const double *p, double &x, double &y, double &z);

class event
{  
public:
  event();
  ~event();

  int event_num = 0;
  Long64_t bco_full = 0;
  int hit_num = 0;

  std::vector < int > cluster_size;
  std::vector < ROOT::Math::XYZVector > positions;
  //std::vector < ROOT::Math::RhoZPhiVector > positions_rzphi; // no need
  
  std::vector < float > error_z;
  std::vector < float > error_phi;
  std::vector < int > adcs;
  std::vector < double > line3d_parameters;
  double fit_estimator;
  TF1* line_best_xy;
  TF1* line_best_zy;
  TF1* line_xy_fit1d;
  TF1* line_zy_fit1d;

  void AddHit( double x, double y, double z, int cluster_size, double error_phi_arg, double error_z_arg, int adc );
  void Clear();
  void Process();

  void SetEventId( int num ){ event_num = num;};
  void SetBcoFull( Long64_t val ){ bco_full = val; };
  
private:

  template < class T >
  void ClearVector( std::vector < T > &vec );
  TGraph2DErrors* g_all_hits_;
  void Fit();
  void Fit1D( TGraphErrors* g, TF1* f );
  void Fit3D( TGraph2DErrors* g );
  
};

// function Object to be minimized
struct SumDistance2
{
  // the TGraph is a data member of the object
  TGraph2DErrors *fGraph;
  
  SumDistance2(TGraph2DErrors *g) : fGraph(g) {};

  // calculate distance line-point
  double distance2(double x,double y,double z, const double *p)
  {
    // distance line point is D= | (xp-x0) cross  ux |
    // where ux is direction of line and x0 is a point in the line (like t = 0)
    ROOT::Math::XYZVector xp(x,y,z);
    ROOT::Math::XYZVector x0(p[0], p[2], p[4] );
    //ROOT::Math::XYZVector x0(p[0], p[2], 0 );
    ROOT::Math::XYZVector x1(p[0] + p[1], p[2] + p[3], p[4] + p[5] );
    //    ROOT::Math::XYZVector x1(p[0] + p[1], p[2] + p[3], 1 );
    ROOT::Math::XYZVector u = (x1-x0).Unit();
    double d2 = ((xp-x0).Cross(u)).Mag2();
    return d2;
  };
  
  // implementation of the function to be minimized
  double operator() (const double *par)
  {
    assert(fGraph != 0);
    double * x = fGraph->GetX();
    double * y = fGraph->GetY();
    double * z = fGraph->GetZ();
    int npoints = fGraph->GetN();
    double sum = 0;
    for (int i  = 0; i < npoints; ++i) {
      double d = distance2(x[i],y[i],z[i],par);
      sum += d;
    }

    return sum;
  };
  
};
