#include "base.h"

channel_classifier_base::channel_classifier_base() {
	// do nothing
}

channel_classifier_base::~channel_classifier_base() {
	// do nothing
}

void
channel_classifier_base::adjust_hitrate (
	Double_t& hitrate,
	InttNameSpace::RawData_s const& raw
) const {
	hitrate /= raw.chip % 13 < 5 ? 2.0 : 1.6;
}

int
channel_classifier_base::fill (
	std::string const&,
	std::string const&
) {
	// do nothing

	return 0;
}

int
channel_classifier_base::fit (
) {
	// do nothing

	return 0;
}

int
channel_classifier_base::write_hist (
	std::string const&
) {
	// do nothing

	return 0;
}
