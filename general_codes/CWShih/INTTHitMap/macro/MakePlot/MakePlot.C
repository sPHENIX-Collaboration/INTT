int MakePlot()
{
    bool is_dark = false;
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/HitMap/HotChMap_Oct25_2024/completed";
    string input_filename = "INTTHitMap_BadChannelMasked_BcoDiffApplied_HitQA_CloneHitRemovedBCO_00054280_merged.root";
    string output_directory = input_directory;
    string final_output_file_name = input_filename.substr(0, input_filename.find(".root"));
    

    if (is_dark) {
        gStyle->SetPalette(kInvertedDarkBodyRadiator);

        final_output_file_name += "_darkPalette";
    }

    std::map<string, TH2D*> data_hist_h2_map; data_hist_h2_map.clear();

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

        if (class_name == "TH2D")
        {
            data_hist_h2_map[hist_name.c_str()] = (TH2D*) file_in -> Get( hist_name.c_str() );
        }
    }

    TCanvas * c1 = new TCanvas("c1", "c1", 1200, 600);
    c1 -> Print(Form("%s/%s.pdf(", output_directory.c_str(), final_output_file_name.c_str()));

    for (auto &pair : data_hist_h2_map)
    {
        pair.second -> SetTitle(pair.first.c_str());
        pair.second -> SetStats(0);
        pair.second -> SetMinimum(0);
        pair.second -> Draw("colz");
        c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), final_output_file_name.c_str()));
        c1 -> Clear();
    }

    c1 -> Print(Form("%s/%s.pdf)", output_directory.c_str(), final_output_file_name.c_str()));

    return 0;
}