#ifndef GEN_ONL_RAW_MAP_C
#define GEN_ONL_RAW_MAP_C

#include <cdbobjects/CDBTTree.h>
#include <intt/InttMap.h>
#include <intt/InttMapping.h>

R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libintt.so)

void gen_onl_raw_map() {
	printf("Hello World\n");

	InttMap::Online_s new_onl;
	new_onl.chp = InttMap::Wildcard;
	new_onl.chn = InttMap::Wildcard;
	InttMap::RawData_s new_raw;
	new_raw.chp = InttMap::Wildcard;
	new_raw.chn = InttMap::Wildcard;

	InttNameSpace::Online_s old_onl;
	InttNameSpace::RawData_s old_raw;

	new_onl.lyr = 0;
	new_onl.ldr = 0;
	new_onl.arm = 0;

	Int_t size = 0;
	CDBTTree cdbttree("InttFeeMap.root");
	while(true) {
		old_onl.lyr = new_onl.lyr;
		old_onl.ldr = new_onl.ldr;
		old_onl.arm = new_onl.arm;
		old_onl.chp = 0;
		old_onl.chn = 0;

		old_raw = InttNameSpace::ToRawData(old_onl);
		new_raw.pid = old_raw.felix_server + 3001;
		new_raw.fee = old_raw.felix_channel;
		new_raw.chp = InttMap::Wildcard;
		new_raw.chn = InttMap::Wildcard;

		cdbttree.SetIntValue(size, "lyr", new_onl.lyr);
		cdbttree.SetIntValue(size, "ldr", new_onl.ldr);
		cdbttree.SetIntValue(size, "arm", new_onl.arm);
		cdbttree.SetIntValue(size, "chp", new_onl.chp);
		cdbttree.SetIntValue(size, "chn", new_onl.chn);

		cdbttree.SetIntValue(size, "pid", new_raw.pid);
		cdbttree.SetIntValue(size, "fee", new_raw.fee);
		cdbttree.SetIntValue(size, "chp", new_raw.chp);
		cdbttree.SetIntValue(size, "chn", new_raw.chn);

		++size;

		if(++new_onl.arm < 2)continue;
		new_onl.arm = 0;
		if(++new_onl.ldr < (new_onl.lyr < 2 ? 12 : 16))continue;
		new_onl.ldr = 0;
		if(++new_onl.lyr < 4)continue;
		new_onl.lyr = 0;

		break;
	}
	cdbttree.SetSingleIntValue("size", size);

	cdbttree.Commit();
	cdbttree.CommitSingle();
	cdbttree.WriteCDBTTree();
}

#endif//GEN_ONL_RAW_MAP_C
