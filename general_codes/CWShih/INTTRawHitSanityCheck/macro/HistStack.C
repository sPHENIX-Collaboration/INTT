int HistStack()
{
    std::string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_CW_Mar252025_INTTQA/completed";
    std::string input_file_name = "BcoDiffNtuple_54280_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit";
    int Nfiles = 800;

    std::string output_directory = input_directory;
    std::string output_file_name = "HistStack_withCorrBack.root";

    // std::vector<std::string> h1D_name_vec = {
    //     "all_felix_BcoDiffHist_0",
    //     "all_felix_BcoDiffHist_1",
    //     "all_felix_BcoDiffHist_2",
    //     "all_felix_BcoDiffHist_3",
    //     "all_felix_BcoDiffHist_4",
    //     "all_felix_BcoDiffHist_5",
    //     "all_felix_BcoDiffHist_6",
    //     "all_felix_BcoDiffHist_7",
    //     "h1D_SemiClusPhiSize",
    //     "h1D_nChipHit_map_stacked"
    // };

    // std::vector<std::string> h2D_name_vec = {
    //     "h2D_NInttHitInner_NInttHitOuter",
    //     "h2D_NInttHit_TrigSpikeSize"
    // };

    std::vector<std::string> h1D_name_vec; h1D_name_vec.clear();
    std::vector<std::string> h2D_name_vec; h2D_name_vec.clear();

    std::vector<THStack*> h1Dstack_vec; h1Dstack_vec.clear();
    std::vector<THStack*> h2Dstack_vec; h2Dstack_vec.clear();



    for (int i = 0; i < Nfiles; i++)
    {
        
        std::string job_index = std::to_string( i );
        int job_index_len = 5;
        job_index.insert(0, job_index_len - job_index.size(), '0');

        TFile* f = TFile::Open(Form("%s/%s_%s.root", input_directory.c_str(), input_file_name.c_str(), job_index.c_str()));
        if (!f->IsOpen())
        {
            std::cout << "Error: cannot open file " << Form("%s/%s_%s.root", input_directory.c_str(), input_file_name.c_str(), job_index.c_str()) << std::endl;
            return -1;
        }

        std::cout << "Processing file " << Form("%s/%s_%s.root", input_directory.c_str(), input_file_name.c_str(), job_index.c_str()) << std::endl;
        

        if (i == 0){
            
            for (TObject* keyAsObj : *f->GetListOfKeys())
            {
                auto key = dynamic_cast<TKey*>(keyAsObj);
                std::string hist_name  = key->GetName();
                std::string class_name = key->GetClassName();

                if (class_name == "TH1D")
                {                   
                    h1D_name_vec.push_back(hist_name);
                    h1Dstack_vec.push_back(new THStack(Form("hstack_%s", hist_name.c_str()), Form("hstack_%s", hist_name.c_str())));
                }
                else if (class_name == "TH2D")
                {
                    h2D_name_vec.push_back(hist_name);
                    h2Dstack_vec.push_back(new THStack(Form("hstack_%s", hist_name.c_str()), Form("hstack_%s", hist_name.c_str())));
                }
            }
        }


        for (int j = 0; j < h1D_name_vec.size(); j++)
        {
            TH1D* h = (TH1D*)f->Get(h1D_name_vec[j].c_str());
            if (h == nullptr)
            {
                std::cout << "Error: cannot find histogram " << h1D_name_vec[j] << " in file " << Form("%s/%s_%s.root", input_directory.c_str(), input_file_name.c_str(), job_index.c_str()) << std::endl;
                return -1;
            }
            h1Dstack_vec[j]->Add(h);
        }

        for (int j = 0; j < h2D_name_vec.size(); j++)
        {
            TH2D* h = (TH2D*)f->Get(h2D_name_vec[j].c_str());
            if (h == nullptr)
            {
                std::cout << "Error: cannot find histogram " << h2D_name_vec[j] << " in file " << Form("%s/%s_%s.root", input_directory.c_str(), input_file_name.c_str(), job_index.c_str()) << std::endl;
                return -1;
            }
            h2Dstack_vec[j]->Add(h);
        }

        // f->Close();
    }

    TFile *fout = new TFile(Form("%s/%s", output_directory.c_str(), output_file_name.c_str()), "RECREATE");
    for (int i = 0; i < h2Dstack_vec.size(); i++)
    {
        ((TH2D*)h2Dstack_vec[i]->GetStack()->Last()) -> Write(Form("h2Dstack_%s", h2D_name_vec[i].c_str()));
    }
    
    for (int i = 0; i < h1Dstack_vec.size(); i++)
    {
        ((TH1D*)h1Dstack_vec[i]->GetStack()->Last()) -> Write(Form("h1Dstack_%s", h1D_name_vec[i].c_str()));
    }

    

    fout->Close();

    return 0;
}