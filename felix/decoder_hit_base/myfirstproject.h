#ifndef __MYFIRSTPROJECT_H__
#define __MYFIRSTPROJECT_H__

#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include "TFile.h"
#include "TTree.h"

extern TFile* file;
extern TTree* tree;

int setup(std::string const&);
int process_event (Event *e); //++CINT 
int module_map(std::string="/home/phnxrc/INTT/map_ladder/");

#endif /* __MYFIRSTPROJECT_H__ */
