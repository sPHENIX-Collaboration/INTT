#ifndef ITR_ONL_TEST_C
#define ITR_ONL_TEST_C

#include <intt/InttMap.h>
#include <intt/InttFeeMap.h>
#include <intt/InttFeeMapv1.h>

R__LOAD_LIBRARY(libintt.so)

void print(InttMap::Online_s const&);

void itr_onl_test() {
	std::unique_ptr<InttFeeMap> ifm = std::make_unique<InttFeeMapv1>();
	ifm->LoadFromFile("InttFeeMap.root");

	printf("Testing Online itrator\n");

	int i;
	struct InttMap::Online_s ctrl_onl;
	struct InttMap::Online_s test_onl;

	std::size_t N = 56 * 2 * 26 * 128;
	struct InttMap::Online_s* ctrl_vals = new InttMap::Online_s[N];
	struct InttMap::Online_s* test_vals = new InttMap::Online_s[N];

	i = 0;
	ctrl_onl.lyr = 0;
	ctrl_onl.ldr = 0;
	ctrl_onl.arm = 0;
	ctrl_onl.chp = 0;
	ctrl_onl.chn = 0;
	while(true) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		ctrl_vals[i] = ctrl_onl;

		++i;
		if(++ctrl_onl.chn < 128)continue;
		ctrl_onl.chn = 0;
		if(++ctrl_onl.chp < 26)continue;
		ctrl_onl.chp = 0;
		if(++ctrl_onl.arm < 2)continue;
		ctrl_onl.arm = 0;
		if(++ctrl_onl.ldr < (ctrl_onl.lyr < 2 ? 12 : 16))continue;
		ctrl_onl.ldr = 0;
		if(++ctrl_onl.lyr < 4)continue;
		ctrl_onl.lyr = 0;

		break;
	}

	i = 0;
	for(test_onl = InttMap::OnlineBegin; test_onl != InttMap::OnlineEnd; ++test_onl) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		test_vals[i] = test_onl;

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
	for(test_onl = InttMap::OnlineRBegin; test_onl != InttMap::OnlineREnd; --test_onl) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		test_vals[i] = test_onl;

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
	InttMap::Online_s const& onl
) {
	printf("\t%-16s%3d\n", "lyr:",	onl.lyr);
	printf("\t%-16s%3d\n", "ldr:",	onl.ldr);
	printf("\t%-16s%3d\n", "arm:", 	onl.arm);
	printf("\t%-16s%3d\n", "chp:", 	onl.chp);
	printf("\t%-16s%3d\n", "chn:",	onl.chn);
}

#endif//ITR_ONL_TEST_C
