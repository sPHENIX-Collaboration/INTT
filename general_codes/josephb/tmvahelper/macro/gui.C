#ifndef GUI_C
#define GUI_C

#include "config.C"

void
gui (
) {
	if (!gROOT->IsBatch()) TMVA::TMVAGui(config::factory_file_name.c_str());
}

#endif//GUI_C
