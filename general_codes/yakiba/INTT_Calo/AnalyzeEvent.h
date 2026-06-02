#ifndef ANALYZEEVENT_H
#define ANALYZEEVENT_H

#include <TH1F.h>

void AnalyzeInit(void);
void AnalyzeEnd(void);
void AnalyzeEvent(void);

//Histogram files
TH1F *hzgvtx;
TH1F *hINTTdphi;




#endif
