#! /bin/bash

SHOW(){
cat << EOF
Running in directory:
$(pwd)

Directory contents:
$(ls -1a)

EOF
}

if [ ! -f "$1" ] || ! [[ $2 =~ ^[0-9]+$ ]] || ! [[ $3 =~ ^[0-9]+$ ]]; then
cat << EOF
	
	usage:
		$0 [macro.C] [subprocess id] [number of events]
	Wrapper shell script to run either Fun4All macro which generates training data
	(macro/Fun4All_HF.C for signal, macro/Fun4All_MB.C for background)

	Must be run as a Condor job

EOF
	exit 0
fi

MACRO=$(basename $1)
JOB_NUM=$2
NUM_EVT=$3

if [ -z "${_CONDOR_SCRATCH_DIR}" ] || ! [ -d "${_CONDOR_SCRATCH_DIR}" ]; then
cat << EOF

	Script must run under Condor

EOF
	exit 1
fi

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
source tmva_setup.sh
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
if [ -n "${MYINSTALL}" ] && [ -d "${MYINSTALL}" ]; then
	source /opt/sphenix/core/bin/setup_local.sh ${MYINSTALL}
fi

cd ${_CONDOR_SCRATCH_DIR}
rsync -av ${TMVA_SOURCE_DIR}/macro/* .
SHOW

root -q -b "${MACRO}(\"${JOB_NUM}\", ${NUM_EVT})"
# gdb -ex run --args root.exe -q -b "${MACRO}(\"${JOB_NUM}\", ${NUM_EVT})"
RV=$?

SHOW
cp *.root ${TMVA_DATA_DIR}/monte_carlo/.

echo "$0" done
exit $RV

