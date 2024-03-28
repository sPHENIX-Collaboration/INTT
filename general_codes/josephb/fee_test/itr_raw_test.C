#ifndef ITR_RAW_TEST_C
#define ITR_RAW_TEST_C

#include <intt/InttMap.h>
#include <intt/InttFeeMap.h>
#include <intt/InttFeeMapv1.h>

R__LOAD_LIBRARY(libintt.so)

void print(InttMap::RawData_s const&);

void itr_raw_test() {
	std::unique_ptr<InttFeeMap> ifm = std::make_unique<InttFeeMapv1>();
	ifm->LoadFromFile("InttFeeMap.root");

	printf("Testing RawData itrator\n");

	int i;
	struct InttMap::RawData_s ctrl_raw;
	struct InttMap::RawData_s test_raw;

	std::size_t N = 8 * 14 * 26 * 128;
	struct InttMap::RawData_s* ctrl_vals = new InttMap::RawData_s[N];
	struct InttMap::RawData_s* test_vals = new InttMap::RawData_s[N];

	i = 0;
	ctrl_raw.pid = 3001;
	ctrl_raw.fee = 0;
	ctrl_raw.chp = 0;
	ctrl_raw.chn = 0;
	while(true) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		ctrl_vals[i] = ctrl_raw;

		++i;
		if(++ctrl_raw.chn < 128)continue;
		ctrl_raw.chn = 0;
		if(++ctrl_raw.chp < 26)continue;
		ctrl_raw.chp = 0;
		if(++ctrl_raw.fee < 14)continue;
		ctrl_raw.fee = 0;
		if(++ctrl_raw.pid < 3009)continue;
		ctrl_raw.pid = 3001;

		break;
	}

	i = 0;
	for(test_raw = InttMap::RawDataBegin; test_raw != InttMap::RawDataEnd; ++test_raw) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		test_vals[i] = test_raw;

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
	for(test_raw = InttMap::RawDataRBegin; test_raw != InttMap::RawDataREnd; --test_raw) {
		if(i >= N) {
			printf("\tbad N\n");
			break;
		}
		test_vals[i] = test_raw;

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
	InttMap::RawData_s const& raw
) {
	printf("\t%-16s%3d\n", "pid",	raw.pid);
	printf("\t%-16s%3d\n", "fee",	raw.fee);
	printf("\t%-16s%3d\n", "chp", 	raw.chp);
	printf("\t%-16s%3d\n", "chn",	raw.chn);
}

#endif//ITR_RAW_TEST_C
