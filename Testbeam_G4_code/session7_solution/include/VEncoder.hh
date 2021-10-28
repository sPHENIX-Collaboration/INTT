#ifndef VEncoder_h
#define VEncoder_h 1

#include "Control.hh"
#include "CGADefs.h"
#include <string>

class CalHit;
 
class VEncoder {
public:
        VEncoder(G4bool flag): id1Flag(flag) {}
        virtual ~VEncoder() {}

	G4bool getID1Flag(void) {return id1Flag;}

	std::string getIDString(void) { return idString;}

	virtual cell_ids encode(G4int, G4int, G4int, G4int, G4int, G4int)=0;

	virtual CalHit* decode(cell_ids)=0;

protected:
	std::string idString;
	G4bool id1Flag;
	cell_ids index;
};

#endif // VEncoder_h
