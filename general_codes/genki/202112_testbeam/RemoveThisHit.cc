#include "RemoveThisHit.hh"

/*!
  @fn void RemoveThisHit
  @brief Element at index in the vector arguments is removed
*/
void RemoveThisHit(
		   int index, 
		   vector < int > *camac_adc,
		   vector < int > *camac_tdc,
		   vector < int > *adc      ,
		   vector < int > *ampl     ,
		   vector < int > *chip_id  ,
		   vector < int > *fpga_id  ,
		   vector < int > *module   ,
		   vector < int > *chan_id  ,
		   vector < int > *fem_id   ,
		   vector < int > *bco      ,
		   vector < int > *bco_full ,
		   vector < int > *event    )
{
  camac_adc->erase	( camac_adc->begin() + index	);
  camac_tdc->erase	( camac_tdc->begin() + index	);
  adc->erase		( adc->begin() + index		);
  ampl->erase		( ampl->begin() + index		);
  chip_id->erase	( chip_id->begin() + index	);
  fpga_id->erase	( fpga_id->begin() + index	);
  module->erase		( module->begin() + index	);
  chan_id->erase	( chan_id->begin() + index	);
  fem_id->erase		( fem_id->begin() + index	);
  bco->erase		( bco->begin() + index		);
  bco_full->erase	( bco_full->begin() + index	);
  event->erase		( event->begin() + index	);
}
