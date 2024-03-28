#ifndef RAW_OFL_TEST_C
#define RAW_OFL_TEST_C

#include <intt/InttMapping.h>
#include <intt/InttMap.h>

// #include <intt/InttSurveyMap.h>
// #include <intt/InttSurveyMapv1.h>

#include <intt/InttFeeMap.h>
#include <intt/InttFeeMapv1.h>

R__LOAD_LIBRARY(libintt.so)

void print(InttMap::RawData_s const&);
void print(InttMap::Offline_s const&);

bool cmp(InttMap::RawData_s const&, InttNameSpace::RawData_s const&);
bool cmp(InttMap::Offline_s const&, InttNameSpace::Offline_s const&);

void raw_ofl_test() {
	std::unique_ptr<InttFeeMap> ifm = std::make_unique<InttFeeMapv1>();
	ifm->LoadFromFile("InttFeeMap.root");

	struct InttMap::RawData_s new_raw;
	struct InttMap::Offline_s new_ofl;

	struct InttNameSpace::RawData_s old_raw;
	struct InttNameSpace::Offline_s old_ofl;

	printf("Comparing rawdata => offline\n");
	new_raw.pid = 3001;
	new_raw.fee = 0;
	new_raw.chp = 0;
	new_raw.chn = 0;
	while(true) {
		old_raw.felix_server = new_raw.pid - 3001;
		old_raw.felix_channel = new_raw.fee;
		old_raw.chip = new_raw.chp;
		old_raw.channel = new_raw.chn;

		if(ifm->Convert(new_ofl, new_raw)) {
			printf("\tbad Convert\n");
			print(new_raw);
			break;
		}
		old_ofl = InttNameSpace::ToOffline(old_raw);

		if(!cmp(new_ofl, old_ofl)) {
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

	printf("Comparing offline => rawdata\n");
	new_ofl.layer = 3;
	new_ofl.ladder_phi = 0;
	new_ofl.ladder_z = 0;
	new_ofl.strip_z = 0;
	new_ofl.strip_phi = 0;
	while(true) {
		old_ofl.layer = new_ofl.layer;
		old_ofl.ladder_phi = new_ofl.ladder_phi;
		old_ofl.ladder_z = new_ofl.ladder_z;
		old_ofl.strip_y = new_ofl.strip_z;
		old_ofl.strip_x = new_ofl.strip_phi;

		if(ifm->Convert(new_raw, new_ofl)) {
			printf("\tbad Convert\n");
			print(new_ofl);
			break;
		}
		old_raw = InttNameSpace::ToRawData(old_ofl);

		if(!cmp(new_raw, old_raw)) {
			printf("\tbad cmp\n");
			print(new_ofl);
			break;
		}

		if(++new_ofl.strip_phi < 256)continue;
		new_ofl.strip_phi = 0;
		if(++new_ofl.strip_z < (new_ofl.ladder_z % 2 ? 5 : 8))continue;
		new_ofl.strip_z = 0;
		if(++new_ofl.ladder_z < 4)continue;
		new_ofl.ladder_z = 0;
		if(++new_ofl.ladder_phi < (new_ofl.layer < 5 ? 12 : 16))continue;
		new_ofl.ladder_phi = 0;
		if(++new_ofl.layer < 7)continue;
		new_ofl.layer = 3;

		break;
	}
	printf("done\n");
}

void print (
	InttMap::RawData_s const& raw
) {
	printf("\t%-16s%3d\n", "pid:",	raw.pid);
	printf("\t%-16s%3d\n", "fee:",	raw.fee);
	printf("\t%-16s%3d\n", "chp:",	raw.chp);
	printf("\t%-16s%3d\n", "chn:",	raw.chn);
}

void print (
	InttMap::Offline_s const& ofl
) {
	printf("\t%-16s%3d\n", "layer:",		ofl.layer);
	printf("\t%-16s%3d\n", "ladder_phi:",	ofl.ladder_phi);
	printf("\t%-16s%3d\n", "ladder_z:", 	ofl.ladder_z);
	printf("\t%-16s%3d\n", "strip_z:", 		ofl.strip_z);
	printf("\t%-16s%3d\n", "strip_phi:",	ofl.strip_phi);
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

bool cmp (
	InttMap::Offline_s const& new_ofl,
	InttNameSpace::Offline_s const& old_ofl
) {
	if(new_ofl.layer != old_ofl.layer)return false;
	if(new_ofl.ladder_phi != old_ofl.ladder_phi)return false;
	if(new_ofl.ladder_z != old_ofl.ladder_z)return false;
	if(new_ofl.strip_z != old_ofl.strip_y)return false;
	if(new_ofl.strip_phi != old_ofl.strip_x)return false;

	return true;
}


#endif//RAW_OFL_TEST_C
