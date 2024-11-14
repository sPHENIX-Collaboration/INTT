#ifndef __CONVERTINTTDATA_H__
#define __CONVERTINTTDATA_H__

#include <Event/Event.h>
#include <Event/EventTypes.h>

int Init (const char* outRootfile); //++CINT 
//int Process_event (Event *e); //++CINT 
int Run (const char* evtfile); //++CINT 
int module_map(std::string="/home/phnxrc/INTT/map_ladder/");

#endif /* __CONVERTINTTDATA_H__ */
