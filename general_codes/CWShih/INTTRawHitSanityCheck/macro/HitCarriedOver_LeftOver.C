double  vector_average (vector <double> input_vector) {
	return accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
}

double vector_stddev (vector <double> input_vector){
	
	double sum_subt = 0;
	double average  = accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());
	
	// cout<<"average is : "<<average<<endl;

	for (int i=0; i<input_vector.size(); i++){ sum_subt += pow((input_vector[i] - average),2); }

	//cout<<"sum_subt : "<<sum_subt<<endl;
	// cout<<"print from the function, average : "<<average<<" std : "<<stddev<<endl;

	return sqrt( sum_subt / double(input_vector.size()-1) );
}	

int HitCarriedOver_LeftOver()
{
    std::string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_CW_Mar252025_INTTQA/completed";
    std::string input_filename = "HistStack_withCorrBack.root";
    int trigger_bin = 55;
    int local_range = 20;

    std::vector<std::string> input_hist_names = {
        "h1Dstack_h1D_HitBco_BcoFullM1",
        "h1Dstack_h1D_HitBco_BcoFullM2",
        "h1Dstack_h1D_HitBco_BcoFullM3",
        "h1Dstack_h1D_HitBco_BcoFullM4"
    };

    TFile * file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_filename.c_str()));
    if (!file_in || file_in->IsZombie()) {
        std::cerr << "Error opening file: " << input_filename << std::endl;
        return -1;
    }

    for (const auto& hist_name : input_hist_names) {
        std::cout<<"------------------------------------------------------------------------------------------------------------------------------------------------------------------------"<<std::endl;
        TH1D* hist = static_cast<TH1D*>(file_in->Get(hist_name.c_str()));
        if (!hist) {
            std::cerr << "Error: Histogram " << hist_name << " not found in file." << std::endl;
            return -2;
        }
        
        std::vector<double> bkg_vec; bkg_vec.clear();

        for (int i = 1; i <= hist->GetNbinsX(); ++i) {
            double x = hist->GetBinCenter(i);
            double y = hist->GetBinContent(i);
            double y_err = hist->GetBinError(i);
            double x_err = hist->GetBinWidth(i) / 2.0;
            
            if (y < hist->GetBinContent(hist->GetMaximumBin()) * 0.4) {continue;}
            if (i == hist->FindBin(trigger_bin)) {continue;}
            if (i < hist->FindBin(trigger_bin) - local_range || i > hist->FindBin(trigger_bin) + local_range) {continue;}

            // std::cout<<"x: "<<x<<", y: "<<y<<std::endl;
            bkg_vec.push_back(y);
        }

        double bkg_val = vector_average(bkg_vec);
        double bkg_err = vector_stddev(bkg_vec);
        double trigger_bin_val = hist->GetBinContent(hist->FindBin(trigger_bin));
        double trigger_bin_err = hist->GetBinError(hist->FindBin(trigger_bin));

        double std_deviation = (trigger_bin_val - bkg_val) / sqrt(pow(bkg_err,2) + pow(trigger_bin_err,2));

        std::cout<<hist_name<<", bkg_vec entries: "<<bkg_vec.size()<<", bkg_val: "<<bkg_val<<", bkg_err: "<<bkg_err<<", trigger_bin_val: "<<trigger_bin_val<<", trigger_bin_err: "<<trigger_bin_err<<", std_deviation: "<<std_deviation<<std::endl;
        std::cout<<std::endl;
    }
    

    return 888;
}


// int HitCarriedOver_LeftOver()
// {
//     std::string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_CW_Mar252025_INTTQA/completed";
//     std::string input_filename = "HistStack_withCorrBack.root";
//     int trigger_bin = 55;

//     std::vector<std::string> input_hist_names = {
//         "h1Dstack_h1D_HitBco_BcoFullM1",
//         "h1Dstack_h1D_HitBco_BcoFullM2",
//         "h1Dstack_h1D_HitBco_BcoFullM3",
//         "h1Dstack_h1D_HitBco_BcoFullM4"
//     };

//     TFile * file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_filename.c_str()));
//     if (!file_in || file_in->IsZombie()) {
//         std::cerr << "Error opening file: " << input_filename << std::endl;
//         return -1;
//     }

//     TF1 * pol0_func = new TF1("pol0_func", "pol0", 0,140);
//     pol0_func->SetLineWidth(1);
//     pol0_func->SetLineColor(kRed);

//     for (const auto& hist_name : input_hist_names) {
//         TH1D* hist = static_cast<TH1D*>(file_in->Get(hist_name.c_str()));
//         if (!hist) {
//             std::cerr << "Error: Histogram " << hist_name << " not found in file." << std::endl;
//             return -2;
//         }
        
//         TGraphErrors * graph = new TGraphErrors();
//         if(graph->GetN() != 0){
//             graph->Set(0);
//         }

//         for (int i = 1; i <= hist->GetNbinsX(); ++i) {
//             double x = hist->GetBinCenter(i);
//             double y = hist->GetBinContent(i);
//             double y_err = hist->GetBinError(i);
//             double x_err = hist->GetBinWidth(i) / 2.0;
            
//             if (y < hist->GetBinContent(hist->GetMaximumBin()) * 0.4) {continue;}
//             if (i == hist->FindBin(trigger_bin)) {continue;}

//             graph->SetPoint(graph->GetN(), x, y);
//             graph->SetPointError(graph->GetN() - 1, x_err, y_err);
//         }

//         graph -> Fit(pol0_func);
//         double pol0_val = pol0_func->GetParameter(0);
//         double pol0_err = pol0_func->GetParError(0);
//         double trigger_bin_val = hist->GetBinContent(hist->FindBin(trigger_bin));
//         double trigger_bin_err = hist->GetBinError(hist->FindBin(trigger_bin));

//         double std_deviation = fabs(pol0_val - trigger_bin_val) / sqrt(pow(pol0_err,2) + pow(trigger_bin_err,2));

//         std::cout<<hist_name<<", graph entries: "<<graph->GetN()<<", pol0_val: "<<pol0_val<<", pol0_err: "<<pol0_err<<", trigger_bin_val: "<<trigger_bin_val<<", trigger_bin_err: "<<trigger_bin_err<<", std_deviation: "<<std_deviation<<std::endl;
//     }
    

//     return 888;
// }