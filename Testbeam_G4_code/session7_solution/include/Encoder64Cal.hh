#ifndef Encoder64Cal_h
#define Encoder64Cal_h 1
                                                                                
#include "CGADefs.h"
#include "VEncoder.hh"

class CalHit;

class Encoder64Cal: public VEncoder {
public:
        Encoder64Cal(void);

	cell_ids encode(G4int, G4int, G4int, G4int, G4int, G4int=0);

	CalHit* decode(cell_ids);
};

#endif // Encoder64Cal_h
