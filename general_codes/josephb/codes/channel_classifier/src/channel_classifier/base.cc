#include "base.h"

channel_classifier_base::channel_classifier_base() {
	// do nothing
}

channel_classifier_base::~channel_classifier_base() {
	// do nothing
}

void
channel_classifier_base::adjust_hitrate (
	Double_t&,
	InttNameSpace::RawData_s const&
) const {
	// do nothing
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
channel_classifier_base::classify (
	std::string const&,
	std::string const&,
	std::string const&,
	std::string const&
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

int
channel_classifier_base::draw (
	std::string const&
) {
	// do nothing

	return 0;
}
