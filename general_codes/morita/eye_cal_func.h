//20210901
//MiuMorita

#ifndef INCLUDE_EYE_FUNCH
#define INCLUDE_EYE_FUNCH

#include "header.h"

//#include "eye_cal_0.C"
//#include "eye_cal_1.C"
//#include "eye_cal_1_0.C"
//#include "eye_cal_1_1.C"
//#include "eye_cal_2.C"
//#include "eye_cal_3.C"
//#include "eye_cal_3_0.C"
//#include "eye_cal_4.C"

TGraph* TexttoGraph(const char *fname){

 ifstream fin(fname);
 
 vector<double> x;
 vector<double> y;
 
 string sbuf;
 while(!fin.eof()){
    getline(fin,sbuf);
    TString stxt(sbuf);
    stxt.ReplaceAll(",", " ");
 
    istringstream iss(stxt.Data());

	 double tmpx, tmpy;

    iss >> tmpx >> tmpy;

	 x.push_back(tmpx);
	 y.push_back(tmpy);

 }
 
 TGraph *g = new TGraph(x.size(), &x[0], &y[0]);

 fin.close();

 return g;

}

int CheckTopBottom(TGraph *g){

 double x, y;

 int PP = 0;
 int MM = 0;

 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);
    //y = g -> GetPointY(i);
	 //GetPointY is used only version 6

	 if( y < 0 )MM++;
	 else PP++;
 }

 if ( MM < PP )return 1;
 else return -1;

}

TGraph* SwapTopBottom(TGraph *g){

 TGraph *g_ = new TGraph();
 double x, y;

 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);
	 g_ -> SetPoint(i, x, -y);
 }

 return g_;

}

double VMaximum(TGraph *g){

 double x, y;
 double max;
 g -> GetPoint(0, x, max);
 //double max = g -> GetPointY(0);

 for( int i = 1; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);
    //y = g -> GetPointY(i);

	 if( max < y )max = y;
 }

 return max;

}

double VMinimum(TGraph *g){

 double x, y;
 double min;
 g -> GetPoint(0, x, min);

 for(int i = 1; i < g -> GetN(); i++){
    g -> GetPoint(i, x, y);
    //y = g -> GetPointY(i);

	 if(y < min)min = y;
 }

 return min;

}

TFile* ReCreateTxttoRoot(const char *fname){

 TString foname(fname);
 foname.ReplaceAll(".txt", ".root");

 TFile *foroot = TFile::Open(foname.Data(), "recreate");

 cout << "recreate new root file : " << foname << endl;

 return foroot;

}

TFile* ReCreateRoot(const char *fname, string *pre){

 TString foname(fname);
 foname.Prepend(Form("%s", pre -> c_str()));

 TFile *foroot = TFile::Open(foname.Data(), "recreate");

 cout << "recreate new root file : " << foname << endl;

 return foroot;

}

TFile* ReCreateReplaceRoot(const char *fname, string *before, string *after){

 TString foname(fname);
 foname.ReplaceAll( Form( "%s", before -> c_str()), Form( "%s", after -> c_str()));

 TFile *foroot = TFile::Open(foname.Data(), "recreate");

 cout << "recreate new root file : " << foname << endl;

 return foroot;

}

/*double ConparisonGraphErrors( TGraphErrors *g1, TGraph Errors *g2 ){

 double y1, y1_error;
 double y2, y2_error;

 double dd;
 double dd_error;

 for( int i = 0; i < g1 -> GetN(); i++ ){
    y1       = g1 -> GetPointY( i );
    y1_error = g1 -> GetErrorY( i );

    y2       = g2 -> GetPointY( i );
    y2_error = g2 -> GetErrorY( i );

	 dd = y1 - y2;
	 dd_error = sqrt( ( y1_error * y1_error ) + ( y2_error * y2_error ) );



 }

}
*/

//TGraph* GetGraphXPart(TGraph *g, double min = NULL, double max = NULL){
TGraph* GetGraphXPart(TGraph *g, double min, double max){

 double x, y;

 TGraph *g_part = new TGraph();

 //if( min != NULL)
 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);

    //if( max != NULL && min < x && x < max ){
    if( min < x && x < max ){
    //if( min < x && x < max ){
       g_part -> SetPoint(i, x, y);
    }
    //if( max != NULL && max < y )continue;
    if( max < y )continue;
    //if( max < x )continue;
 }

 return g_part;

}

//TGraph* GetGraphYPart(TGraph *g, double min = NULL, double max = NULL){
TGraph* GetGraphYPart(TGraph *g, double min, double max){

 double x, y;

 TGraph *g_part = new TGraph();

 //if( min != NULL)
 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);

    //if( max != NULL && min < y && y < max ){
    if( min < y && y < max ){
    //if( min < x && x < max ){
       g_part -> SetPoint(i, x, y);
    }
    //if( max != NULL && max < x )continue;
    if( max < x )continue;
    //if( max < x )continue;
 }

 return g_part;

}

string axis_t = "time[s]";
string axis_V = "voltage[V]";

double tt, vv;

//double GetRiseCtrPoint(TGraph *g, int *yctr){
vector<double> GetRiseCtrPoint(TGraph *g, double yctr){

 double x, y;
 double xl, yl, xh, yh;
 double x1 = 0.;
 double x2 = 0.;
 double y1 = yctr;
 double y2 = yctr;
 double x_buf = 0.;

 double xmin, xmax;

 vector<double> xctr;

 int GATE = 0;

 g -> GetPoint(               0, xmin, y );
 g -> GetPoint( g -> GetN() - 1, xmax, y );

 //cout << ( xmax - xmin ) / g -> GetN() << endl;

 int NPOINT = int( 2.5e-9 / (( xmax - xmin ) / ( g -> GetN() - 1 )));
 //I search that the before and after point 2.5e-9[s] from rising point.

 //int T = 0;

 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);

	 if( y < yctr ){
       x1 = x;
		 y1 = y;
		 GATE = 1;
	 }
	 
	 if( y > yctr ){
       x2 = x;
		 y2 = y;
		 GATE = 2;
	 }
	 //if( GATE == 2 )g -> GetPoint( i - 400, x_, y_ );
	 //if( GATE == 2 )g -> GetPoint( i + 400, x__, y__ );
	 //if( GATE == 2 )g -> GetPoint( i - 100, x_, y_ );
	 //if( GATE == 2 )g -> GetPoint( i + 100, x__, y__ );

	 if( GATE == 2 )g -> GetPoint( i - NPOINT , xl, yl );
	 if( GATE == 2 )g -> GetPoint( i + NPOINT , xh, yh );

	 //if( y1 < yctr && yctr < y2 && GATE == 2 ){
	 if( y1 < yctr && yctr < y2 && GATE == 2 && yl < 0 && 0 < yh ){
       x_buf = ( x1 * y2 - x2 * y1 ) / ( y2 - y1 );
       xctr.push_back(x_buf);

		 x1 = 0.;
		 x2 = 0.;
		 y1 = yctr;
		 y2 = yctr;

		 GATE = 0;
		 //T++;
	 }
 }
 //cout << T << endl;

 return xctr;

}

vector<double> GetRiseCtrPoint10(TGraph *g, double yctr){

 double x, y;
 double xx[20] = {0.};
 double yy[20] = {yctr};
 double x_buf = 0.;

 vector<double> xctr;

 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);
	 for( int j = 0; j < 19; j++ ){
       xx[j] = xx[j + 1];
       yy[j] = yy[j + 1];
	 }
	 xx[19] = x;
	 yy[19] = y;

	 //if( yy[0] < yctr && yy[1] < yctr &&  yy[2] < yctr &&  yy[3] < yctr &&  yy[4] < yctr
	 // && yctr < yy[5] && yctr < yy[6] &&  yctr < yy[7] &&  yctr < yy[8] &&  yctr < yy[9] ){
	 
	 if( yy[0] < yy[1] && yy[1] < yy[2] &&  yy[2] < yy[3] &&  yy[3] < yy[4] &&  yy[4] < yy[5]
	  && yy[5] < yy[6] && yy[6] < yy[7] &&  yy[7] < yy[8] &&  yy[8] < yy[9] &&  yy[9] < yctr
	  && yctr   < yy[10] && yy[10] < yy[11] &&  yy[11] < yy[12] &&  yy[12] < yy[13] &&  yy[13] < yy[14]
	  && yy[14] < yy[15] && yy[15] < yy[16] &&  yy[16] < yy[17] &&  yy[17] < yy[18] &&  yy[18] < yy[19] ){
	
	 //if( yy[0] < yctr && yy[1] < yctr &&  yy[2] < yctr &&  yy[3] < yctr &&  yy[4] < yctr
	 // && yy[5] < yctr && yy[6] < yctr &&  yy[7] < yctr &&  yy[8] < yctr &&  yy[9] < yctr
	 // && yctr < yy[10] && yctr < yy[11] && yctr < yy[12] && yctr < yy[13] && yctr < yy[14]
	 // && yctr < yy[15] && yctr < yy[16] && yctr < yy[17] && yctr < yy[18] && yctr < yy[19] ){
       x_buf = ( xx[0] * yy[19] - xx[19] * yy[0] ) / ( yy[19] - yy[0] );
       xctr.push_back(x_buf);

		 for( int j = 0; j < 20; j++ ){
          xx[j] = 0.;
			 yy[j] = yctr;
		 }
	 }
 }

 return xctr;

}

vector<double> GetRiseSlope(TGraph *g, double yctr){

 double x, y;
 double nslope = 0.;

 double x1 = 0.;
 double x2 = 0.;
 double y1 = yctr;
 double y2 = yctr;

 vector<double> slope;

 int GATE = 0;

 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint(i, x, y);

	 if( y < yctr ){
       x1 = x;
		 y1 = y;
		 GATE = 1;
	 }
	 
	 if( y > yctr ){
       x2 = x;
		 y2 = y;
		 GATE = 2;
	 }

	 if( y1 < yctr && yctr < y2 && GATE == 2 ){
       nslope = ( y2 - y1 ) / ( x2 - x1 );
       slope.push_back(nslope);

		 x1 = 0.;
		 x2 = 0.;
		 y1 = yctr;
		 y2 = yctr;

		 GATE = 0;
	 }
 }

 return slope;

}

TGraph* MakeEyeDiagram(TGraph *g, double d, double a, int l){
 
 double x, y;
 int k = 0;

 TGraph *g_ = new TGraph();

 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint( i, x, y );
	 
	 for ( int j = 0; j < l; j++ ){
       if( d + a * ( j - 2 ) < x && x < d + a * ( j + 1 )){
          g_ -> SetPoint( k, x - ( d + a * ( j - 1 )), y );
			 k++;
		 }
	 }
 }

 //cout << "point number : " << k << endl;

 return g_;

}

TH2D* GraphtoHist(TGraph *g, TH2D *h){

 double x, y;

 for( int i = 0; i < g -> GetN(); i++){
    g -> GetPoint( i, x, y );
    h -> Fill( x, y );
 }
 
 return h;

}

TGraph* SumGraph(TGraph *g, TGraph *g_, int k){
 
 double x, y;

 for( int i = 0; i < g -> GetN(); i++){
    g  -> GetPoint( i, x, y );
	 g_ -> SetPoint( k, x, y );
	 k++;
 }

 return g_;

}

TLine* MakeMask(TLine *l, int i, double centx){

 if( i == 0 )l = new TLine( centx - 1.01e-9,  0. , centx - 0.37e-9,  0.1);
 if( i == 1 )l = new TLine( centx - 0.37e-9,  0.1, centx + 0.37e-9,  0.1);
 if( i == 2 )l = new TLine( centx + 0.37e-9,  0.1, centx + 1.01e-9,  0. );
 if( i == 3 )l = new TLine( centx + 1.01e-9,  0. , centx + 0.37e-9, -0.1);
 if( i == 4 )l = new TLine( centx + 0.37e-9, -0.1, centx - 0.37e-9, -0.1);
 if( i == 5 )l = new TLine( centx - 0.37e-9, -0.1, centx - 1.01e-9,  0. );

 l -> SetLineColor(kRed);
 l -> SetLineWidth(2);
 
 return l;

}

double GetLowOutPeak(TH1D *h){
//find some peak
//select lower peak

 TSpectrum *ts = new TSpectrum();
 ts -> Search( h );

 double spct;
 double lout = ts -> GetPositionX()[0];

 for( int i = 0; i < ts -> GetNPeaks(); i++){
    spct = ts -> GetPositionX()[i];
	 
	 if( spct < lout )lout = spct;
 }

 return lout;

}

double GetHighOutPeak(TH1D *h){
//find some peak
//select higher peak

 TSpectrum *ts = new TSpectrum();
 ts -> Search( h );

 double spct;
 double hout = ts -> GetPositionX()[0];

 for( int i = 0; i < ts -> GetNPeaks(); i++){
    spct = ts -> GetPositionX()[i];
	 
	 if( hout < spct )hout = spct;
 }

 return hout;

}

double GetGausLow(TH1D *h, double mean, double min, double max){
//set a gaussian
//fit gaus
//get 3 sigma lower point from gaus mean

 TF1 *f = new TF1("f", "gaus", min, max);
 f -> SetParameters( h -> GetBinContent( h -> FindBin( mean ) ), mean, 0.01 );
 f -> SetParLimits( 0, h -> GetBinContent( h -> FindBin( mean ) ) - 1, h -> GetBinContent( h -> FindBin( mean ) ) + 1 );
 f -> SetParLimits( 1, mean - 0.01, mean + 0.01 );

 h -> Fit( f, "QB" );

 double low = f -> GetParameter(1) - ( f -> GetParameter(2) ) * 3;

 return low;
 
}

double GetGausHigh(TH1D *h, double mean, double min, double max){
//set a gaussian
//fit gaus
//get 3 sigma higher point from gaus mean

 TF1 *f = new TF1("f", "gaus", min, max);
 f -> SetParameters( h -> GetBinContent( h -> FindBin( mean ) ), mean, 0.01 );
 f -> SetParLimits( 0, h -> GetBinContent( h -> FindBin( mean ) ) - 1, h -> GetBinContent( h -> FindBin( mean ) ) + 1 );
 f -> SetParLimits( 1, mean - 0.01, mean + 0.01 );

 h -> Fit( f, "QB" );

 double high = f -> GetParameter(1) + ( f -> GetParameter(2) ) * 3;

 return high;
 
}

double GetLow(TH1D *h, int bin){
//start at nothing point
//search lowest point there is somting

 double low = 0.;

 for( int i = bin; i > -1 ; i-- ){
    if( h -> GetBinContent( i ) != 0 ){
       low = h -> GetBinCenter( i );
	    break;
	 }
 }

 return low;

}

double GetHigh(TH1D *h, int bin){
//start at nothing point
//search highest point there is somting

 double high = 0.;

 for( int i = bin; i < h -> GetNbinsX(); i++ ){
    if( h -> GetBinContent( i ) != 0 ){
       high = h -> GetBinCenter( i );
	    break;
	 }
 }

 return high;

}

double GetError(double ave, vector<double> xx){
//get average error

 double dis, sqdis;
 double sum_sqdis = 0.;
 double err;

 for( int i = 0; i < int(xx.size()); i++ ){
    dis = ave - xx[i];
	 sqdis = dis * dis;
	 sum_sqdis += sqdis;
 }

 err = sqrt( sum_sqdis / ( xx.size() * ( xx.size() - 1 )));

 return err;

}

template < class TTemplate >
int SetAxisTitle( TTemplate* plot, string *xaxis = NULL, string *yaxis = NULL ){
	
 if(xaxis != NULL)plot -> GetXaxis() -> SetTitle(Form("%s", xaxis -> c_str()));
 if(yaxis != NULL)plot -> GetYaxis() -> SetTitle(Form("%s", yaxis -> c_str()));
 
 plot -> GetXaxis() -> CenterTitle();
 plot -> GetYaxis() -> CenterTitle();
 
 return 0;

}

TFile* ReCreateAppRoot(TString foname, string *app){

 foname.Append(Form("%s", app -> c_str()));

 TFile *foroot = TFile::Open(foname.Data(), "recreate");
 //TFile *foroot = TFile::Open(foname, "recreate");

 cout << "recreate new root file : " << foname << endl;

 return foroot;

}

//TGraph *GetRiseHighPoint(double hi){}

TGraph* SlideGraph(TGraph *g, double xx, double yy){

 double x, y;

 TGraph *g_ = new TGraph();

 for( int i = 0; i < g -> GetN(); i++){
    g  -> GetPoint( i, x, y );
	 g_ -> SetPoint( i, x - xx, y - yy );
 }

 return g_;

}

int CountBuf(const char *fname){
		  
 ifstream ifs(fname);
 string sbuf;
 
 int CNT = 0;

 while( ifs >> sbuf ){
    CNT++;
 }

 return CNT;

}

//double DFT(){}

double SinPoint(int NPOINT, int frequency, int point){

 double pi = acos(-1);
 double v_sin = sin( ( 2 * pi / NPOINT ) * frequency * point );

 if( -1e-10 < v_sin && v_sin < 1e-10 )v_sin = 0.;

 return v_sin;

}

double CosPoint(int NPOINT, int frequency, int point){

 double pi = acos(-1);
 double v_cos = cos( ( 2 * pi / NPOINT ) * frequency * point );

 if( -1e-10 < v_cos && v_cos < 1e-10 )v_cos = 0.;

 return v_cos;

}

TGraph* GetDFTAnGraph( TGraph *g, int NSAMPLE ){

 double an = 0.;
 TGraph *g_ = new TGraph();

 for( int i = 0; i < NSAMPLE; i++ ){
    an = 0.;
    for( int j = 0; j < g -> GetN(); j++ ){
       g -> GetPoint( j, tt, vv );
       an += vv * cos(( i + 1 ) * tt );
	 }

    g_ -> SetPoint( i, i + 1, an * 2. / ( g -> GetN()));
 }

 return g_;

}

TGraph* GetDFTBnGraph( TGraph *g, int NSAMPLE ){

 double bn = 0.;
 TGraph *g_ = new TGraph();

 for( int i = 0; i < NSAMPLE; i++ ){
    bn = 0.;
    for( int j = 0; j < g -> GetN(); j++ ){
       g -> GetPoint( j, tt, vv );
       bn += vv * sin(( i + 1 ) * tt );
	 }

    g_ -> SetPoint( i, i + 1, bn * 2. / ( g -> GetN()) );
 }

 return g_;

}

TGraph* GetDFTRSS( TGraph *g_an, TGraph *g_bn ){

 double an, bn;
 double rss;

 TGraph *g_ = new TGraph();

 for( int i = 0; i < g_an -> GetN(); i++ ){
    an = g_an -> GetPointY(i);
    bn = g_bn -> GetPointY(i);

    rss = sqrt( an * an + bn * bn );

	 g_ -> SetPoint( i, i + 1, rss );
 }

 return g_;

}

TGraph* ScallingGraph( TGraph *g, double min, double max ){

 TGraph *g_ = new TGraph();

 double x_i = g -> GetPointX(0);
 double x_f = g -> GetPointX( g -> GetN() - 1 );

 double x, x_, y;

 for( int i = 0; i < g -> GetN(); i++ ){
    g -> GetPoint( i, x, y );
	 
	 x_ = ( x - x_i ) * ( max - min ) / ( x_f - x_i ) + min;

	 g_ -> SetPoint( i, x_, y );
 }

 return g_;

}

double SumGraphPointX( TGraph *g ){

 double sum = 0.;
 
 for( int i = 0; i < g -> GetN(); i++ ){
    sum += g -> GetPointX(i);
 }

 return sum;

}

double SumGraphPointY( TGraph *g ){

 double sum = 0.;
 
 for( int i = 0; i < g -> GetN(); i++ ){
    sum += g -> GetPointY(i);
 }

 return sum;

}

template < class TTemplate >
int SetMarker( TTemplate* plot, double size, int style, Color_t color ){

 plot -> SetMarkerSize(size);
 plot -> SetMarkerStyle(style);
 plot -> SetMarkerColor(color);
 plot -> SetLineColor(color);

 return 0;

}

TGraph* ReDFTNCosGraph( TGraph *g, TGraph *g_an, double a0, int NPOINT ){

 double x;
 double y = a0 / 2.;
 double an;

 TGraph *g_ = new TGraph();

 for( int i = 0; i < g -> GetN(); i++ ){
    x = g -> GetPointX(i);
	 y = a0 / 2.;

    for( int j = 0; j < NPOINT; j++ ){
       an = g_an -> GetPointY(j); 

       y += an * cos(( j + 1 ) * x );
    }

	 g_ -> SetPoint( i, x, y );
 }

 return g_;

}

TGraph* ReDFTNGraph( TGraph *g, TGraph *g_an, TGraph *g_bn, double a0, int NPOINT ){

 double x;
 double y = a0 / 2.;
 double an, bn;

 TGraph *g_ = new TGraph();

 for( int i = 0; i < g -> GetN(); i++ ){
    x = g -> GetPointX(i);
	 y = a0 / 2.;

    for( int j = 0; j < NPOINT; j++ ){
       an = g_an -> GetPointY(j); 
       bn = g_bn -> GetPointY(j); 

       y += an * cos(( j + 1 ) * x ) + bn * sin(( j + 1 ) * x );
    }

	 g_ -> SetPoint( i, x, y );
 }

 return g_;

}

TGraph* ReDFTGraph( TGraph *g, TGraph *g_an, TGraph *g_bn, double a0 ){

 double x;
 double y = a0 / 2.;
 double an, bn;

 TGraph *g_ = new TGraph();

 for( int i = 0; i < g -> GetN(); i++ ){
    x = g -> GetPointX(i);
	 y = a0 / 2.;

    for( int j = 0; j < g_an -> GetN(); j++ ){
       an = g_an -> GetPointY(j); 
       bn = g_bn -> GetPointY(j); 

       y += an * cos(( j + 1 ) * x ) + bn * sin(( j + 1 ) * x );
    }

	 g_ -> SetPoint( i, x, y );
 }

 return g_;

}

TGraph* GetdBGraph( TGraph *g_ori_rss, TGraph *g_rss ){

 double y, y_ori;
 double y_;

 int N = 0;

 TGraph *g_ = new TGraph();

 for( int i = 0; i < g_rss -> GetN(); i++ ){
    y     = g_rss     -> GetPointY(i);
    y_ori = g_ori_rss -> GetPointY(i);

	 //if( y_ori < y ){
    //   //cout << i << " : " << y << " : " << y_ori << endl;
	 //   N++;
	 //}

	 if( y_ori != 0 && y != 0 )y_ = 20. * log10( y / y_ori );
	 //y_ = 20. * ( log10(y) - log10(y_ori));

	 else y_ = 0.;

	 g_ -> SetPoint( i, i + 1, y_ );
 }

 //cout << N << endl;

 return g_;

}

TGraphErrors* GetdBGraphErrors( TGraphErrors *g_ori_rss, TGraphErrors *g_rss ){

 double y, y_ori;
 double x;
 double y_error, y_ori_error;
 double x_error;
 double y_error_;
 double y_;

 int N = 0;

 TGraphErrors *g_ = new TGraphErrors();

 for( int i = 0; i < g_rss -> GetN(); i++ ){
    y           = g_rss     -> GetPointY(i);
    y_ori       = g_ori_rss -> GetPointY(i);
    y_error     = g_rss     -> GetErrorY(i);
    y_ori_error = g_ori_rss -> GetErrorY(i);
	 
    x       = g_rss -> GetPointX(i);
    x_error = g_rss -> GetErrorX(i);

	 if( y_ori < y ){
       //cout << i << " : " << y << " : " << y_ori << endl;
		 N++;
	 }

	 if( y != 0 && y_ori != 0 ){
	    y_ = 20. * log10( y / y_ori );
	    //y_ = 20. * ( log10(y) - log10(y_ori));

	    y_error_ = sqrt( 20. ) * ( y_error / fabs( y ) + y_ori_error / fabs( y_ori ) );
	 }

	 else{
       y_ = 0.;
		 y_error_ = 0.;
	 }

	 g_ -> SetPoint( i, x, y_ );
	 g_ -> SetPointError( i, x_error, y_error_ );
 }

 //cout << N << endl;

 return g_;

}

double GetGraphXAverage( TGraph *g ){

 double x;
 double sum = 0.;
 double ave;

 for( int i = 0; i < g -> GetN(); i++ ){
    x = g -> GetPointX(i);

	 sum += x;
 }
 ave = sum / double( g -> GetN());

 return ave;

}

double GetGraphXRMS( TGraph *g, double ave ){

 double x;
 double rs = 0.;
 double rms;

 for( int i = 0; i < g -> GetN(); i++ ){
    x = g -> GetPointX(i);

	 rs += ( ave - x ) * ( ave - x );
 }

 rms = sqrt( rs / double(( g -> GetN() - 1. ) * g -> GetN()));

 return rms;
 
}

double GetGraphYAverage( TGraph *g ){

 double y;
 double sum = 0.;
 double ave;

 for( int i = 0; i < g -> GetN(); i++ ){
    y = g -> GetPointY(i);

	 sum += y;
 }
 ave = sum / double( g -> GetN());

 return ave;

}

double GetGraphYRMS( TGraph *g, double ave ){

 double y;
 double rs = 0.;
 double rms;

 for( int i = 0; i < g -> GetN(); i++ ){
    y = g -> GetPointY(i);

	 rs += ( ave - y ) * ( ave - y );
 }

 rms = sqrt( rs / double(( g -> GetN() - 1. ) * g -> GetN()));

 return rms;
 
}

TGraphErrors *GetGraphNAverage( TGraph *g, int NAVE ){

 double xave;
 double xrms;
 double yave;
 double yrms;

 double x;
 double y;

 int NPOINT = 0;

 TGraph *g_ave = new TGraph();

 TGraphErrors *ge = new TGraphErrors();

 for( int i = 0; i < g -> GetN(); i++ ){
    x = g -> GetPointX(i);
    y = g -> GetPointY(i);

	 g_ave -> SetPoint( i%NAVE, x, y );

	 if( i%NAVE == NAVE - 1 ){
       xave = GetGraphXAverage( g_ave );
       xrms = GetGraphXRMS( g_ave, xave );

       yave = GetGraphYAverage( g_ave );
       yrms = GetGraphYRMS( g_ave, yave );

		 ge -> SetPoint( NPOINT, xave, yave );
		 ge -> SetPointError( NPOINT, xrms, yrms );
		 //cout << xave << endl;

		 NPOINT++;
	 }
 }

 return ge;

}

TGraph *MakeSampleWaveform( TGraph *g_uni, TGraph *g_level ){
 
 TGraph *g = new TGraph();

 vector<double> level;

 double hmin = VMinimum( g_uni );
 double hmax = VMaximum( g_uni );

 double tu_i = g_uni -> GetPointX(0);
 double tu_f = g_uni -> GetPointX( g_uni -> GetN() - 1 );

 double t_int = 0.;

 if( g_uni -> GetN() != 0 )t_int = ( tu_f - tu_i ) / double( g_uni -> GetN() - 1 );
 else return g_uni;

 double x, y;

 for( int i = 0; i < g_level -> GetN(); i++ ){
    y = g_level -> GetPointY(i);

	 if( y == 0 )level.push_back( hmin );
    else level.push_back( hmax );
 }

 for( int i = 0; i < g_uni -> GetN(); i++ ){
    x = g_uni -> GetPointX(i);
	 y = level[int( i / ( 5.3e-9 / t_int ) )];

	 g -> SetPoint( i, x, y );
 }

 return g;

}


 string xax = "frequency[MHz]";
 string xax_Hz = "frequency[Hz]";
 string yax = "coefficient";

 string yax2 = "[dB]";

 string axis_pow = "power spectrum (sqrt(an^2 + bn^2))";


#endif
