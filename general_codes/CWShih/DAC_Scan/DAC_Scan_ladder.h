#ifndef LadderDAC_h
#define LadderDAC_h

TString color_code [8]={"#343434","#1A3947","#566575","#797983","#EFBD9D","#FCA26E","#F5751D","#F5321D"};
TString color_code_2 [8]={"#CC768D","#19768D","#DDA573","#009193","#6E9193","#941100","#A08144","#517E66"};

struct map_format{
    int module_id;
    TString port;
    TString ladder;
};

map<TString,int> port_map = {
    {"A1", 0}, {"A2", 1}, {"A3", 2}, 
    {"B1", 3}, {"B2", 4}, {"B3", 5}, 
    {"C1", 6}, {"C2", 7}, {"C3", 8}, 
    {"D1", 9}, {"D2", 10}, {"D3", 11}
};

// note : the reason not to start with "0" is because that the return value would be 0 if the match not found 
map<TString,int> layer_map = {
    {"B0L0",1},
    {"B0L1",2},
    {"B1L0",3},
    {"B1L1",4}
};

double Func_gaus_draw(double *x, double*par) // note : [0 1 2], for drawing purpose only
{
	double gaussian_eq  = par[0] * TMath::Exp( -1*( pow(x[0]-par[1],2)/(2*pow(par[2],2)) ) );
	return gaussian_eq;
}

double Func_exp_draw(double *x, double*par) // note : [0 1 2], for drawing purpose only
{
	double exp_eq = TMath::Exp(-1 * (par[0] * x[0] + par[1])) + par[2];
	return exp_eq;
}

double Func_gaus_1(double *x, double*par) // note : [4 5 6]
{
	double gaussian_eq  = par[4] * TMath::Exp( -1*( pow(x[0]-par[5],2)/(2*pow(par[6],2)) ) );
	return gaussian_eq;
}

double Func_gaus_2(double *x, double*par) // note : [7 8 9]
{
	double gaussian_eq  = par[7] * TMath::Exp( -1*( pow(x[0]-par[8],2)/(2*pow(par[9],2)) ) );
	return gaussian_eq;
}

double Func_exp_1(double *x, double*par) // note : [4 5 6]
{
	double exp_eq = TMath::Exp(-1 * (par[4] * x[0] + par[5])) + par[6];
	return exp_eq;
}

// double Func_exp_2(double *x, double*par) // note : [7 8 9]
// {
// 	double exp_eq = TMath::Exp(-1 * (par[7] * x[0] + par[8])) + par[9];
// 	return exp_eq;
// }


double Func_Langaus(double *x, double *par) // note : [0 to 3]
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

double Func_Sig_Gaus(double *x, double *par) // note : 4 + 3 parameters
{
    return Func_Langaus(x,par) + Func_gaus_1(x,par);
}

double Func_Sig_Gaus_Gaus(double *x, double *par) // note : 4 + 3 + 3 parameters
{
    return Func_Langaus(x,par) + Func_gaus_1(x,par) + Func_gaus_2(x,par);
}


double Func_Sig_Exp(double *x, double *par) // note : 4 + 3 parameters
{
    return Func_Langaus(x,par) + Func_exp_1(x,par);
}

double Func_Sig_Exp_Gaus(double *x, double *par) // note : 4 + 3 + 3 parameters
{
    return Func_Langaus(x,par) + Func_exp_1(x,par) + Func_gaus_2(x,par);
}

class LadderDAC 
{
    public :
        LadderDAC(TString ana_level, TString ladder_name, int ROC_index, TString ladder_port, int Felix_ch, vector<vector<int>> Scan_config);

        void Fill(int run_id, int chip_id, int chan_id, int adc);
        void Weight();
        void Fill_final();
        void Fit();
        void Spit_BinContent_All(); // note : the function for testing 
        void Output_bin_plots(TString plot_folder_dire, TString set_name, bool bin8_set_zero = false); // note : because it will be multi-run, so file_name doesn't work, let's try set_name
        void Output_comb_plots(TString plot_folder_dire, TString set_name);
        void Output_final_plots(TString plot_folder_dire, TString set_name, int fit_opt = - 1); // note : final
        void Output_root(TString plot_folder_dire, TString set_name);

    private : 
        // double Func_gaus_draw(double *x, double*par); // note : assigned par[0 ~2], for drawing only
        // double Func_gaus_1(double *x, double*par); // note : assigned par[4 ~6]
        // double Func_gaus_2(double *x, double*par); // note : assigned par[7 ~9]
        // double Func_Langaus(double *x, double *par);

        // double Func_Sig_Gaus(double *x, double *par); // note : combine Func_Langaus [0~3] + Func_gaus_1[4~6]
        // double Func_Sig_Gaus_Gaus(double *x, double *par); // note : combine Func_Langaus [0~3] + Func_gaus_1[4~6] + Func_gaus_2[7~9]  
        void empty_check();
        int bin_convert(int run_id, int bin);

        TString ana_level;
        TString ladder_name;
        int ROC_index;
        TString ladder_port;
        int Felix_ch;
        vector<vector<int>> Scan_config;
        // note : adc 7, overflow
        // note : adc 5 and adc 6, overlap region
        // note : Scan_config[0] = {4 , 8, 12, 16, 20,,,,,}, the first file
        // note : Scan_config[1] = {20 , 24, 28, 32, 36,,,,,}, the second file

        int run_number;

        // note : the number of run is not fixed.
        // note : the number of runs can be obtained by the "Scan_config.size()"
        vector< vector< TH1F * > > DAC_hist_bin ; // note : it will be 8 histograms shown in the same TCanvas 
        vector< vector< TH1F * > > DAC_hist_comb; // note : it will be multiple histograms, weighted and use draw "same"
        vector< TH1F * >           DAC_hist_all ; // note : the final distribution with fitting
        vector< int >              unit_entry_array;
        vector< vector < TF1 * > > Fit_vec;  // note : Fit_vec[check_number, chip + all][number of fit trial for each]
        vector< TF1 * >            Fit_gaus_1;
        vector< TF1 * >            Fit_gaus_2;
        vector< TF1 * >            Fit_landgaus; 
        // note : 2 -> fitting combines 1 signal + 1 gaus background 
        // note : 3 -> fitting combines 1 signal + 2 guas backgrounds
        vector<int>                fit_tag;
        int empty_criterion = 1000; // todo : the empty criteria is here
        
         
};

// note : some assumption 
// note : assume the increments are all the same for all the runs

// note : input, single hit cluster
LadderDAC::LadderDAC(TString ana_level, TString ladder_name, int ROC_index, TString ladder_port, int Felix_ch, vector<vector<int>> Scan_config)
:ana_level(ana_level),
ladder_name(ladder_name),
ROC_index(ROC_index),
ladder_port(ladder_port),
Felix_ch(Felix_ch),
Scan_config(Scan_config)
{
    cout<<"Initialization start"<<endl;
    
    run_number = Scan_config.size();

    // note : the number of run
    vector< TH1F* > run_unit(run_number, NULL);
    
    // note : fit-0 -> sig + bg-gaus-1 + bg-gaus-2
    // note : fit-1 -> sig + bg-gaus-1
    // note : fit-2 -> sig + bg-gaus-1 + bg-gaus-2, mean of bg-gaus zero
    // note : fit-3 -> sig + bg-gaus-1, mean of bg-gaus zero
    vector< TF1 * > fit_unit(6, NULL); // todo : the number of fitting for each unit is here, now 4

    if (ana_level == "chip") // note : 26 chips + 1 total in the half-ladder
    {
        int check_number = 27; // note : 0 ~ 25 chip, 26 full half-ladder
        int increment = Scan_config[0][1] - Scan_config[0][0];
        int Scan_limit = Scan_config.back().back();

        DAC_hist_bin.assign(check_number,run_unit);
        DAC_hist_comb.assign(check_number,run_unit);
        DAC_hist_all.assign(check_number,NULL);
        
        Fit_vec.assign(check_number,fit_unit);
        Fit_gaus_1.assign(check_number,NULL);
        Fit_gaus_2.assign(check_number,NULL);
        Fit_landgaus.assign(check_number,NULL);
        fit_tag.assign(check_number,-1); // note : -1 -> means no fitting

        unit_entry_array.assign(check_number,0);

        for (int i = 0; i < check_number; i++) // note : 26 chips + 1 total in the half-ladder
        {
            for (int i1 = 0; i1 < run_number; i1++) // note : number of files (number of runs)
            {
                DAC_hist_bin[i][i1] = new TH1F("","",8,0,8); // todo : change here if you want to only show 7 bins
                DAC_hist_bin[i][i1] -> GetXaxis() -> SetTitle("ADC");
                DAC_hist_bin[i][i1] -> GetYaxis() -> SetTitle("Entry");
                DAC_hist_bin[i][i1] -> SetLineColor( TColor::GetColor(Form("%s",color_code[1].Data())) );
                DAC_hist_bin[i][i1] -> SetLineWidth(3);

                DAC_hist_comb[i][i1] = new TH1F("","", Scan_limit/increment + 5, 0, Scan_limit + increment * 5);
                DAC_hist_comb[i][i1] -> GetXaxis() -> SetTitle("DAC value");
                DAC_hist_comb[i][i1] -> GetYaxis() -> SetTitle("A.U.");
                DAC_hist_comb[i][i1] -> SetLineColor( TColor::GetColor(Form("%s",color_code_2[i1].Data())) );
                DAC_hist_comb[i][i1] -> SetLineWidth(3);
                DAC_hist_comb[i][i1] -> GetYaxis()->SetRangeUser(0,1000); // todo : here just follows what we had in "DAC_scan.C"
                DAC_hist_comb[i][i1] -> SetStats(0);

                if (i == (check_number - 1)) // note : all combined, TH1F[26]
                {
                    DAC_hist_bin[i][i1] -> SetTitle(Form("Chip level, all chips, run-%i",i1));
                    DAC_hist_comb[i][i1] -> SetTitle(Form("Chip level, all chips, run-%i",i1));
                }
                else // note : chip level [0 ~ 25] -> 1 to 26
                {
                    DAC_hist_bin[i][i1] -> SetTitle(Form("Chip level, U%i, run-%i",i+1,i1));
                    DAC_hist_comb[i][i1] -> SetTitle(Form("Chip level, U%i, run-%i",i+1,i1));
                }
                
            }

            DAC_hist_all[i] = new TH1F("","",Scan_limit/increment + 5, 0, Scan_limit + increment * 5);
            DAC_hist_all[i] -> GetXaxis() -> SetTitle("DAC value");
            DAC_hist_all[i] -> GetYaxis() -> SetTitle("A.U.");
            DAC_hist_all[i] -> SetLineColor( 1 /*TColor::GetColor(Form("%s",color_code[1].Data()))*/ );
            DAC_hist_all[i] -> SetLineWidth(3);
            DAC_hist_all[i] -> SetStats(0);
            if (i == (check_number - 1)) // note : all combined, TH1F[26]
            {
                DAC_hist_all[i] -> SetTitle(Form("Chip level, all chips"));
            }
            else // note : chip level [0 ~ 25] -> 1 to 26
            {
                DAC_hist_all[i] -> SetTitle(Form("Chip level, U%i",i+1));
            }

        } 

        cout<<"DAC_hist_comb config confirmation, Bin width : "<<DAC_hist_comb[0][0] -> GetBinWidth(2)<<" range : "<<DAC_hist_comb[0][0] -> GetXaxis() -> GetXmin()<<" to "<<DAC_hist_comb[0][0] -> GetXaxis() -> GetXmax()<<endl;
    }
    else if (ana_level == "channel")
    {
        throw std::invalid_argument("Input channel, not done yet");
        return;
    }
    else 
    {
        throw std::invalid_argument("Wrong ana_level input !");
        return;
    }
    
    cout<<"Initialization done"<<endl;

}

void LadderDAC::Fill(int run_id, int chip_id, int chan_id, int adc)
{   
    if (chip_id > 0 && chip_id < 27 && chan_id > -1 && chan_id < 128 && adc > -1 && adc < 8)
    {
        // note : run_ID starts from zero
        // note : for the chips
        DAC_hist_bin[chip_id - 1][run_id] -> Fill(adc);
        if (adc < 7) DAC_hist_comb[chip_id - 1][run_id] -> Fill(Scan_config[run_id][adc]); // note : adc 0 ~ 6`

        // note : for the half-ladders 
        DAC_hist_bin.back()[run_id] -> Fill(adc);
        if (adc < 7) DAC_hist_comb.back()[run_id] -> Fill(Scan_config[run_id][adc]); // note : adc 0 ~ 6
    }
    
}

int LadderDAC::bin_convert(int run_id, int bin)
{
    return (Scan_config[run_id][bin] / DAC_hist_comb[0][0] -> GetBinWidth(2)) + 1;
}

void LadderDAC::Weight()
{

    double previous_content;
    double next_content;
    double scale_weight;

    int selected_run = 4; // todo : the selected run to do the weighting is here

    for (int i = 0; i < DAC_hist_comb.size(); i++) // note : handle all the hists, 27 for now
    {
        previous_content = 0;
        next_content = 0;
        scale_weight = 0;

        for (int i1 = selected_run; i1 > 0; i1--)
        {

            // note : the first run, bin 6th and 7th	
            previous_content = DAC_hist_comb[i][i1 - 1] -> GetBinContent(bin_convert(i1 - 1, 5)) + DAC_hist_comb[i][i1 - 1] -> GetBinContent(bin_convert(i1 - 1, 6)); // note : adc5 and adc6 bin of the previous hist
            // note : the second run, bin 1th and 2th
            next_content     = DAC_hist_comb[i][i1] -> GetBinContent(bin_convert(i1, 0)) + DAC_hist_comb[i][i1] -> GetBinContent(bin_convert(i1, 1)); // note : adc0 and adc1 bin of the next hist              

            // note : no hit in the bin 6 and 7 of the first run. 
            if (previous_content == 0) { scale_weight = 1; }
            else
            {
                if (next_content == 0) { next_content = 1.; }
                scale_weight = next_content / previous_content; // note : only for the first bin to match the second bin	
            }

            DAC_hist_comb[i][i1 - 1] -> Scale(scale_weight); // todo : we are using the "selected_run" run to re-weight the first run

            previous_content = 0;
            next_content = 0;
        }

        // // note : the first run, bin 6th and 7th	
		// previous_content = DAC_hist_comb[i][0] -> GetBinContent(bin_convert(0,5)) + DAC_hist_comb[i][0] -> GetBinContent(bin_convert(0,6)); // note : adc5 and adc6 bin of the previous hist
		// // note : the second run, bin 1th and 2th
		// next_content     = DAC_hist_comb[i][1] -> GetBinContent(bin_convert(1,0)) + DAC_hist_comb[i][1] -> GetBinContent(bin_convert(1,1)); // note : adc0 and adc1 bin of the next hist  

        // // note : no hit in the bin 6 and 7 of the first run. 
		// if (previous_content == 0) { scale_weight = 1; }
		// else
		// {
		// 	if (next_content == 0) { next_content = 1.; }
		// 	scale_weight = next_content / previous_content; // note : only for the first bin to match the second bin	
		// }

        // DAC_hist_comb[i][0] -> Scale(scale_weight); // todo : we are using the second run to re-weight the first run

        // previous_content = 0;
		// next_content = 0;
        
        for (int i1 = selected_run; i1 < Scan_config.size() - 1; i1++) 
        {
            // note : the first run, bin 6th and 7th	
            previous_content = DAC_hist_comb[i][i1] -> GetBinContent(bin_convert(i1,5)) + DAC_hist_comb[i][i1] -> GetBinContent(bin_convert(i1,6)); // note : adc5 and adc6 bin of the previous hist
            // note : the second run, bin 1th and 2th
            next_content     = DAC_hist_comb[i][i1 + 1] -> GetBinContent(bin_convert(i1 + 1,0)) + DAC_hist_comb[i][i1 + 1] -> GetBinContent(bin_convert(i1 + 1,1)); // note : adc0 and adc1 bin of the next hist 

            if (previous_content == 0) { scale_weight = 1; }
            else
            {
                if (next_content == 0) { next_content = 1.; }
                scale_weight = next_content / previous_content; // note : only for the first bin to match the second bin	
            }

            DAC_hist_comb[i][i1 + 1] -> Scale(1. / scale_weight);

            previous_content = 0;
			next_content = 0;

        }


    }   


    
}

void LadderDAC::Fill_final()
{
    for (int i = 0; i < DAC_hist_all.size(); i++) // note : resolution, 27 for the moment
    {
        for (int run = 0; run < DAC_hist_comb[i].size(); run++) // note : number of runs
        {
            for (int bin = 0; bin < DAC_hist_all[i] -> GetNbinsX(); bin++) // note : number of bins
            {
                if (DAC_hist_comb[i][run] -> GetBinContent(bin + 1) != 0)
                {
                    if (DAC_hist_all[i] -> GetBinContent(bin + 1) == 0 )
                    {
                        DAC_hist_all[i] -> SetBinContent(bin + 1, DAC_hist_comb[i][run] -> GetBinContent(bin + 1));
                    }
                    else 
                    {
                        int temp_origin_content = DAC_hist_all[i] -> GetBinContent(bin + 1);
                        DAC_hist_all[i] -> SetBinContent(bin + 1, (temp_origin_content + DAC_hist_comb[i][run] -> GetBinContent(bin + 1)) / 2. );
                    }
                }
            }
        }
    }
}

void LadderDAC::empty_check()
{

    for (int i = 0; i < DAC_hist_bin.size(); i++)
    {   
        for (int run = 0; run < run_number; run++ )
        {
            // note : the bin[8] is the overflow bin, useless
            unit_entry_array[i] += (DAC_hist_bin[i][run] -> GetNbinsX() == 8) ? (DAC_hist_bin[i][run] -> GetEntries() - DAC_hist_bin[i][run] -> GetBinContent(8)) : (DAC_hist_bin[i][run] -> GetEntries());
        }
        
    }   

}

void LadderDAC::Fit()
{
    pair<TString,double> Par_Land_width   = {"LandWidth     ", 4.5  };
    pair<TString,double> Par_MPV          = {"MPV           ", 93.8 };
    pair<TString,double> Par_size         = {"LandGaus_size ", 10   };
    pair<TString,double> Par_Gaus_width   = {"Gaus_width    ", 12   };

    pair<TString,double> Par_Gaus1_height = {"Gaus1_height  ", 1 };
    pair<TString,double> Par_Gaus1_mean   = {"Gaus1_mean    ", 0   }; 
    pair<TString,double> Par_Gaus1_width  = {"Gaus1_width   ", 10  };

    pair<TString,double> Par_Gaus2_height = {"Gaus2_height  ", 1   };
    pair<TString,double> Par_Gaus2_mean   = {"Gaus2_mean    ", 0   }; 
    pair<TString,double> Par_Gaus2_width  = {"Gaus2_width   ", 30  };

    pair<TString,double> Par_Exp_slope    = {"Exp_slope     ", 0.1 };
    pair<TString,double> Par_Exp_x_offset = {"Exp_x_offset  ", 1.5 }; 
    pair<TString,double> Par_Exp_y_offset = {"Exp_y_offset  ", 0.  };



    LadderDAC::empty_check(); // note : for the empty check 

    vector<double> re_chi_vec; re_chi_vec.clear();

    for (int i = 0; i < DAC_hist_all.size(); i++) {

        if (unit_entry_array[i] < empty_criterion) continue; // note : if the total used-bin entry is less than 1000, don't fit.
        
        DAC_hist_all[i] -> Scale(1. / DAC_hist_all[i] -> Integral(-1,-1)); // note : normalize the distribution

        Fit_vec[i][0] = new TF1("",Func_Sig_Gaus_Gaus, 0, DAC_hist_all[i] -> GetXaxis() -> GetXmax(),10); // todo : the N_parameters is given manually
        Fit_vec[i][1] = new TF1("",Func_Sig_Gaus, 0, DAC_hist_all[i] -> GetXaxis() -> GetXmax(),7); // todo : the N_parameters is given manually

        Fit_vec[i][0] -> SetParNames(
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
        Fit_vec[i][0] -> SetParameters(
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

        Fit_vec[i][1] -> SetParNames(
            Par_Land_width.first, 
            Par_MPV.first, 
            Par_size.first, 
            Par_Gaus_width.first, 

            Par_Gaus1_height.first, 
            Par_Gaus1_mean.first, 
            Par_Gaus1_width.first
        );
        Fit_vec[i][1] -> SetParameters(
            Par_Land_width.second, 
            Par_MPV.second, 
            Par_size.second, 
            Par_Gaus_width.second, 
            
            Par_Gaus1_height.second, 
            Par_Gaus1_mean.second, 
            Par_Gaus1_width.second
        );        
        
        // todo : limit of the mean of the gaussian bkg, under testing 
        Fit_vec[i][0] -> SetParLimits(5,-1000,35); Fit_vec[i][0] -> SetParLimits(8,-1000,35);
        Fit_vec[i][1] -> SetParLimits(5,-1000,35);

        // note : the height of the gaussian can not be negative
        Fit_vec[i][0] -> SetParLimits(4,0,100000); Fit_vec[i][0] -> SetParLimits(7,0,100000);
        Fit_vec[i][1] -> SetParLimits(4,0,100000);


        Fit_vec[i][2] = (TF1*)Fit_vec[i][0] -> Clone("");
        Fit_vec[i][2] -> FixParameter(5,0); Fit_vec[i][2] -> FixParameter(8,0);

        Fit_vec[i][3] = (TF1*)Fit_vec[i][1] -> Clone("");
        Fit_vec[i][3] -> FixParameter(5,0);

        // note : exp in 
        Fit_vec[i][4] = new TF1("",Func_Sig_Exp, 0, DAC_hist_all[i] -> GetXaxis() -> GetXmax(),7);
        Fit_vec[i][5] = new TF1("",Func_Sig_Exp_Gaus, 0, DAC_hist_all[i] -> GetXaxis() -> GetXmax(),10);

        Fit_vec[i][4] -> SetParNames(
            Par_Land_width.first, 
            Par_MPV.first, 
            Par_size.first, 
            Par_Gaus_width.first, 

            Par_Exp_slope.first, 
            Par_Exp_x_offset.first, 
            Par_Exp_y_offset.first
        );
        Fit_vec[i][4] -> SetParameters(
            Par_Land_width.second, 
            Par_MPV.second, 
            Par_size.second, 
            Par_Gaus_width.second, 
            
            Par_Exp_slope.second, 
            Par_Exp_x_offset.second, 
            Par_Exp_y_offset.second
        );

        Fit_vec[i][5] -> SetParNames(
            Par_Land_width.first, 
            Par_MPV.first, 
            Par_size.first, 
            Par_Gaus_width.first, 

            Par_Exp_slope.first, 
            Par_Exp_x_offset.first, 
            Par_Exp_y_offset.first, 

            Par_Gaus2_height.first,
            Par_Gaus2_mean.first,
            Par_Gaus2_width.first
        );
        Fit_vec[i][5] -> SetParameters(
            Par_Land_width.second, 
            Par_MPV.second, 
            Par_size.second, 
            Par_Gaus_width.second, 
            
            Par_Exp_slope.second, 
            Par_Exp_x_offset.second, 
            Par_Exp_y_offset.second, 
            
            Par_Gaus2_height.second,
            Par_Gaus2_mean.second,
            Par_Gaus2_width.second
        );   

        // note : the height of the gaussian can not be negative
        Fit_vec[i][5] -> SetParLimits(7,0,100000);

        for (int N_fit = 0; N_fit < Fit_vec[i].size(); N_fit++)
        {
            Fit_vec[i][N_fit] -> SetLineColor(TColor::GetColor(color_code_2[6]));            
            Fit_vec[i][N_fit] -> SetLineWidth(5);
            Fit_vec[i][N_fit] -> SetNpx(10000);

            // note : fit the distribution with four functions, and compare the reduced chi square.
            DAC_hist_all[i] -> Fit(Fit_vec[i][N_fit],"NQ");
            re_chi_vec.push_back( (Fit_vec[i][N_fit] -> GetChisquare() / Fit_vec[i][N_fit] -> GetNDF()) );

            cout<<"Fit check "<<N_fit<<" : "<<re_chi_vec[N_fit]<<endl;
        }

        auto minIndex = std::distance(re_chi_vec.begin(), std::min_element(re_chi_vec.begin(), re_chi_vec.end()));
        fit_tag[i] = minIndex;

        cout<<"winner fit tag : "<< minIndex <<endl;
        cout<<""<<endl;

        re_chi_vec.clear();
    }
}

void LadderDAC::Spit_BinContent_All()
{
    for ( int i = 0; i < DAC_hist_comb.back()[3] -> GetNbinsX(); i++ )
    {
        // cout<<" Bin "<<i + 1<<" content : "<<DAC_hist_comb.back()[3] -> GetBinContent(i + 1)<<" Error : "<<DAC_hist_comb.back()[3] -> GetBinError(i + 1)<<" "<<DAC_hist_comb.back()[3] -> GetBinErrorLow(i + 1)<<" "<<DAC_hist_comb.back()[3] -> GetBinErrorUp(i + 1)<<endl;
        cout<<"DAC_hist -> SetBinContent("<<i+1<<", "<<DAC_hist_all.back() -> GetBinContent(i + 1)<<"); DAC_hist -> SetBinError("<<i+1<<", "<<DAC_hist_all.back() -> GetBinError(i + 1)<<");"<<endl;
    }
}

void LadderDAC::Output_bin_plots(TString plot_folder_dire, TString set_name, bool bin8_set_zero = false)
{    
    TCanvas * c_bin = new TCanvas("c_bin","c_bin",800,800);
    // c_bin -> Divide(3,3); // todo : the number of the plots are fixed, 8 runs are maximum

    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.08);

    c_bin -> Print( Form("%s/DAC_scan_matrix_%s.pdf(", plot_folder_dire.Data(), ladder_name.Data()) );
	for (int i = 0; i < DAC_hist_bin.size(); i++)
    {	
        // cout<<"size test "<<DAC_hist_bin.size()<<endl;
        c_bin -> Divide(3,3); // todo : the number of the plots are fixed, 8 runs are maximum

		for (int i1 = 0; i1 < DAC_hist_bin[i].size(); i1++)
        {
			c_bin->cd(i1+1);

            if ( bin8_set_zero == true ) { DAC_hist_bin[i][i1] -> SetBinContent(8,0); }

			DAC_hist_bin[i][i1] -> Draw("hist");	
            // cout<<"matrix : "<<i<<" "<<i1<<endl;
		}
        c_bin -> cd(9);
        draw_text -> DrawLatex(0., 0.5, Form("%s",set_name.Data()));
        if (i == DAC_hist_bin.size() - 1 ){
            draw_text -> DrawLatex(0., 0.35, Form("FC#%i, Port %s, All chips",Felix_ch,ladder_port.Data()));
        }
        else {
            draw_text -> DrawLatex(0., 0.35, Form("FC#%i, Port %s, U%i",Felix_ch,ladder_port.Data(),i+1));
        }
        if (bin8_set_zero == true){
            draw_text -> DrawLatex(0., 0.2, Form("Bin8 set content 0"));
        }
        

		c_bin -> Print( Form("%s/DAC_scan_matrix_%s.pdf",plot_folder_dire.Data(), ladder_name.Data()) );
        c_bin -> Clear();
	}
	c_bin -> Print( Form("%s/DAC_scan_matrix_%s.pdf)",plot_folder_dire.Data(), ladder_name.Data()) );
	c_bin -> Clear();
}

void LadderDAC::Output_comb_plots(TString plot_folder_dire, TString set_name)
{
    TCanvas * c1 = new TCanvas("c1","c1",800,800);

    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.025);

    c1 -> Print( Form("%s/DAC_scan_overlap_%s.pdf(", plot_folder_dire.Data(), ladder_name.Data()) );
	for (int i = 0; i < DAC_hist_comb.size(); i++) // note : chip + total
    {	
		for (int i1 = 0; i1 < DAC_hist_comb[i].size(); i1++) // note : runs
        {
            DAC_hist_comb[i][i1] -> SetMaximum( DAC_hist_comb[i][0] -> GetMaximum() * 3 );
            
            // (i != DAC_hist_comb.size() - 1) ? DAC_hist_comb[i][i1]->GetYaxis()->SetRangeUser(0,100000) : DAC_hist_comb[i][i1]->GetYaxis()->SetRangeUser(0,260000); // note : because the "scale" function

			if (i1 == 0) { DAC_hist_comb[i][i1] -> Draw("hist"); }
			else { DAC_hist_comb[i][i1]->Draw("hist same"); }
		}
        
        draw_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, Port %s",set_name.Data(),Felix_ch,ladder_port.Data()));

		c1 -> Print( Form("%s/DAC_scan_overlap_%s.pdf",plot_folder_dire.Data(), ladder_name.Data()) );
        c1 -> Clear();
	}
	c1 -> Print( Form("%s/DAC_scan_overlap_%s.pdf)",plot_folder_dire.Data(), ladder_name.Data()) );
	c1 -> Clear();


}

void LadderDAC::Output_final_plots(TString plot_folder_dire, TString set_name, int fit_opt = -1)
{
    TCanvas * c1 = new TCanvas("c1","c1",800,800);

    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.02);

    TLegend * legend = new TLegend (0.3, 0.85, 0.9, 0.90);
    legend -> SetNColumns(4);
    legend -> SetTextSize (0.028);

    vector<int> fit_tag_opt(fit_tag.size(),-1);

    if (fit_opt == -1) // note : use the fitting with the smallest reduced chisquare
    {
        fit_tag_opt = fit_tag;
    }
    else 
    {
        for (int i = 0; i < fit_tag.size(); i++)
        {            
            if (fit_tag[i] != -1){fit_tag_opt[i] = fit_opt;}
        }
    }


    c1 -> Print( Form("%s/DAC_scan_final_%s_FitOpt_%i.pdf(", plot_folder_dire.Data(), ladder_name.Data(), fit_opt) );
	for (int i = 0; i < DAC_hist_all.size(); i++) // note : chip + total
    {		
        DAC_hist_all[i] -> SetMaximum( DAC_hist_all[i] -> GetMaximum() * 1.4 );

        DAC_hist_all[i] -> Draw("ep");
        draw_text -> DrawLatex(0.11, 0.91, Form("%s, FC#%i, Port %s",set_name.Data(),Felix_ch,ladder_port.Data()));

        if (fit_tag_opt[i] == -1) {
            c1 -> Print( Form("%s/DAC_scan_final_%s_FitOpt_%i.pdf",plot_folder_dire.Data(), ladder_name.Data(), fit_opt) );
            c1 -> Clear();
            continue;
        }
		
        TF1 * Line_signal = new TF1("", Func_Langaus, 0, DAC_hist_all[i] -> GetXaxis() -> GetXmax(), 4);
        Line_signal -> SetParameter( 0, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(0) );  Line_signal -> SetParError( 0, Fit_vec[i][fit_tag_opt[i]] -> GetParError(0) );
        Line_signal -> SetParameter( 1, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(1) );  Line_signal -> SetParError( 1, Fit_vec[i][fit_tag_opt[i]] -> GetParError(1) );
        Line_signal -> SetParameter( 2, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(2) );  Line_signal -> SetParError( 2, Fit_vec[i][fit_tag_opt[i]] -> GetParError(2) );
        Line_signal -> SetParameter( 3, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(3) );  Line_signal -> SetParError( 3, Fit_vec[i][fit_tag_opt[i]] -> GetParError(3) );
        Line_signal -> SetLineColor(  TColor::GetColor( color_code_2[5] ) );
        Line_signal -> SetLineStyle(9);
        // Line_signal -> SetNpx(10000);

        TF1 * Line_bg_1 = new TF1("", ( fit_tag_opt[i] == 4 || fit_tag_opt[i] == 5 ) ? Func_exp_draw : Func_gaus_draw, 0, DAC_hist_all[i] -> GetXaxis() -> GetXmax(), 3);
        Line_bg_1 -> SetParameter( 0, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(4) );  Line_bg_1 -> SetParError( 0, Fit_vec[i][fit_tag_opt[i]] -> GetParError(4) );
        Line_bg_1 -> SetParameter( 1, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(5) );  Line_bg_1 -> SetParError( 1, Fit_vec[i][fit_tag_opt[i]] -> GetParError(5) );
        Line_bg_1 -> SetParameter( 2, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(6) );  Line_bg_1 -> SetParError( 2, Fit_vec[i][fit_tag_opt[i]] -> GetParError(6) );
        Line_bg_1 -> SetLineColor(  TColor::GetColor(color_code_2[7]) );
        Line_bg_1 -> SetLineStyle(9);
        // Line_bg_1 -> SetNpx(10000);

        legend -> AddEntry (Fit_vec[i][fit_tag_opt[i]], "Total Fit", "l");
        legend -> AddEntry (Line_signal, "Signal", "l");
        legend -> AddEntry (Line_bg_1, "Bkg-1", "l");

        Fit_vec[i][fit_tag_opt[i]] -> Draw("lsame");
        Line_signal -> Draw("lsame");
        Line_bg_1   -> Draw("lsame");

        TF1 * Line_bg_2;

        if(fit_tag_opt[i] == 0 || fit_tag_opt[i] == 2 || fit_tag_opt[i] == 5)
        {
            Line_bg_2 = new TF1("", Func_gaus_draw, 0, DAC_hist_all[i] -> GetXaxis() -> GetXmax(), 3);
            Line_bg_2 -> SetParameter(0, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(7) );  Line_bg_2 -> SetParError(0, Fit_vec[i][fit_tag_opt[i]] -> GetParError(7) );
            Line_bg_2 -> SetParameter(1, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(8) );  Line_bg_2 -> SetParError(1, Fit_vec[i][fit_tag_opt[i]] -> GetParError(8) );
            Line_bg_2 -> SetParameter(2, Fit_vec[i][fit_tag_opt[i]] -> GetParameter(9) );  Line_bg_2 -> SetParError(2, Fit_vec[i][fit_tag_opt[i]] -> GetParError(9) );
            Line_bg_2 -> SetLineColor(  TColor::GetColor( color_code[6] ) );
            Line_bg_2 -> SetLineStyle(9);
            // Line_bg_2 -> SetNpx(10000);
            legend -> AddEntry (Line_bg_2, "Bkg-2", "l");

            Line_bg_2 -> Draw("lsame");

            draw_text -> DrawLatex(0.6, 0.610 - 0.05, Form("#bullet #bf{G2 height : %.3f #pm %.3f}", Line_bg_2 -> GetParameter(0), Line_bg_2 -> GetParError(0)));
            draw_text -> DrawLatex(0.6, 0.580 - 0.05, Form("#bullet #bf{G2 mean   : %.3f #pm %.3f}", Line_bg_2 -> GetParameter(1), Line_bg_2 -> GetParError(1)));
            draw_text -> DrawLatex(0.6, 0.550 - 0.05, Form("#bullet #bf{G2 width  : %.3f #pm %.3f}", Line_bg_2 -> GetParameter(2), Line_bg_2 -> GetParError(2)));

        }

        draw_text -> DrawLatex(0.6, 0.850 - 0.05, Form("Signal"));
        draw_text -> DrawLatex(0.6, 0.820 - 0.05, Form("#bullet #bf{Land width : %.3f #pm %.3f}", Line_signal -> GetParameter(0), Line_signal -> GetParError(0)));
        draw_text -> DrawLatex(0.6, 0.790 - 0.05, Form("#bullet #bf{Gaus width : %.3f #pm %.3f}", Line_signal -> GetParameter(3), Line_signal -> GetParError(3)));
        draw_text -> DrawLatex(0.6, 0.760 - 0.05, Form("#bullet #bf{MPV        : %.3f #pm %.3f}", Line_signal -> GetParameter(1), Line_signal -> GetParError(1)));

        TString B1_P0_text = (fit_tag_opt[i] == 4 || fit_tag_opt[i] == 5) ? "Exp P0 " : "G1 height ";
        TString B1_P1_text = (fit_tag_opt[i] == 4 || fit_tag_opt[i] == 5) ? "Exp P1 " : "G1 mean   "; 
        TString B1_P2_text = (fit_tag_opt[i] == 4 || fit_tag_opt[i] == 5) ? "Exp P2 " : "G1 width  ";

        draw_text -> DrawLatex(0.6, 0.730 - 0.05, Form("Background"));
        draw_text -> DrawLatex(0.6, 0.700 - 0.05, Form("#bullet #bf{%s: %.3f #pm %.3f}", B1_P0_text.Data(), Line_bg_1 -> GetParameter(0), Line_bg_1 -> GetParError(0)));
        draw_text -> DrawLatex(0.6, 0.670 - 0.05, Form("#bullet #bf{%s: %.3f #pm %.3f}", B1_P1_text.Data(), Line_bg_1 -> GetParameter(1), Line_bg_1 -> GetParError(1)));
        draw_text -> DrawLatex(0.6, 0.640 - 0.05, Form("#bullet #bf{%s: %.3f #pm %.3f}", B1_P2_text.Data(), Line_bg_1 -> GetParameter(2), Line_bg_1 -> GetParError(2)));

        if (fit_tag_opt[i] == 4 || fit_tag_opt[i] == 5)
        {
            draw_text -> DrawLatex(0.6, 0.520 - 0.05, Form("#bullet #bf{BG1 : exp(-(P0 * x + P1)) + P2}"));
        }// TMath::Exp(-1 * (par[4] * x[0] + par[5])) + par[6]

        DAC_hist_all[i] -> Draw("ep same");
        legend -> Draw("same");

		c1 -> Print( Form("%s/DAC_scan_final_%s_FitOpt_%i.pdf",plot_folder_dire.Data(), ladder_name.Data(), fit_opt) );
        c1 -> Clear();
        legend -> Clear();
	}
	c1 -> Print( Form("%s/DAC_scan_final_%s_FitOpt_%i.pdf)",plot_folder_dire.Data(), ladder_name.Data(), fit_opt) );
	c1 -> Clear();
}

void LadderDAC::Output_root(TString plot_folder_dire, TString set_name)
{
    // note : file name, ladder_name, module, used sigma, classify based, cut range 
    // note : chip, chan, entry, entry_ampl_cut, turn_on, width
    // todo : if the cut increases, this part has to be updated as well.

    TFile * file_output = new TFile(Form("%s/%s_fit_info.root", plot_folder_dire.Data(), ladder_name.Data()), "RECREATE");
    file_output -> cd();

    TString set_name_tree;
    TString ladder_name_tree;
    int ROC_index_tree;
    int Felix_ch_tree;
    int port_id_tree;

    int layer_index_tree; // note : B0L0 -> 0, B0L1 -> 1, B1L0 -> 2, B1L1 -> 3
    int ladder_index_tree; // note : 0 to 11 or 0 to 15
    int arm_tree; // note : south : 0, north : 1

    int chip_tree;
    
    double Par_Land_width_tree;   
    double Par_MPV_tree;          
    double Par_size_tree;         
    double Par_Gaus_width_tree;   

    double Par_Gaus1_height_tree; 
    double Par_Gaus1_mean_tree;   
    double Par_Gaus1_width_tree;  

    double Par_Gaus2_height_tree; 
    double Par_Gaus2_mean_tree;   
    double Par_Gaus2_width_tree;  

    TTree * fit_info_tree = new TTree("edep_fit","The fitting of the each edep distribution");
    fit_info_tree -> Branch("set_name",&set_name_tree);
    fit_info_tree -> Branch("roc_id",&ROC_index_tree);
    fit_info_tree -> Branch("ladder",&ladder_name_tree);
    fit_info_tree -> Branch("layer_index",&layer_index_tree);
    fit_info_tree -> Branch("ladder_index",&ladder_index_tree);
    fit_info_tree -> Branch("arm",&arm_tree);
    fit_info_tree -> Branch("felix_ch",&Felix_ch_tree);
    fit_info_tree -> Branch("port_id",&port_id_tree);
    
    fit_info_tree -> Branch("chip_id",&chip_tree);
    
    fit_info_tree -> Branch("Par_Land_width", &Par_Land_width_tree);   
    fit_info_tree -> Branch("Par_MPV", &Par_MPV_tree);          
    fit_info_tree -> Branch("Par_size", &Par_size_tree);         
    fit_info_tree -> Branch("Par_Gaus_width", &Par_Gaus_width_tree);   

    fit_info_tree -> Branch("Par_B1_P0", &Par_Gaus1_height_tree); 
    fit_info_tree -> Branch("Par_B1_P1", &Par_Gaus1_mean_tree);   
    fit_info_tree -> Branch("Par_B1_P2", &Par_Gaus1_width_tree);  

    fit_info_tree -> Branch("Par_Gaus2_height", &Par_Gaus2_height_tree); 
    fit_info_tree -> Branch("Par_Gaus2_mean", &Par_Gaus2_mean_tree);   
    fit_info_tree -> Branch("Par_Gaus2_width", &Par_Gaus2_width_tree); 

    set_name_tree = set_name;
    ROC_index_tree = ROC_index;

    ladder_name_tree = ladder_name;
    layer_index_tree = layer_map.at(TString(ladder_name(0,4))); // note : "BxLx"yyS
    if (TString(ladder_name(4,2)).IsDigit() != true) throw std::logic_error(Form("---- Ladder name format incorrect -->%s<-- ----",ladder_name.Data()));
    ladder_index_tree = TString(ladder_name(4,2)).Atoi(); // note : BxLx"yy"S
    arm_tree = (ladder_name[6] == 'S') ? 0 : 1;

    Felix_ch_tree = Felix_ch;
    port_id_tree = port_map.at(ladder_port);

    for (int i = 0; i < DAC_hist_all.size(); i++)
    {
        chip_tree             = (i != DAC_hist_all.size() - 1) ? i+1 : 999;

        Par_Land_width_tree   = ( fit_tag[i] == -1 ) ? -999 : Fit_vec[i][fit_tag[i]] -> GetParameter(0);   
        Par_MPV_tree          = ( fit_tag[i] == -1 ) ? -999 : Fit_vec[i][fit_tag[i]] -> GetParameter(1);          
        Par_size_tree         = ( fit_tag[i] == -1 ) ? -999 : Fit_vec[i][fit_tag[i]] -> GetParameter(2);         
        Par_Gaus_width_tree   = ( fit_tag[i] == -1 ) ? -999 : Fit_vec[i][fit_tag[i]] -> GetParameter(3);   

        Par_Gaus1_height_tree = ( fit_tag[i] == -1 ) ? -999 : Fit_vec[i][fit_tag[i]] -> GetParameter(4); 
        Par_Gaus1_mean_tree   = ( fit_tag[i] == -1 ) ? -999 : Fit_vec[i][fit_tag[i]] -> GetParameter(5);   
        Par_Gaus1_width_tree  = ( fit_tag[i] == -1 ) ? -999 : Fit_vec[i][fit_tag[i]] -> GetParameter(6);  
        
        Par_Gaus2_height_tree = ( fit_tag[i] == 0 || fit_tag[i] == 2 ) ? Fit_vec[i][fit_tag[i]] -> GetParameter(7) : -999; 
        Par_Gaus2_mean_tree   = ( fit_tag[i] == 0 || fit_tag[i] == 2 ) ? Fit_vec[i][fit_tag[i]] -> GetParameter(8) : -999;   
        Par_Gaus2_width_tree  = ( fit_tag[i] == 0 || fit_tag[i] == 2 ) ? Fit_vec[i][fit_tag[i]] -> GetParameter(9) : -999; 

        fit_info_tree -> Fill();
    }

    file_output -> cd();
    fit_info_tree -> Write("", TObject::kOverwrite);
    
    file_output -> Close();
}

#endif

// todo : 
// todo : deal with the empty           V
// todo : true distribution comparison  V
// todo : BG, guas fix at zero          V
// todo : BG, exp() ?                   V