int MakePlot()
{
    bool is_dark = true;
    bool set_logY = true;
    string input_directory = "/sphenix/user/ChengWei/Data_temp/run_54280/ChipOccupancy_Trig30cm/completed";
    string input_filename = "ChipOccupancy_HitQA_CloneHitRemovedBCO_MBDNSTrigvtxZ30cm_00054280_merged.root";
    string output_directory = input_directory;
    string final_output_file_name = input_filename.substr(0, input_filename.find(".root"));
    

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

    TCanvas * c1 = new TCanvas("c1", "c1", 800, 600);
    c1 -> SetLogy(set_logY);
    c1 -> Print(Form("%s/%s.pdf(", output_directory.c_str(), final_output_file_name.c_str()));

    for (auto &pair : data_hist_h1_map)
    {
        c1 -> SetLogy(set_logY);

        pair.second -> SetTitle(pair.first.c_str());
        pair.second -> GetXaxis() -> SetTitle("nHits (per chip and per HitBco)");
        // pair.second -> SetStats(0);
        // pair.second -> SetMinimum(0);
        pair.second -> Draw("hist");
        c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), final_output_file_name.c_str()));
        c1 -> Clear();
    }

    c1 -> Print(Form("%s/%s.pdf)", output_directory.c_str(), final_output_file_name.c_str()));

    return 0;
}