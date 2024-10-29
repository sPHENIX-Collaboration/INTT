#ifndef SURVEY_STUFF_C
#define SURVEY_STUFF_C

#include <phool/phool.h>
#include <phool/recoConsts.h>
R__LOAD_LIBRARY(libphool.so)

#include <intt/InttMap.h>
#include <intt/InttSurveyMapv1.h>
R__LOAD_LIBRARY(libintt.so)

void
SurveyStuff (
) {
	recoConsts* rc = recoConsts::instance();
	rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
	rc->set_uint64Flag("TIMESTAMP", 1);

	InttSurveyMapv1 survey_from_file;
	// if (survey_from_file.LoadFromFile("../codes/intt_alignment/dat/intt_survey_cdbttree.root")) {
	if (survey_from_file.LoadFromFile("../codes/intt_alignment/dat/intt_survey_cdbttree_p02_3950mm.root")) {
		std::cerr << PHWHERE << std::endl;
		exit(1);
	}

	InttSurveyMapv1 survey_from_cdb;
	if (survey_from_cdb.LoadFromCDB()) {
		std::cerr << PHWHERE << std::endl;
		exit(1);
	}

	InttSurveyMap::val_t val_from_cdb;
	InttSurveyMap::val_t val_from_file;

	for (InttMap::Offline_s ofl = InttMap::OfflineBegin; ofl != InttMap::OfflineEnd; ++ofl) {

		survey_from_cdb. GetStripTransform(ofl, val_from_cdb );
		survey_from_file.GetStripTransform(ofl, val_from_file);

		if (val_from_cdb.matrix() == val_from_file.matrix()) continue;

		std::cout << "\n\nDifferent at:\n"
		          << ofl << "\n"
		          << val_from_cdb. matrix() << "\n"
		          << val_from_file.matrix() << "\n"
		          << "\n\nEarly returning" << std::endl;
		exit(0);
	}

	std::cout << "same" << std::endl;
}

#endif//SURVEY_STUFF_C
