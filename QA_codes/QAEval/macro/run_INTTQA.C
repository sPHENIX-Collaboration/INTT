#include <InttQAEval.h>

R__LOAD_LIBRARY(libInttQAEval.so)

void run_INTTQA()
{
    InttQAEval *eval = new InttQAEval();
    eval->SetDebug(false);
    eval->SetUseHtml(false);
//    eval->SetQAhtmlInputDir("/sphenix/data/data02/sphnxpro/QAhtml/aggregated");
//    eval->SetInputBaseFileName("HIST_DST_TRKR_HIT");
    // eval->SetRunNumber(40874);
    // eval->FetchODBCInfo();    
for (int i = 40874; i < 55030; i++)
    {
        eval->SetRunNumber(i);
        eval->DoInttQA();
    }
    eval->SaveTreeToFile("InttQAEval_2.root");
}