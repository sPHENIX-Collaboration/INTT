#pragma once

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
		   vector < int > *event    );

#ifdef __CINT__
#include "RemoveThisHit.cc"
#endif // __CINT__
