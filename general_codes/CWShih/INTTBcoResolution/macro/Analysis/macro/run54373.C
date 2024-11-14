#include "../INTTBCOAna.h"

R__LOAD_LIBRARY(../libinttbcoana.so)

int run54373()
{
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54373/bco_diff/completed";
    string input_filename = "BcoDiffNtuple_54373_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_merged_1p14M.root";
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54373/bco_diff/completed/TimeResolutionAna";
    // string output_filename = "evt_BCODiff_QA_54373_2M.root";

    INTTBCOAna * inttbcoana = new INTTBCOAna(
        input_directory, 
        input_filename, 
        output_directory,
        30000,
        false,
        "",
        117
    );

    inttbcoana->ProcessEvents();
    inttbcoana->DrawPlots();
    // inttbcoana->SaveHistFile( output_directory + "/TimeResolutionAna.root");
    inttbcoana->EndRun();

    return 0;
}