#ifndef __CONVERTINTTDATA_H__
#define __CONVERTINTTDATA_H__

#include <Event/Event.h>
#include <Event/EventTypes.h>

int Init (const char* outRootfile); //++CINT 
//int Process_event (Event *e); //++CINT 
int Run (const char* evtfile, ULong64_t hit_num_limit=-1 ); //++CINT 
int module_map( std::string="/sphenix/tg/tg01/commissioning/INTT/map_ladder/" );

#endif /* __CONVERTINTTDATA_H__ */
