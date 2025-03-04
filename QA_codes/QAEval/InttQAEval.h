// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTQAEVAL_H
#define INTTQAEVAL_H

#include <string>
#include <vector>

class TFile;
class TH3D;
class TH2D;
class TTree;
namespace odbc
{
  class Connection;
  class Statement;
  class ResultSet;
};  // namespace odbc

class InttQAEval
{
public:
    InttQAEval();
    ~InttQAEval();

    void SetQAhtmlInputDir(std::string path = "/sphenix/data/data02/sphnxpro/QAhtml/aggregated") { _inputdir = path; };
    void SetInputBaseFileName(std::string name = "HIST_DST_TRKR_HIT") { _inputbasefile = name; };
    void LoadQAFileFromhtml();
    void LoadFile();
    void SaveLoadedFile();
    void SetDebug(bool debug) { _debug = debug; };
    void SetUseHtml(bool usehtml) { _useHtml = usehtml; };
    void SetRunNumber(int runnumber) { _runnumber = runnumber; };
    int DoInttQA();
    void SaveTreeToFile(const std::string& filename);
    int getTimeDifferenceInSeconds(const std::string& brtimestamp_str, const std::string& ertimestamp_str); 
    // BCO QA
    int DoBcoQA();
    double DoGoodChanQA();
    int FetchODBCInfo();

private:
    int GetConnection();
    std::string dbname{"daq"};
    std::string dbowner{"phnxrc"};
    std::string dbpasswd{""};
    std::string table{"run"};

    std::string _inputdir = "/sphenix/data/data02/sphnxpro/QAhtml/aggregated";
    std::string _inputbasefile = "HIST_DST_TRKR_HIT";
    TFile *_file = nullptr;
    bool _debug = false;
    const int _streaming_bco = 23;
    int _runnumber = -1;
    double _goodchanratio = -1;
    double _goodchanratio_wo_deadch = -1;
    int _intt_bco_diff_qa = -999;
    TH2D *_hist2D[8][14] = {nullptr};
    bool _hasValidHist3D = false;
    bool _useHtml = false;
    void CreateTH2DFromTH3D(TH3D *hist3D, int felix_index);
    odbc::Connection *con = nullptr;
    int _runtime;
    int _runmode;
    int _nevents; 
    int _N_dead;
    int _N_cold;
    int _N_hot;
    TTree* tree = nullptr; 
};

#endif // INTTQAEVAL_H
