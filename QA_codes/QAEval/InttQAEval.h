// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTQAEVAL_H
#define INTTQAEVAL_H

#include <string>
#include <vector>

class TFile;
class TH3D;
class TH2D;

class InttQAEval
{
public:
    InttQAEval();
    ~InttQAEval();

    void SetQAhtmlInputDir(std::string path = "/sphenix/data/data02/sphnxpro/QAhtml/aggregated") { _inputdir = path; };
    void SetInputBaseFileName(std::string name = "HIST_DST_TRKR_HIT") { _inputbasefile = name; };
    void LoadQAFileFromhtml(int runnumber);
    void LoadFile();
    void SaveLoadedFile();
    void SetDebug(bool debug) { _debug = debug; };


    void DoInttQA(); 
    //BCO QA
    bool DoBcoQA();
    double DoGoodChanQA();
private:
    std::string _inputdir = "/sphenix/data/data02/sphnxpro/QAhtml/aggregated";
    std::string _inputbasefile = "HIST_DST_TRKR_HIT";
    TFile* _file = nullptr;
    bool _debug = false;
    const int _streaming_bco = 23;
    int _runnumber;    
    TH2D* _hist2D[8][14] = {nullptr};
    bool _hasValidHist3D = false;

    void CreateTH2DFromTH3D(TH3D* hist3D, int felix_index);
};

#endif // INTTQAEVAL_H
