#include <InttQAEval.h>

R__LOAD_LIBRARY(libInttQAEval.so)

void run_INTTQA()
{
    InttQAEval *eval = new InttQAEval();
    // eval->SetDebug(true);
    eval->SetQAhtmlInputDir("/sphenix/data/data02/sphnxpro/QAhtml/aggregated");
    eval->SetInputBaseFileName("HIST_DST_TRKR_HIT");
    for(int i=47220;i<48000;i++)
    {
        eval->LoadQAFileFromhtml(i);
//        eval->SaveLoadedFile();
        eval->DoInttQA();
    }
}