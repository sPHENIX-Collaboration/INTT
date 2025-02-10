#include "InttQAEval.h"

#include <TFile.h>
#include <TH3D.h>
#include <TH2D.h>
#include <TTree.h>
#include <filesystem>
#include <regex>
#include <iostream>
#include <vector>

#include <odbc++/connection.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>
#include <odbc++/resultsetmetadata.h>

#include <boost/format.hpp>

#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>

#include <phool/phool.h>
#include <phool/recoConsts.h>

//____________________________________________________________________________..
InttQAEval::InttQAEval()
{
    tree = new TTree("tree", "tree");
    tree->Branch("runnumber", &_runnumber, "runnumber/I");
    tree->Branch("runtime", &_runtime, "runtime/D");
    tree->Branch("runmode", &_runmode, "runmode/I");
    tree->Branch("goodchanratio", &_goodchanratio, "goodchanratio/D");
    tree->Branch("intt_bco_diff_qa", &_intt_bco_diff_qa, "intt_bco_diff_qa/I");
    GetConnection();
}

//____________________________________________________________________________..
InttQAEval::~InttQAEval()
{
    delete tree;
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

    if (con)
    {
        delete con;
    }
}

void InttQAEval::LoadQAFileFromhtml()
{
    if (_file)
    {
        _file->Close();
        delete _file;
        _file = nullptr;
    }

    std::string runnumber_str = std::to_string(_runnumber);
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
int InttQAEval::DoInttQA()
{
    if (_runnumber < 0)
    {
        std::cout << "No run number is set for INTT QA" << std::endl;
        std::cout << "Set Runnumber with SetRunNumber(int runnumber)" << std::endl;
        return -999;
    }
    
    if (_useHtml)
    {
        if (!_file)
        {
            std::cout << "No html file is used for INTT QA" << std::endl;
            return -999;
        }
        if (!_hasValidHist3D)
        {
            std::cout << "No INTT html QA histograms in the File" << std::endl;
            return -999;
        }
    }
    int ODBCflag = FetchODBCInfo();
    if(ODBCflag != 1) 
    {
        std::cout << "No ODBC info for Run " << _runnumber << std::endl;
        return -999;
    }
    _goodchanratio = DoGoodChanQA();
    std::cout << "Run number: " << _runnumber << std::endl;
    std::cout << "Good channel ratio: " << _goodchanratio << std::endl;
    if (_goodchanratio < 0)
    {
        std::cout << "No INTT GOODCHAN for Run " << _runnumber << std::endl;
        return -999;
    }
    // BCO QA
    _intt_bco_diff_qa = DoBcoQA();
    if (_intt_bco_diff_qa==1)
    {
        std::cout << "INTT BCO diff QA passed for Run " << _runnumber << std::endl;
    }
    else
    {
        std::cout << "INTT BCO diff QA failed for Run " << _runnumber << std::endl;
    }

    tree->Fill();

    return 0;
}

int InttQAEval::DoBcoQA()
{
    // get bco diff qa from cdb (must be 23 for INTT streaming mode)
    int intt_bco_diff_qa = 1;

    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", _runnumber);
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", _runnumber);

    std::string intt_bco_calib_dir = CDBInterface::instance()->getUrl("INTT_BCOMAP");
    if (intt_bco_calib_dir.empty())
    {
        std::cout << "No INTT BCOMAP for Run " << _runnumber << std::endl;
        intt_bco_diff_qa = 0;
    }
    else
    {
        if(_debug)
        {
            std::cout<<"INTT BCO MAP : "<<intt_bco_calib_dir<<std::endl;
        }
        CDBTTree *cdbttree = new CDBTTree(intt_bco_calib_dir);
        cdbttree->LoadCalibrations();
        uint64_t N = cdbttree->GetSingleIntValue("size");
        // CDBTTree has the branch for telling you the runmode. 0 : Trigger, 1 : Streaming
        _runmode = cdbttree->GetSingleIntValue("runmode");
        // Stdandard deviation of all fees' BCO diff. should be 0 if all felixs are properly aligned.
        // Masking or dead fees are excluded in the calculation
        double StdDev = cdbttree->GetSingleDoubleValue("StdDev");
        if (_runmode == 1)
        {
            for (uint64_t n = 0; n < N; ++n)
            {
                int bco_diff = cdbttree->GetIntValue(n, "bco_diff");
                if (!(bco_diff == _streaming_bco || bco_diff == -1))
                {
                    intt_bco_diff_qa = 0;
                    break;
                }
            }
        }
        else
        {
            if (StdDev != 0)
                intt_bco_diff_qa = 0;
        }
        delete cdbttree; 
    }
    return intt_bco_diff_qa;
}

double InttQAEval::DoGoodChanQA()
{
    // get good channel QA from cdb
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
        return -1;
    }
    else
    {
        // Check if the directory contains 'run'
        if (intt_good_chan_dir.find("run") == std::string::npos)
        {
            if (_debug)
                std::cerr << "File is not run by run calibration : " << intt_good_chan_dir << std::endl;
            return -1;
        }

        CDBTTree *cdbttree = new CDBTTree(intt_good_chan_dir);
        if (_debug)
            std::cout << intt_good_chan_dir << std::endl;
        cdbttree->LoadCalibrations();
        N = cdbttree->GetSingleIntValue("size");
        // for (uint64_t n = 0; n < N; ++n)
        // {
        //     int chip_id = cdbttree->GetIntValue(n, "chip_id");
        //     int chan_id = cdbttree->GetIntValue(n, "chan_id");
        //     int status = cdbttree->GetIntValue(n, "status");
        //     if (status != 1)
        //     {
        //         // Handle non-good channels
        //     }
        // }
        delete cdbttree;
    }
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

int InttQAEval::GetConnection()
{
  if (con)
  {
    return 0;
  }
  try
  {
    con = odbc::DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
  }
  catch (odbc::SQLException &e)
  {
    std::cout << " Exception caught during DriverManager::getConnection" << std::endl;
    std::cout << "Message: " << e.getMessage() << std::endl;
    if (con)
    {
      delete con;
      con = nullptr;
    }
    return -1;
  }
  std::cout << "opened DB connection" << std::endl;
  return 0;
}

int InttQAEval::FetchODBCInfo()
{
    if (GetConnection() != 0)
    {
        return -1;
    }

    std::stringstream cmd;
    cmd << "SELECT * FROM " << table << " WHERE runnumber = " << _runnumber << ";";
    odbc::Statement *stmt = con->createStatement();
    odbc::ResultSet *rs = nullptr;

    try
    {
        rs = stmt->executeQuery(cmd.str());
    }
    catch (odbc::SQLException &e)
    {
        std::cout << (boost::format("Error: %s.\n") % e.getMessage().c_str()).str();
        delete rs;
        delete stmt;
        return -999;
    }

    if (rs->next() == 0)
    {
        std::cout << (boost::format("Error : Can't find data for run %d \n") % _runnumber).str();
        delete rs;
        delete stmt;
        return -999;
    }

    int ncol = rs->getMetaData()->getColumnCount();
    for (int icol = 0; icol < ncol; icol++)
    {
        std::string col_name = rs->getMetaData()->getColumnName(icol + 1);
        std::string cont = rs->getString(col_name);
        std::cout << (boost::format("%2d : %s = %s\n") % icol % col_name % cont).str();

        if (col_name == "runtime")
        {
            _runtime = std::stod(cont);
        }
    }

    std::cout << "Runtime value: " << _runtime << std::endl;

    delete rs;
    delete stmt;
    return 1;
}

void InttQAEval::SaveTreeToFile(const std::string& filename)
{
    TFile* outputFile = TFile::Open(filename.c_str(), "RECREATE");
    if (!outputFile || !outputFile->IsOpen())
    {
        std::cerr << "Failed to create output file for TTree." << std::endl;
        return;
    }

    tree->Write();
    outputFile->Close();
    delete outputFile;
}