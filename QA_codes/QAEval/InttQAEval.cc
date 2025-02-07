#include "InttQAEval.h"

#include <TFile.h>
#include <TH3D.h>
#include <TH2D.h>

#include <filesystem>
#include <regex>
#include <iostream>
#include <vector>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <phool/phool.h>
#include <phool/recoConsts.h>


//____________________________________________________________________________..
InttQAEval::InttQAEval()
{
}

//____________________________________________________________________________..
InttQAEval::~InttQAEval()
{
    if (_file)
    {
        _file->Close();
        delete _file;
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 14; ++j)
        {
            if (_hist2D[i][j])
            {
                delete _hist2D[i][j];
            }
        }
    }
}

void InttQAEval::LoadQAFileFromhtml(int runnumber)
{
    _runnumber = runnumber;
    if (_file)
    {
        _file->Close();
        delete _file;
        _file = nullptr;
    }

    std::string runnumber_str = std::to_string(runnumber);
    std::regex file_pattern(_inputbasefile + "_.*" + runnumber_str + ".*\\.root");

    for (const auto& entry : std::filesystem::directory_iterator(_inputdir))
    {
        if (entry.is_regular_file())
        {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, file_pattern))
            {
                std::string filepath = entry.path().string();
                _file = TFile::Open(filepath.c_str(), "READ");
                if (_file && _file->IsOpen())
                {
                    std::cout << "Loaded file: " << filepath << std::endl;
                    LoadFile();
                }
                else
                {
                    std::cerr << "Failed to open file: " << filepath << std::endl;
                    delete _file;
                    _file = nullptr;
                }
                break;
            }
        }
    }
}
void InttQAEval::DoInttQA()
{
    if (!_file || !_hasValidHist3D)
    {
        std::cout << "no INTT run" << std::endl;
        return;
    }

    // BCO QA
    bool intt_bco_diff_qa = DoBcoQA();
    if (intt_bco_diff_qa)
    {
        std::cout << "INTT BCO diff QA passed for Run " << _runnumber << std::endl;
    }
    else
    {
        std::cout << "INTT BCO diff QA failed for Run " << _runnumber << std::endl;
    }
    double goodchanratio = DoGoodChanQA();
    std::cout << "Good channel ratio: " << goodchanratio << std::endl;
}

bool InttQAEval::DoBcoQA()
{
    // get bco diff qa from cdb (must be 23 for INTT streaming mode)
    bool intt_bco_diff_qa = true;

    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", _runnumber);
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", _runnumber);

    std::string intt_bco_calib_dir = CDBInterface::instance()->getUrl("INTT_BCOMAP");
    if (intt_bco_calib_dir.empty())
    {
        std::cout << "No INTT BCOMAP for Run " << _runnumber << std::endl;
        intt_bco_diff_qa = false;
    }
    else
    {
        CDBTTree *cdbttree = new CDBTTree(intt_bco_calib_dir);
        cdbttree->LoadCalibrations();
        uint64_t N = cdbttree->GetSingleIntValue("size");
        // CDBTTree has the branch for telling you the runmode. 0 : Trigger, 1 : Streaming
        int runmode = cdbttree->GetSingleIntValue("runmode");
        // Stdandard deviation of all fees' BCO diff. should be 0 if all felixs are properly aligned.
        // Masking or dead fees are excluded in the calculation
        double StdDev = cdbttree->GetSingleDoubleValue("StdDev");
        if (runmode == 1)
        {
            for (uint64_t n = 0; n < N; ++n)
            {
                int bco_diff = cdbttree->GetIntValue(n, "bco_diff");
                if (!(bco_diff == _streaming_bco || bco_diff == -1))
                {
                    intt_bco_diff_qa = false;
                    break;
                }
            }
        }
        else
        {
            if (StdDev != 0)
                intt_bco_diff_qa = false;
        }
    }
    return intt_bco_diff_qa;
}

double InttQAEval::DoGoodChanQA()
{
    // get good channel QA from cdb
    // bool intt_good_chan_qa = true;
    double goodchanratio = 0.;
    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", _runnumber);
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", _runnumber);
    uint64_t N = 0;
    std::string intt_good_chan_dir = CDBInterface::instance()->getUrl("INTT_HotMap");
    if (intt_good_chan_dir.empty())
    {
        std::cout << "No INTT GOODCHAN for Run " << _runnumber << std::endl;
        // intt_good_chan_qa = false;
    }
    else
    {
        CDBTTree *cdbttree = new CDBTTree(intt_good_chan_dir);
        cdbttree->LoadCalibrations();
        N = cdbttree->GetSingleIntValue("size");
        // for (uint64_t n = 0; n < N; ++n)
        // {
        //     int chip_id = cdbttree->GetIntValue(n, "chip_id");
        //     int chan_id = cdbttree->GetIntValue(n, "chan_id");
        //     int status = cdbttree->GetIntValue(n, "status");
        //     if (status != 1)
        //     {
        //         intt_good_chan_qa = false;
        //         break;
        //     }
        // }
    }
    std::cout<<"Number of bad channel : "<<N<<std::endl;
    goodchanratio = (1.0 - (double)N / (128 * 26 * 14 * 8)) * 100;
    return goodchanratio;
}

void InttQAEval::LoadFile()
{
    _hasValidHist3D = false;

    if (_file && _file->IsOpen())
    {
        if(_debug)
        {
            _file->Print();
            _file->ls();
        }

        // h_InttRawHitQA_intt{0~7} 
        for (int i = 0; i < 8; ++i)
        {
            std::string hist_name = "h_InttRawHitQA_intt" + std::to_string(i);
            TH3D* hist3D = dynamic_cast<TH3D*>(_file->Get(hist_name.c_str()));
            if (hist3D)
            {
                CreateTH2DFromTH3D(hist3D, i);
                _hasValidHist3D = true;
            }
            else
            {
                std::cerr << "Failed to load histogram: " << hist_name << std::endl;
            }
        }
    }
}

void InttQAEval::SaveLoadedFile()
{
    TFile* new_file = TFile::Open("saved_TH2D.root", "RECREATE");
    if (!new_file || !new_file->IsOpen())
    {
        std::cerr << "Failed to create new file for saving TH2D objects." << std::endl;
        return;
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 14; ++j)
        {
            if (_hist2D[i][j])
            {
                _hist2D[i][j]->Write();
            }
        }
    }

    new_file->Close();
    delete new_file;
}

void InttQAEval::CreateTH2DFromTH3D(TH3D* hist3D, int intt_index)
{
    int x_bins = hist3D->GetXaxis()->GetNbins();  
    int y_bins = hist3D->GetYaxis()->GetNbins();
    int z_bins = hist3D->GetZaxis()->GetNbins();

    double y_min = hist3D->GetYaxis()->GetXmin();
    double y_max = hist3D->GetYaxis()->GetXmax();
    double z_min = hist3D->GetZaxis()->GetXmin();
    double z_max = hist3D->GetZaxis()->GetXmax();

    for (int xbin = 1; xbin <= x_bins; xbin++)
    {
        TString hist2D_name = Form("h_InttRawHitQA_intt%d_fee%d", intt_index, xbin - 1);
        _hist2D[intt_index][xbin - 1] = new TH2D(hist2D_name, hist2D_name+";chip_id;chan_id", y_bins, y_min, y_max, z_bins, z_min, z_max);

        TH1D* tempYZ = hist3D->ProjectionY("tempYZ", xbin, xbin);

        for (int ybin = 1; ybin <= y_bins; ybin++)
        {
            for (int zbin = 1; zbin <= z_bins; zbin++)
            {
                double content = hist3D->GetBinContent(xbin, ybin, zbin);
                _hist2D[intt_index][xbin - 1]->SetBinContent(ybin, zbin, content);
            }
        }

        delete tempYZ;
    }
}
