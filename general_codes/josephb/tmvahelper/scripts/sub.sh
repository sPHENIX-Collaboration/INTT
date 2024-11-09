#!/bin/bash

if [ ! -f "$1" ] || ! [[ $2 =~ ^[0-9]+$ ]] || ! [[ $3 =~ ^[0-9]+$ ]]; then
cat << EOF
	
	usage:
		$0 [../any/path/to/macro.C] [number of subjobs] [number of events per subjob]
	Script to submit jobs which produce Monte-Carlo training data
	These jobs in turn run the wrapper script
		gen.sh

	e.g., for signal
		$0 ../macro/Fun4All_HF.C 100 2000

	and for background
		$0 ../macro/Fun4All_MB.C 100 200

EOF
	exit 1
fi

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
source tmva_setup.sh

MACRO=$(basename $1)
NUM_JOB=$2
NUM_EVT=$3

mkdir -p ${TMVA_SOURCE_DIR}/job
mkdir -p ${TMVA_SOURCE_DIR}/out

# used in naming job, out, and log files
ARGS="$(basename ${MACRO} .C)_\$(process)_${NUM_EVT}"
FILE="${TMVA_SOURCE_DIR}/job/$(basename ${MACRO} .C)_${NUM_JOB}_${NUM_EVT}.job"
cat << EOF > ${FILE}
universe           = vanilla
executable         = ${TMVA_SOURCE_DIR}/scripts/gen.sh
arguments          = ${TMVA_SOURCE_DIR}/macro/${MACRO} \$(process) ${NUM_EVT}

notification       = Never

output             = ${TMVA_SOURCE_DIR}/out/${ARGS}.out
error              = ${TMVA_SOURCE_DIR}/out/${ARGS}.out
log                = /tmp/${USER}_${ARGS}.log

request_memory     = 8192MB
PeriodicHold       = (NumJobStarts >= 1 && JobStatus == 1)
concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100

queue ${NUM_JOB}
EOF

echo "Submitting job file ${FILE}"
condor_submit ${FILE}

exit 0

