// note : find the rightmost bin with at least one entry
double GetRightEdge(TH1D* hist){
    int nbin = hist->GetNbinsX();
    for (int i = nbin; i > 0; i--){
        if (hist->GetBinContent(i) > 0) return hist->GetBinLowEdge(i);
    }
    return -10;
}

int MakeTemplate()
{
    bool is_dark = true;
    bool set_logY = true;
    string input_directory = "/sphenix/user/ChengWei/Data_temp/run_54280/ChipOccupancy_Trig30cm/completed";
    string input_filename = "ChipOccupancy_HitQA_CloneHitRemovedBCO_MBDNSTrigvtxZ30cm_00054280_merged.root";
    string output_directory = input_directory;
    string final_output_file_name = input_filename.substr(0, input_filename.find(".root"));
    
    final_output_file_name.insert(0, "ForTemplate_");

    if (is_dark) {
        gStyle->SetPalette(kInvertedDarkBodyRadiator);

        final_output_file_name += "_darkPalette";
    }

    final_output_file_name += (set_logY) ? "_logY" : "";

    std::map<std::string, TH1D*> data_hist_h1_map; data_hist_h1_map.clear();

    TFile * file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_filename.c_str()));
    if (file_in == nullptr)
    {
        std::cout << "Error: file_in can not be opened" << std::endl;
        exit(1);
    }


    for (TObject* keyAsObj : *file_in->GetListOfKeys())
    {
        auto key = dynamic_cast<TKey*>(keyAsObj);
        std::string hist_name  = key->GetName();
        std::string class_name = key->GetClassName();

        if (class_name == "TH1D")
        {
            data_hist_h1_map[hist_name.c_str()] = (TH1D*) file_in -> Get( hist_name.c_str() );
        }
    }

    TH1D * chip_nHit_mean = new TH1D("chip_nHit_mean","chip_nHit_mean;Mean of nHit (per chip and per HitBco);Number of chips", 100,0,5);
    TH1D * chip_nHit_RightEdge = new TH1D("chip_nHit_RightEdge","chip_nHit_RightEdge;Right edge (per chip and per HitBco);Number of chips", 95,-15,80);

    std::pair<double,double> normal_range = {60,80};
    double normal_threshold = 67;
    std:vector<std::string> normal_chip_vec; normal_chip_vec.clear();

    std::pair<double,double> halfentry_range = {30,40};
    double halfentry_threshold = 31;
    std::vector<std::string> halfentry_chip_vec; halfentry_chip_vec.clear();

    TLine * line = new TLine();
    line -> SetLineColor(kRed);
    line -> SetLineWidth(2);
    line -> SetLineStyle(2);

    TCanvas * c1 = new TCanvas("c1", "c1", 800, 600);
    c1 -> SetLogy(set_logY);
    c1 -> Print(Form("%s/%s.pdf(", output_directory.c_str(), final_output_file_name.c_str()));

    for (auto &pair : data_hist_h1_map)
    {
        c1 -> SetLogy(set_logY);

        double hist_right_edge = GetRightEdge(pair.second);

        pair.second -> SetTitle(pair.first.c_str());
        pair.second -> GetXaxis() -> SetTitle("nHits (per chip and per HitBco)");
        // pair.second -> SetStats(0);
        // pair.second -> SetMinimum(0);
        pair.second -> Draw("hist");

        
        if (normal_range.first < hist_right_edge && hist_right_edge < normal_range.second)
        {
            line -> DrawLine(normal_threshold, 0, normal_threshold, pair.second -> GetMaximum());
            normal_chip_vec.push_back(pair.first);
        }
        else if (halfentry_range.first < hist_right_edge && hist_right_edge < halfentry_range.second)
        {
            line -> DrawLine(halfentry_threshold, 0, halfentry_threshold, pair.second -> GetMaximum());
            halfentry_chip_vec.push_back(pair.first);
        }


        c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), final_output_file_name.c_str()));
        c1 -> Clear();

        chip_nHit_mean -> Fill(pair.second -> GetMean());

        chip_nHit_RightEdge -> Fill(hist_right_edge);
    }

    c1 -> Print(Form("%s/%s.pdf)", output_directory.c_str(), final_output_file_name.c_str()));
    c1 -> Clear();

    c1 -> cd();
    chip_nHit_mean -> Draw("hist");
    c1 -> Print(Form("%s/chip_nHit_mean.pdf", output_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    chip_nHit_RightEdge -> SetStats(0);
    chip_nHit_RightEdge -> Draw("hist");
    c1 -> Print(Form("%s/chip_nHit_RightEdge.pdf", output_directory.c_str()));
    c1 -> Clear();

    std::cout<<"normal_chip_vec"<<std::endl;
    for (auto &chip : normal_chip_vec)
    {
        std::cout <<"{\""<< chip <<"\"},"<< std::endl;
    }

    std::cout<<"halfentry_chip_vec"<<std::endl;
    for (auto &chip : halfentry_chip_vec)
    {
        std::cout <<"{\""<< chip <<"\"},"<< std::endl;
    }

    return 0;
}