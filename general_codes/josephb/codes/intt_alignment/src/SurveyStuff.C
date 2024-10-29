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
	if (survey_from_file.LoadFromFile("../dat/intt_survey_cdbttree.root")) {
	// if (survey_from_file.LoadFromFile("../dat/intt_survey_cdbttree_p02_3950mm.root")) {
		std::cerr << PHWHERE << std::endl;
		exit(1);
	}

	InttSurveyMapv1 survey_from_cdb;
	if (survey_from_cdb.LoadFromCDB()) {
		std::cerr << PHWHERE << std::endl;
		exit(1);
	}

	int flag;
	InttSurveyMap::val_t val_from_cdb;
	InttSurveyMap::val_t val_from_file;

	flag = 1;
	for (InttMap::Offline_s ofl = InttMap::OfflineBegin; ofl != InttMap::OfflineEnd; ++ofl) {

		survey_from_cdb. GetStripTransform(ofl, val_from_cdb );
		survey_from_file.GetStripTransform(ofl, val_from_file);

		if (val_from_cdb.matrix() == val_from_file.matrix()) continue;

		flag = 0;
		std::cout << "\n\nDifferent at strip level:\n"
		          << ofl << "\n"
		          << "From CDB:\n"
		          << val_from_cdb. matrix() << "\n"
		          << "From File:\n"
		          << val_from_file.matrix() << "\n" << std::endl;
		break;
	}
	if (flag) std::cout << "strip transforms consistent" << std::endl;

	flag = 1;
	for (InttMap::Offline_s ofl = InttMap::OfflineBegin; ofl != InttMap::OfflineEnd; ++ofl) {

		survey_from_cdb. GetLadderTransform(ofl, val_from_cdb );
		survey_from_file.GetLadderTransform(ofl, val_from_file);

		if (val_from_cdb.matrix() == val_from_file.matrix()) continue;

		flag = 0;
		std::cout << "\n\nDifferent at ladder level:\n"
		          << ofl << "\n"
		          << "From CDB:\n"
		          << val_from_cdb. matrix() << "\n"
		          << "From File:\n"
		          << val_from_file.matrix() << "\n" << std::endl;
		break;
	}
	if (flag) std::cout << "ladder transforms consistent" << std::endl;

	flag = 1;
	for (InttMap::Offline_s ofl = InttMap::OfflineBegin; ofl != InttMap::OfflineEnd; ++ofl) {

		survey_from_cdb. GetSensorTransform(ofl, val_from_cdb );
		survey_from_file.GetSensorTransform(ofl, val_from_file);

		if (val_from_cdb.matrix() == val_from_file.matrix()) continue;

		flag = 0;
		std::cout << "\n\nDifferent at sensor level:\n"
		          << ofl << "\n"
		          << "From CDB:\n"
		          << val_from_cdb. matrix() << "\n"
		          << "From File:\n"
		          << val_from_file.matrix() << "\n" << std::endl;
		break;
	}
	if (flag) std::cout << "sensor transforms consistent" << std::endl;
}

#endif//SURVEY_STUFF_C
