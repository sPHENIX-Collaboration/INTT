#include "../INTTBCOAna.h"

R__LOAD_LIBRARY(../libinttbcoana.so)

int run54280()
{
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/From_official_INTTRAWHIT_DST/BCO_check/completed";
    string input_filename = "BcoDiffNtuple_54280_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_merged2M.root";
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/From_official_INTTRAWHIT_DST/BCO_check/completed/TimeResolutionAna";
    string output_filename = "evt_BCODiff_QA_54280_2M.root";

    INTTBCOAna * inttbcoana = new INTTBCOAna(
        input_directory, 
        input_filename, 
        output_directory,
        -1,
        true,
        output_filename
    );

    inttbcoana->ProcessEvents();
    // inttbcoana->DrawPlots();
    // inttbcoana->SaveHistFile( output_directory + "/TimeResolutionAna.root");
    inttbcoana->EndRun();

    return 0;
}