double Gaus_function_1 (double *x, double*par)
{
	double gaussian_eq  = par[4] * TMath::Exp( -1*( pow(x[0]-par[5],2)/(2*pow(par[6],2)) ) );
	return gaussian_eq;
}

double Gaus_function_2 (double *x, double*par)
{
	double gaussian_eq  = par[7] * TMath::Exp( -1*( pow(x[0]-par[8],2)/(2*pow(par[9],2)) ) );
	return gaussian_eq;
}

double Func_Langaus(double *x, double *par)
{
	// + Fit parameters:
	//------------------
	// * Signal part:
	// * - par[0]: Width (scale) parameter of Landau density
	// * - par[1]: Most Probable (MP, location) parameter of Landau density
	// * - par[2]: Total area (integral -inf to inf, normalization constant)
	// * - par[3]: Width (sigma) of convoluted Gaussian function
	
	//Note1: In the Landau distribution (represented by the CERNLIB approximation),
	//      the maximum is located at x=-0.22278298 with the location parameter=0.
	//      This shift is corrected within this function, so that the actual
	//      maximum is identical to the MP parameter.
	
	//Note2: In a convolution, the variable in the integral run from -inf to +inf.
	//       We can replace the infinity by a number whose magnitude large enough
	//       that beyond its value, the contribution to the convolution is neglectable
	
	// + Numeric constants
	//--------------------
	double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
	double mpshift  = -0.22278298;       // Landau maximum location
	
	// + Control constants
	//--------------------
	double nStep  = 100.0;    // number of convolution steps
	double extSig = 5.0;      // convolution extends to +- [extSig * Gaussian sigmas]
	
	// + Variables
	//------------
	double xx;            // Variable of integration
	double meanLand;      // "Mean" value of Landau distribution used in ROOT
	double fland;         // Landau in the convolution integral
	double fgaus;         // Gaussian in the convolution integral
	double sum = 0.0;     // The sum that replace the integral
	double xlow,xupp;     // Lowest and highest boundary of the integration
	double step;          // step size
	double signal;        // Value of the convolution Land x Gaus at x[0]
	double background;    // Value of the background at x[0]
	
	// + MP shift correction
	//----------------------
	meanLand = par[1] - mpshift * par[0];
	
	// + Range of convolution integral
	//--------------------------------
	xlow = x[0] - extSig * par[3];
	xupp = x[0] + extSig * par[3];
	
	step = (xupp-xlow) / nStep;
	
	// + Convolution at x[0]: integral of Landau and Gaussian by sum
	//--------------------------------------------------------------
	for(double i=1.0; i<=nStep/2; i++)
	{
		xx = xlow + (i-0.5) * step;
		fland = TMath::Landau(xx,meanLand,par[0]) / par[0];
		fgaus = TMath::Gaus(x[0],xx,par[3]);
		sum += fland * fgaus;
		
		xx = xupp - (i-0.5) * step;
		fland = TMath::Landau(xx,meanLand,par[0]) / par[0];
		fgaus = TMath::Gaus(x[0],xx,par[3]);
		sum += fland * fgaus;
	}
	signal = par[2] * step * sum * invsq2pi / par[3];
	
	return signal;
}



double Func_Langaus_comb(double *x, double *par)
{
    return Func_Langaus(x,par) + Gaus_function_1(x,par);
}

double Func_Langaus_comb_3(double *x, double *par)
{
    return Func_Langaus(x,par) + Gaus_function_1(x,par) + Gaus_function_2(x,par);
}

TString color_code [8]={"#343434","#1A3947","#566575","#797983","#EFBD9D","#FCA26E","#F5751D","#F5321D"};
TString color_code_2 [8]={"#CC768D","#19768D","#DDA573","#009193","#6E9193","#941100","#A08144","#517E66"};

void DAC_Scan_truth_comp ()
{
    pair<TString,double> Par_Land_width   = {"LandWidth     ", 4.5  };
    pair<TString,double> Par_MPV          = {"MPV           ", 93.8 };
    pair<TString,double> Par_size         = {"LandGaus_size ", 26407};
    pair<TString,double> Par_Gaus_width   = {"Gaus_width    ", 12   };
    pair<TString,double> Par_Gaus1_height = {"Gaus1_height  ", 800 };
    pair<TString,double> Par_Gaus1_mean   = {"Gaus1_mean    ", 0   }; 
    pair<TString,double> Par_Gaus1_width  = {"Gaus1_width   ", 10  };
    pair<TString,double> Par_Gaus2_height = {"Gaus2_height  ", 100 };
    pair<TString,double> Par_Gaus2_mean   = {"Gaus2_mean    ", 0   }; 
    pair<TString,double> Par_Gaus2_width  = {"Gaus2_width   ", 30  };

    TF1 * test_TF1_2 = new TF1 ("",Func_Langaus_comb_3,-0,250,10);
    test_TF1_2 -> SetLineColor(4);
    test_TF1_2 -> SetNpx(10000);

    test_TF1_2 -> SetParNames(
            Par_Land_width.first, 
            Par_MPV.first, 
            Par_size.first, 
            Par_Gaus_width.first, 

            Par_Gaus1_height.first, 
            Par_Gaus1_mean.first, 
            Par_Gaus1_width.first,

            Par_Gaus2_height.first, 
            Par_Gaus2_mean.first, 
            Par_Gaus2_width.first
        );
    test_TF1_2 -> SetParameters(
        Par_Land_width.second, 
        Par_MPV.second, 
        Par_size.second, 
        Par_Gaus_width.second, 
        
        Par_Gaus1_height.second, 
        Par_Gaus1_mean.second, 
        Par_Gaus1_width.second,

        Par_Gaus2_height.second, 
        Par_Gaus2_mean.second, 
        Par_Gaus2_width.second
    );

    gRandom->SetSeed(100);

    TH1F * true_hist = new TH1F("","",49,0,196);
    true_hist -> SetFillColor(TColor::GetColor(color_code_2[6]));
    true_hist -> SetLineColor(TColor::GetColor(color_code_2[6]));
    true_hist -> SetStats(0);
    true_hist -> SetTitle("DAC Scan, truth comparison");
    true_hist -> GetXaxis() -> SetTitle("ADC");
    true_hist -> GetYaxis() -> SetTitle("A.U.");

    for (int i = 0; i < 2000000; i++){
        true_hist -> Fill(test_TF1_2 -> GetRandom());
    }

    true_hist -> Scale(1./ true_hist -> Integral(18, 24) );


    TH1F * DAC_hist = new TH1F("","",49,0,196);
    DAC_hist -> SetLineColor(TColor::GetColor(color_code[1]));
    DAC_hist -> SetMarkerColor(TColor::GetColor(color_code[1]));
    DAC_hist -> SetMarkerStyle(20);
    DAC_hist -> SetMarkerSize(0.8);
    DAC_hist -> SetLineWidth(2);


    cout<<"DAC_hist_comb config confirmation, Bin width : "<<DAC_hist -> GetBinWidth(2)<<" range : "<<DAC_hist -> GetXaxis() -> GetXmin()<<" to "<<DAC_hist -> GetXaxis() -> GetXmax()<<endl;

    int used_BKG = 2;

    // note : event : uniform 2M to 5M
    if (used_BKG == 1)
    {    
        DAC_hist -> SetBinContent(1, 0); DAC_hist -> SetBinError(1, 0);
        DAC_hist -> SetBinContent(2, 0); DAC_hist -> SetBinError(2, 0);
        DAC_hist -> SetBinContent(3, 0.0687131); DAC_hist -> SetBinError(3, 0.000180071);
        DAC_hist -> SetBinContent(4, 0.0427734); DAC_hist -> SetBinError(4, 0.000142073);
        DAC_hist -> SetBinContent(5, 0.0226605); DAC_hist -> SetBinError(5, 0.000103409);
        DAC_hist -> SetBinContent(6, 0.0103689); DAC_hist -> SetBinError(6, 6.99507e-05);
        DAC_hist -> SetBinContent(7, 0.00399446); DAC_hist -> SetBinError(7, 4.34164e-05);
        DAC_hist -> SetBinContent(8, 0.00131761); DAC_hist -> SetBinError(8, 2.49355e-05);
        DAC_hist -> SetBinContent(9, 0.000379954); DAC_hist -> SetBinError(9, 1.33903e-05);
        DAC_hist -> SetBinContent(10, 9.72044e-05); DAC_hist -> SetBinError(10, 6.77279e-06);
        DAC_hist -> SetBinContent(11, 3.17084e-05); DAC_hist -> SetBinError(11, 3.86823e-06);
        DAC_hist -> SetBinContent(12, 6.13376e-05); DAC_hist -> SetBinError(12, 5.38007e-06);
        DAC_hist -> SetBinContent(13, 0.00015861); DAC_hist -> SetBinError(13, 8.65147e-06);
        DAC_hist -> SetBinContent(14, 0.000453349); DAC_hist -> SetBinError(14, 1.46265e-05);
        DAC_hist -> SetBinContent(15, 0.00113916); DAC_hist -> SetBinError(15, 2.31855e-05);
        DAC_hist -> SetBinContent(16, 0.00264386); DAC_hist -> SetBinError(16, 3.53219e-05);
        DAC_hist -> SetBinContent(17, 0.00545918); DAC_hist -> SetBinError(17, 5.07561e-05);
        DAC_hist -> SetBinContent(18, 0.010473); DAC_hist -> SetBinError(18, 7.03006e-05);
        DAC_hist -> SetBinContent(19, 0.0180913); DAC_hist -> SetBinError(19, 9.23974e-05);
        DAC_hist -> SetBinContent(20, 0.02882); DAC_hist -> SetBinError(20, 0.00011662);
        DAC_hist -> SetBinContent(21, 0.0414953); DAC_hist -> SetBinError(21, 0.000139934);
        DAC_hist -> SetBinContent(22, 0.0549312); DAC_hist -> SetBinError(22, 0.000161003);
        DAC_hist -> SetBinContent(23, 0.0669512); DAC_hist -> SetBinError(23, 0.000177748);
        DAC_hist -> SetBinContent(24, 0.075304); DAC_hist -> SetBinError(24, 0.00018851);
        DAC_hist -> SetBinContent(25, 0.0785887); DAC_hist -> SetBinError(25, 0.000192577);
        DAC_hist -> SetBinContent(26, 0.0766445); DAC_hist -> SetBinError(26, 0.00019018);
        DAC_hist -> SetBinContent(27, 0.0702214); DAC_hist -> SetBinError(27, 0.000182037);
        DAC_hist -> SetBinContent(28, 0.0611425); DAC_hist -> SetBinError(28, 0.000169862);
        DAC_hist -> SetBinContent(29, 0.0509732); DAC_hist -> SetBinError(29, 0.000155094);
        DAC_hist -> SetBinContent(30, 0.0412898); DAC_hist -> SetBinError(30, 0.000139587);
        DAC_hist -> SetBinContent(31, 0.0329312); DAC_hist -> SetBinError(31, 0.00012466);
        DAC_hist -> SetBinContent(32, 0.0258716); DAC_hist -> SetBinError(32, 0.000110493);
        DAC_hist -> SetBinContent(33, 0.0204118); DAC_hist -> SetBinError(33, 9.81444e-05);
        DAC_hist -> SetBinContent(34, 0.0163051); DAC_hist -> SetBinError(34, 8.77176e-05);
        DAC_hist -> SetBinContent(35, 0.0131988); DAC_hist -> SetBinError(35, 7.8921e-05);
        DAC_hist -> SetBinContent(36, 0.0108078); DAC_hist -> SetBinError(36, 7.14155e-05);
        DAC_hist -> SetBinContent(37, 0.00899924); DAC_hist -> SetBinError(37, 6.5167e-05);
        DAC_hist -> SetBinContent(38, 0.0076457); DAC_hist -> SetBinError(38, 6.00666e-05);
        DAC_hist -> SetBinContent(39, 0.00648083); DAC_hist -> SetBinError(39, 5.53019e-05);
        DAC_hist -> SetBinContent(40, 0.00557543); DAC_hist -> SetBinError(40, 5.12937e-05);
        DAC_hist -> SetBinContent(41, 0.00494379); DAC_hist -> SetBinError(41, 4.83008e-05);
        DAC_hist -> SetBinContent(42, 0.00433092); DAC_hist -> SetBinError(42, 4.5208e-05);
        DAC_hist -> SetBinContent(43, 0.00389641); DAC_hist -> SetBinError(43, 4.28802e-05);
        DAC_hist -> SetBinContent(44, 0.00342283); DAC_hist -> SetBinError(44, 4.019e-05);
        DAC_hist -> SetBinContent(45, 0); DAC_hist -> SetBinError(45, 0);
        DAC_hist -> SetBinContent(46, 0); DAC_hist -> SetBinError(46, 0);
        DAC_hist -> SetBinContent(47, 0); DAC_hist -> SetBinError(47, 0);
        DAC_hist -> SetBinContent(48, 0); DAC_hist -> SetBinError(48, 0);
        DAC_hist -> SetBinContent(49, 0); DAC_hist -> SetBinError(49, 0);
    }
    else if (used_BKG == 2)
    {
        DAC_hist -> SetBinContent(1, 0); DAC_hist -> SetBinError(1, 0);
        DAC_hist -> SetBinContent(2, 0); DAC_hist -> SetBinError(2, 0);
        DAC_hist -> SetBinContent(3, 0.0720854); DAC_hist -> SetBinError(3, 0.000292775);
        DAC_hist -> SetBinContent(4, 0.0487332); DAC_hist -> SetBinError(4, 0.000240726);
        DAC_hist -> SetBinContent(5, 0.0304134); DAC_hist -> SetBinError(5, 0.00019017);
        DAC_hist -> SetBinContent(6, 0.0186117); DAC_hist -> SetBinError(6, 0.000148766);
        DAC_hist -> SetBinContent(7, 0.0120393); DAC_hist -> SetBinError(7, 0.000119649);
        DAC_hist -> SetBinContent(8, 0.0086429); DAC_hist -> SetBinError(8, 0.000101377);
        DAC_hist -> SetBinContent(9, 0.00693054); DAC_hist -> SetBinError(9, 9.07807e-05);
        DAC_hist -> SetBinContent(10, 0.00559238); DAC_hist -> SetBinError(10, 8.15471e-05);
        DAC_hist -> SetBinContent(11, 0.00463353); DAC_hist -> SetBinError(11, 7.42277e-05);
        DAC_hist -> SetBinContent(12, 0.00398066); DAC_hist -> SetBinError(12, 6.87999e-05);
        DAC_hist -> SetBinContent(13, 0.00326595); DAC_hist -> SetBinError(13, 6.23182e-05);
        DAC_hist -> SetBinContent(14, 0.00293222); DAC_hist -> SetBinError(14, 5.90484e-05);
        DAC_hist -> SetBinContent(15, 0.00311609); DAC_hist -> SetBinError(15, 6.08716e-05);
        DAC_hist -> SetBinContent(16, 0.00404649); DAC_hist -> SetBinError(16, 6.93664e-05);
        DAC_hist -> SetBinContent(17, 0.00634011); DAC_hist -> SetBinError(17, 8.68278e-05);
        DAC_hist -> SetBinContent(18, 0.0103518); DAC_hist -> SetBinError(18, 0.000110948);
        DAC_hist -> SetBinContent(19, 0.0172111); DAC_hist -> SetBinError(19, 0.000143059);
        DAC_hist -> SetBinContent(20, 0.0266889); DAC_hist -> SetBinError(20, 0.000178146);
        DAC_hist -> SetBinContent(21, 0.0383527); DAC_hist -> SetBinError(21, 0.000213554);
        DAC_hist -> SetBinContent(22, 0.050466); DAC_hist -> SetBinError(22, 0.000244968);
        DAC_hist -> SetBinContent(23, 0.0614232); DAC_hist -> SetBinError(23, 0.000270257);
        DAC_hist -> SetBinContent(24, 0.0689164); DAC_hist -> SetBinError(24, 0.000286267);
        DAC_hist -> SetBinContent(25, 0.0714569); DAC_hist -> SetBinError(25, 0.000291496);
        DAC_hist -> SetBinContent(26, 0.0700918); DAC_hist -> SetBinError(26, 0.000288698);
        DAC_hist -> SetBinContent(27, 0.0639061); DAC_hist -> SetBinError(27, 0.000275665);
        DAC_hist -> SetBinContent(28, 0.0557184); DAC_hist -> SetBinError(28, 0.000257401);
        DAC_hist -> SetBinContent(29, 0.0467123); DAC_hist -> SetBinError(29, 0.000235682);
        DAC_hist -> SetBinContent(30, 0.0376045); DAC_hist -> SetBinError(30, 0.000211461);
        DAC_hist -> SetBinContent(31, 0.0299268); DAC_hist -> SetBinError(31, 0.000188643);
        DAC_hist -> SetBinContent(32, 0.0236121); DAC_hist -> SetBinError(32, 0.000167563);
        DAC_hist -> SetBinContent(33, 0.0185009); DAC_hist -> SetBinError(33, 0.000148322);
        DAC_hist -> SetBinContent(34, 0.0147767); DAC_hist -> SetBinError(34, 0.000132556);
        DAC_hist -> SetBinContent(35, 0.0119576); DAC_hist -> SetBinError(35, 0.000119243);
        DAC_hist -> SetBinContent(36, 0.00987638); DAC_hist -> SetBinError(36, 0.00010837);
        DAC_hist -> SetBinContent(37, 0.00814231); DAC_hist -> SetBinError(37, 9.83975e-05);
        DAC_hist -> SetBinContent(38, 0.00695653); DAC_hist -> SetBinError(38, 9.09508e-05);
        DAC_hist -> SetBinContent(39, 0.00583752); DAC_hist -> SetBinError(39, 8.33152e-05);
        DAC_hist -> SetBinContent(40, 0.00509997); DAC_hist -> SetBinError(40, 7.78742e-05);
        DAC_hist -> SetBinContent(41, 0.00449971); DAC_hist -> SetBinError(41, 7.3148e-05);
        DAC_hist -> SetBinContent(42, 0.00393806); DAC_hist -> SetBinError(42, 6.84308e-05);
        DAC_hist -> SetBinContent(43, 0.00352865); DAC_hist -> SetBinError(43, 6.47761e-05);
        DAC_hist -> SetBinContent(44, 0.00308282); DAC_hist -> SetBinError(44, 6.05458e-05);
        DAC_hist -> SetBinContent(45, 0); DAC_hist -> SetBinError(45, 0);
        DAC_hist -> SetBinContent(46, 0); DAC_hist -> SetBinError(46, 0);
        DAC_hist -> SetBinContent(47, 0); DAC_hist -> SetBinError(47, 0);
        DAC_hist -> SetBinContent(48, 0); DAC_hist -> SetBinError(48, 0);
        DAC_hist -> SetBinContent(49, 0); DAC_hist -> SetBinError(49, 0);
    }
    


    TLegend * legend = new TLegend (0.5, 0.85, 0.9, 0.90);
    legend -> SetNColumns(2);
    legend -> SetTextSize (0.028);
        
    legend -> AddEntry (true_hist, "Truth", "f");
    legend -> AddEntry (DAC_hist, "DAC Scan", "ep");

    cout<<"Integral test : "<<DAC_hist -> Integral(18, 24)<<endl;
    DAC_hist -> Scale(1./ DAC_hist -> Integral(18, 24) );

    TCanvas * c_bin = new TCanvas("c_bin","c_bin",800,800);

    true_hist -> Draw("hist");
    DAC_hist -> Draw("ep same");
    legend -> Draw("same");

    c_bin -> Print("test_file/output_randevent_2BKG/DACScan_truth_comp.pdf");

}