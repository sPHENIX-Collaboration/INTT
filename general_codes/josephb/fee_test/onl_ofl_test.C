#ifndef ONL_OFL_TEST_C
#define ONL_OFL_TEST_C

#include <intt/InttMapping.h>
#include <intt/InttMap.h>

// #include <intt/InttSurveyMap.h>
// #include <intt/InttSurveyMapv1.h>

#include <intt/InttFeeMap.h>
#include <intt/InttFeeMapv1.h>

R__LOAD_LIBRARY(libintt.so)

void print(InttMap::Online_s const&);
void print(InttMap::Offline_s const&);

bool cmp(InttMap::Online_s const&, InttNameSpace::Online_s const&);
bool cmp(InttMap::Offline_s const&, InttNameSpace::Offline_s const&);

void onl_ofl_test() {
	std::unique_ptr<InttFeeMap> ifm = std::make_unique<InttFeeMapv1>();

	struct InttMap::Online_s new_onl;
	struct InttMap::Offline_s new_ofl;

	struct InttNameSpace::Online_s old_onl;
	struct InttNameSpace::Offline_s old_ofl;

	printf("Comparing online => offline\n");
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

		if(ifm->Convert(new_onl, new_ofl)) {
			printf("\tbad Convert\n");
			print(new_onl);
			break;
		}
		old_ofl = InttNameSpace::ToOffline(old_onl);

		if(!cmp(new_ofl, old_ofl)) {
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

	printf("Comparing offline => online\n");
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

		if(ifm->Convert(new_ofl, new_onl)) {
			printf("\tbad Convert\n");
			print(new_ofl);
			break;
		}
		old_onl = InttNameSpace::ToOnline(old_ofl);

		if(!cmp(new_onl, old_onl)) {
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
		new_ofl.layer = 0;

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
	InttMap::Offline_s const& ofl
) {
	printf("\t%-16s%3d\n", "layer:",		ofl.layer);
	printf("\t%-16s%3d\n", "ladder_phi:",	ofl.ladder_phi);
	printf("\t%-16s%3d\n", "ladder_z:",		ofl.ladder_z);
	printf("\t%-16s%3d\n", "strip_z:",		ofl.strip_z);
	printf("\t%-16s%3d\n", "strip_phi:",	ofl.strip_phi);
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

#endif//ONL_OFL_TEST_C
