#ifndef TEST_C
#define TEST_C

#include <climits>
#include <cstddef>
#include <cstdint>

#include <memory>
#include <set>
#include <random>
#include <vector>

#include <TFile.h>
#include <TTree.h>

R__LOAD_LIBRARY(libintt_josephb.so)
//#include <intt_josephb/InttHitJb/InttHitJb.h>
//#include <intt_josephb/InttHitJb/InttHitJbVer1.h>

void Test()
{
	// std::unique_ptr<InttHitJb> hit0 = std::make_unique<InttHitJb>();
	// std::unique_ptr<InttHitJb> hit1 = std::make_unique<InttHitJbVer1>();

	// hit0->print();
	// hit1->print();
}

#endif
