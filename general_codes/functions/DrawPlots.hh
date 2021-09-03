#pragma once

// Review plots are made using a TTree in the given ROOT file
void DrawPlots(string root_file, int usemod, string mode);

// no infinit loop happen, I think
#include "DrawPlots.cc"
