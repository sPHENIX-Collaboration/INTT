#ifndef MASTER_C
#define MASTER_C

#include "defs.C"
#include "fit.C"
#include "train.C"
#include "apply.C"

void
master (
) {
	fit();
	std::cout << defs::mean  << std::endl;
	std::cout << defs::sigma << std::endl;
	train();
	apply();
}

#endif//MASTER_C
