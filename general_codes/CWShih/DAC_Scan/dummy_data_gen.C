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

// double Func_Langaus(double *x, double *par)
// {
// 	// + Fit parameters:
// 	//------------------
// 	// * Signal part:
// 	// * - par[0]: Width (scale) parameter of Landau density
// 	// * - par[1]: Most Probable (MP, location) parameter of Landau density
// 	// * - par[2]: Total area (integral -inf to inf, normalization constant)
// 	// * - par[3]: Width (sigma) of convoluted Gaussian function

// 	// par 4 : size 
// 	// par 5 : width
// 	// par 6 : scale
	
// 	//Note1: In the Landau distribution (represented by the CERNLIB approximation),
// 	//      the maximum is located at x=-0.22278298 with the location parameter=0.
// 	//      This shift is corrected within this function, so that the actual
// 	//      maximum is identical to the MP parameter.
	
// 	//Note2: In a convolution, the variable in the integral run from -inf to +inf.
// 	//       We can replace the infinity by a number whose magnitude large enough
// 	//       that beyond its value, the contribution to the convolution is neglectable
	
// 	// + Numeric constants
// 	//--------------------
// 	double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
// 	double mpshift  = -0.22278298;       // Landau maximum location
	
// 	// + Control constants
// 	//--------------------
// 	double nStep  = 100.0;    // number of convolution steps
// 	double extSig = 5.0;      // convolution extends to +- [extSig * Gaussian sigmas]
	
// 	// + Variables
// 	//------------
// 	double xx;            // Variable of integration
// 	double meanLand;      // "Mean" value of Landau distribution used in ROOT
// 	double fland;         // Landau in the convolution integral
// 	double fgaus;         // Gaussian in the convolution integral
// 	double sum = 0.0;     // The sum that replace the integral
// 	double xlow,xupp;     // Lowest and highest boundary of the integration
// 	double step;          // step size
// 	double signal;        // Value of the convolution Land x Gaus at x[0]
// 	double background;    // Value of the background at x[0]
	
// 	// + MP shift correction
// 	//----------------------
// 	meanLand = par[1] - mpshift * par[0];
	
// 	// + Range of convolution integral
// 	//--------------------------------
// 	xlow = x[0] - extSig * par[3];
// 	xupp = x[0] + extSig * par[3];
	
// 	step = (xupp-xlow) / nStep;
	
// 	// + Convolution at x[0]: integral of Landau and Gaussian by sum
// 	//--------------------------------------------------------------
// 	for(double i=1.0; i<=nStep/2; i++)
// 	{
// 		xx = xlow + (i-0.5) * step;
// 		fland = TMath::Landau(xx,meanLand,par[0]) / par[0];
// 		fgaus = TMath::Gaus(x[0],xx,par[3]);
// 		sum += fland * fgaus;
		
// 		xx = xupp - (i-0.5) * step;
// 		fland = TMath::Landau(xx,meanLand,par[0]) / par[0];
// 		fgaus = TMath::Gaus(x[0],xx,par[3]);
// 		sum += fland * fgaus;
// 	}
// 	signal = par[2] * step * sum * invsq2pi / par[3];

// 	double gaussian_eq  = par[4]*( 1/( par[5]* sqrt(2*TMath::Pi()) ) ) * TMath::Exp( -1*( pow(x[0]-par[6],2)/(2*pow(par[5],2)) ) );
	
// 	return signal;
// }

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

void dummy_data_gen()
{
    TH1F * hist = new TH1F("","",50,0,200);
    cout<< "bin width : "<< hist -> GetBinWidth(2) <<endl;


    int adc_setting_run[8][9] =
	{	
		{8   ,12 ,16 ,20 ,24 ,28 ,32 ,36 ,40},
		{28  ,32 ,36 ,40 ,44 ,48 ,52 ,56 ,60},
		{48  ,52 ,56 ,60 ,64 ,68 ,72 ,76 ,80},
		{68  ,72 ,76 ,80 ,84 ,88 ,92 ,96 ,100},
		{88  ,92 ,96 ,100,104,108,112,116,120},
		{108 ,112,116,120,124,128,132,136,140},
		{128 ,132,136,140,144,148,152,156,160},
		{148 ,152,156,160,164,168,172,176,180}
	};

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

    int chip;
    int adc;
    TFile * DACScan_test[8];

    TTree * DACScan_tree[8];

    TRandom * rand_chip = new TRandom3();

    for (int i = 0; i < 100000; i++){
        hist -> Fill(test_TF1_2 -> GetRandom ());
    }

    TRandom * rand_event = new TRandom3();
    gRandom->SetSeed(200);

    for (int i = 0; i < 8; i++) // note : run
    {
        DACScan_test[i] = new TFile(Form("test_file/DACScan_test_twoBKG_randevent_%i.root",i),"RECREATE");
        DACScan_test[i] -> cd();


        DACScan_tree[i] = new TTree("tree","tree");
        DACScan_tree[i] -> Branch("chip_id",&chip);
        DACScan_tree[i] -> Branch("adc",&adc);

        int event = int(rand_event -> Uniform(500000, 3000000));

        cout<<i<<" "<<event<<endl;

        for (int i1 = 0; i1 < event; i1++)
        {
            
            chip = int(rand_chip -> Uniform(0,26)) + 1;

            if ( chip == 10 ) continue;

            int ev_DAC_convert = test_TF1_2 -> GetRandom();

            if (ev_DAC_convert < adc_setting_run[i][0]) {continue;}
            else if (ev_DAC_convert >= adc_setting_run[i][0] && ev_DAC_convert < adc_setting_run[i][1]){adc=0;}
            else if (ev_DAC_convert >= adc_setting_run[i][1] && ev_DAC_convert < adc_setting_run[i][2]){adc=1;}
            else if (ev_DAC_convert >= adc_setting_run[i][2] && ev_DAC_convert < adc_setting_run[i][3]){adc=2;}
            else if (ev_DAC_convert >= adc_setting_run[i][3] && ev_DAC_convert < adc_setting_run[i][4]){adc=3;}
            else if (ev_DAC_convert >= adc_setting_run[i][4] && ev_DAC_convert < adc_setting_run[i][5]){adc=4;}
            else if (ev_DAC_convert >= adc_setting_run[i][5] && ev_DAC_convert < adc_setting_run[i][6]){adc=5;}
            else if (ev_DAC_convert >= adc_setting_run[i][6] && ev_DAC_convert < adc_setting_run[i][7]){adc=6;}
            else{adc=7;}

            DACScan_tree[i] -> Fill();

        }

        DACScan_test[i] -> cd();
        DACScan_tree[i] -> Write("", TObject::kOverwrite);
        
        DACScan_test[i] -> Close();

    }



    hist -> Fit(test_TF1_2,"N");

    hist -> Draw("hist");
    // // pure_Gaus -> Draw();
    // // func_fit -> Draw("same");

    // // comb_TF1 -> Draw();
    test_TF1_2 -> Draw("same");

    return;
}

    // TF1 *func_fit = new TF1("func_fit", Func_Langaus, -50, 250,4);
	// // func_fit -> SetParNames   ("widthSignal", "MPVSignal", "AreaSignal", "sigmaSignal");
	// func_fit -> SetParameters (4.5, 73.8,  26407, 12);
    // func_fit -> SetNpx(10000);

    // TF1 * pure_Gaus = new TF1 ("pure_Gaus",Gaus_function,-50,250,3);
    // pure_Gaus -> SetParameters(800,0,8);
    // pure_Gaus -> SetNpx(10000);

    // TF1NormSum * fit_comb = new TF1NormSum(func_fit,pure_Gaus);

    // TF1 * comb_TF1 = new TF1("comb_TF1",fit_comb,-50,250,fit_comb->GetNpar());
    // cout<<"N par : "<<fit_comb->GetNpar()<<endl;
    // comb_TF1 -> SetParameters(1,1,4.5, 73.8,  26407, 12,800,0,8);
    // comb_TF1 -> SetLineColor(4);
    // comb_TF1 -> SetNpx(10000);

    // TF1 * test_TF1 = new TF1 ("",Func_Langaus_comb,-50,250,7);
    // test_TF1 -> SetParameters(4.5, 73.8,  26407, 12,800,0,15);
    // test_TF1 -> SetLineColor(4);
    // test_TF1 -> SetNpx(10000);