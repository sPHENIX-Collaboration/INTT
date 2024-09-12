#ifndef BCO_ANA_V3_H
#define BCO_ANA_V3_H

#include "intt_event_pool.h"
#include <Event/fileEventiterator.h>

class bco_ana_v3 : public intt_event_pool {
public:
	bco_ana_v3();
	virtual ~bco_ana_v3();

	int analyze(Packet*) override;
	int analyze() override;

private:

};

#endif//BCO_ANA_V3_H
