#! /bin/bash

if [[ -z "$1" || ("$1" -ne 0 && "$1" -ne 1) ]]; then
cat << EOF

	usage:
		$0 [0 or 1]
		0 for default (do NOT source setup_local with custom install)
		1 for custom (source setup_local with custom install)

EOF
	exit 0
fi

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
if [ "$1" -eq 1 ]; then
	export MYINSTALL2="/sphenix/user/jbertaux/MYINSTALL2"
	source /opt/sphenix/core/bin/setup_local.sh ${MYINSTALL2}
else
	source /opt/sphenix/core/bin/setup_local.sh
fi

# Prepend the install location of our libraries;
# in this case, it's local "install" subdirectory
# SCRIPT_DIR="$( cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd )"
# export LD_LIBRARY_PATH="${SCRIPT_DIR}/install:${LD_LIBRARY_PATH}"

cat << EOF

---------------------------------------------
LD_LIBRARY_PATH is:
${LD_LIBRARY_PATH}
---------------------------------------------

EOF
# root -q -b Fun4All_G4_sPHENIX.C | tee Fun4All_G4_sPHENIX.out
# root -q -b check.C | tee check.out
root -q -b do_round_trip.C | tee do_round_trip.out
