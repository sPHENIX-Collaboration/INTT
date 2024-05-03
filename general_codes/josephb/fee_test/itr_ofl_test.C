#ifndef ITR_OFL_TEST_C
#define ITR_OFL_TEST_C

#include <intt/InttMap.h>
#include <intt/InttFeeMap.h>
#include <intt/InttFeeMapv1.h>

R__LOAD_LIBRARY(libintt.so)

void print(InttMap::Offline_s const&);

void itr_ofl_test() {
	std::unique_ptr<InttFeeMap> ifm = std::make_unique<InttFeeMapv1>();
	ifm->LoadFromFile("InttFeeMap.root");

	printf("Testing Offline itrator\n");

	int i;
	struct InttMap::Offline_s ctrl_ofl;
	struct InttMap::Offline_s test_ofl;

	std::size_t N = 56 * 26 * 256;
	struct InttMap::Offline_s* ctrl_vals = new InttMap::Offline_s[N];
	struct InttMap::Offline_s* test_vals = new InttMap::Offline_s[N];

	i = 0;
	ctrl_ofl.layer = 3;
	ctrl_ofl.ladder_phi = 0;
	ctrl_ofl.ladder_z = 0;
	ctrl_ofl.strip_z = 0;
	ctrl_ofl.strip_phi = 0;
	while(true) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		ctrl_vals[i] = ctrl_ofl;

		++i;
		if(++ctrl_ofl.strip_phi < 256)continue;
		ctrl_ofl.strip_phi = 0;
		if(++ctrl_ofl.strip_z < (ctrl_ofl.ladder_z % 2 ? 5 : 8))continue;
		ctrl_ofl.strip_z = 0;
		if(++ctrl_ofl.ladder_z < 4)continue;
		ctrl_ofl.ladder_z = 0;
		if(++ctrl_ofl.ladder_phi < (ctrl_ofl.layer < 5 ? 12 : 16))continue;
		ctrl_ofl.ladder_phi = 0;
		if(++ctrl_ofl.layer < 7)continue;
		ctrl_ofl.layer = 3;

		break;
	}

	i = 0;
	for(test_ofl = InttMap::OfflineBegin; test_ofl != InttMap::OfflineEnd; ++test_ofl) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		test_vals[i] = test_ofl;

		++i;
	}

	for(i = 0; i < N; ++i) {
		if(ctrl_vals[i] == test_vals[i])continue;
		printf("\tfailed (%d)\n", i);
		print(test_vals[i]);
		print(ctrl_vals[i]);
		break;
	}

	for(i = 1; i < N; ++i) {
		if(test_vals[i - 1] < test_vals[i])continue;
		printf("\tunordered (%d)\n", i);
		print(test_vals[i - 1]);
		print(test_vals[i]);
		break;
	}

	i = 0;
	for(test_ofl = InttMap::OfflineRBegin; test_ofl != InttMap::OfflineREnd; --test_ofl) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		test_vals[i] = test_ofl;

		++i;
	}

	for(i = 0; i < N; ++i) {
		if(ctrl_vals[i] == test_vals[N - i - 1])continue;
		printf("\tbad decrement (%d)\n", i);
		print(ctrl_vals[i]);
		print(test_vals[N - i - 1]);
		break;
	}

	delete[] ctrl_vals;
	delete[] test_vals;

	printf("done\n");
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

#endif//ITR_OFL_TEST_C
