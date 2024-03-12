#ifndef ONL_RAW_TEST_C
#define ONL_RAW_TEST_C

#include <intt/InttMapping.h>
#include <intt/InttMap.h>

// #include <intt/InttSurveyMap.h>
// #include <intt/InttSurveyMapv1.h>

#include <intt/InttFeeMap.h>
#include <intt/InttFeeMapv1.h>

R__LOAD_LIBRARY(libintt.so)

void print(InttMap::Online_s const&);
void print(InttMap::RawData_s const&);

bool cmp(InttMap::Online_s const&, InttNameSpace::Online_s const&);
bool cmp(InttMap::RawData_s const&, InttNameSpace::RawData_s const&);

void onl_raw_test() {
	std::unique_ptr<InttFeeMap> ifm = std::make_unique<InttFeeMapv1>();
	ifm->LoadFromFile("InttFeeMap.root");

	struct InttMap::Online_s new_onl;
	struct InttMap::RawData_s new_raw;

	struct InttNameSpace::Online_s old_onl;
	struct InttNameSpace::RawData_s old_raw;

	printf("Comparing online => rawdata\n");
	new_onl.lyr = 0;
	new_onl.ldr = 0;
	new_onl.arm = 0;
	new_onl.chp = 0;
	new_onl.chn = 0;
	while(true) {
		old_onl.lyr = new_onl.lyr;
		old_onl.ldr = new_onl.ldr;
		old_onl.arm = new_onl.arm;
		old_onl.chp = new_onl.chp;
		old_onl.chn = new_onl.chn;

		if(ifm->Convert(new_onl, new_raw)) {
			printf("\tbad Convert\n");
			print(new_onl);
			break;
		}
		old_raw = InttNameSpace::ToRawData(old_onl);

		if(!cmp(new_raw, old_raw)) {
			printf("\tbad cmp\n");
			print(new_onl);
			break;
		}

		if(++new_onl.chn < 128)continue;
		new_onl.chn = 0;
		if(++new_onl.chp < 26)continue;
		new_onl.chp = 0;
		if(++new_onl.arm < 2)continue;
		new_onl.arm = 0;
		if(++new_onl.ldr < (new_onl.lyr < 2 ? 12 : 16))continue;
		new_onl.ldr = 0;
		if(++new_onl.lyr < 4)continue;
		new_onl.lyr = 0;

		break;
	}
	printf("done\n");

	printf("Comparing rawdata => online\n");
	new_raw.pid = 3001;
	new_raw.fee = 0;
	new_raw.chp = 0;
	new_raw.chn = 0;
	while(true) {
		old_raw.felix_server = new_raw.pid - 3001;
		old_raw.felix_channel = new_raw.fee;
		old_raw.chip = new_raw.chp;
		old_raw.channel = new_raw.chn;

		if(ifm->Convert(new_raw, new_onl)) {
			printf("\tbad Convert\n");
			print(new_raw);
			break;
		}
		old_onl = InttNameSpace::ToOnline(old_raw);

		if(!cmp(new_onl, old_onl)) {
			printf("\tbad cmp\n");
			print(new_raw);
			break;
		}

		if(++new_raw.chn < 128)continue;
		new_raw.chn = 0;
		if(++new_raw.chp < 26)continue;
		new_raw.chp = 0;
		if(++new_raw.fee < 14)continue;
		new_raw.fee = 0;
		if(++new_raw.pid < 3009)continue;
		new_raw.pid = 3001;

		break;
	}
	printf("done\n");
}

void print (
	InttMap::Online_s const& onl
) {
	printf("\t%-16s%3d\n", "lyr:",	onl.lyr);
	printf("\t%-16s%3d\n", "ldr:",	onl.ldr);
	printf("\t%-16s%3d\n", "arm:", 	onl.arm);
	printf("\t%-16s%3d\n", "chp:", 	onl.chp);
	printf("\t%-16s%3d\n", "chn:",	onl.chn);
}

void print (
	InttMap::RawData_s const& raw
) {
	printf("\t%-16s%3d\n", "pid:",	raw.pid);
	printf("\t%-16s%3d\n", "fee:",	raw.fee);
	printf("\t%-16s%3d\n", "chp:",	raw.chp);
	printf("\t%-16s%3d\n", "chn:",	raw.chn);
}

bool cmp (
	InttMap::Online_s const& new_onl,
	InttNameSpace::Online_s const& old_onl
) {
	if(new_onl.lyr != old_onl.lyr)return false;
	if(new_onl.ldr != old_onl.ldr)return false;
	if(new_onl.arm != old_onl.arm)return false;
	if(new_onl.chp != old_onl.chp)return false;
	if(new_onl.chn != old_onl.chn)return false;

	return true;
}

bool cmp (
	InttMap::RawData_s const& new_raw, 
	InttNameSpace::RawData_s const& old_raw 
) {
	if(new_raw.pid != old_raw.felix_server + 3001)return false;
	if(new_raw.fee != old_raw.felix_channel)return false;
	if(new_raw.chp != old_raw.chip)return false;
	if(new_raw.chn != old_raw.channel)return false;

	return true;
}

#endif//ONL_RAW_TEST_C
