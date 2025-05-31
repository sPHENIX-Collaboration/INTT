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
SCRIPT_DIR="$( cd "$(dirname "$(readlink -- "${BASH_SOURCE[0]}")")" &> /dev/null && pwd )"

export CPLUS_INCLUDE_PATH="${SCRIPT_DIR}/install/include:${CPLUS_INCLUDE_PATH}"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/install/lib:${LD_LIBRARY_PATH}"

cat << EOF

---------------------------------------------
CPLUS_INCLUDE_PATH is:
${CPLUS_INCLUDE_PATH}
---------------------------------------------
LD_LIBRARY_PATH is:
${LD_LIBRARY_PATH}
---------------------------------------------

EOF

# Simulation
root -q -b Fun4All_G4_sPHENIX.C | tee Fun4All_G4_sPHENIX.out
root -q -b "Fun4All_Check.C(\"G4sPHENIX.root\")" | tee sim_check.out

# Tracking production
root -q -b Fun4All_PRDFReconstruction.C | tee Fun4All_PRDFReconstruction.out
root -q -b "Fun4All_Check.C(\"dstout.root\")" | tee prdf_check.out

# Calibration check
root -q -b calib_check.C | tee calib_check.out

# Arborist check
root -q -b arborist_check.C | tee arborist_check.out

# Direct mapping check
root -q -b round_trip_check.C | tee round_trip_check.out
