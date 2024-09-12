#!/bin/bash

USR="$(id -u -n)"
PWD=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
# The location of this shell script

EXE="shell.sh"
MEM="1024MB"

show_help() {
cat << EOF
	usage:
		./$0 [run number] [num events (optional)]
	submits a condor job to do a calibration for [run number]
	the output will be in the local directory
EOF
}

if [[ $# -lt 1 ]]; then
	show_help
	exit 1
fi

if [[ $1 == "-h" || $1 == "--help" ]]; then
	show_help
	exit 0
fi

RUN_NUM="$1"

[[ -d ${PWD}/job ]] || mkdir -p ${PWD}/job
[[ -d ${PWD}/out ]] || mkdir -p ${PWD}/out

# underscore-deliminated list of command line args
IFS="_"
ARGS="$*"
IFS=" "

# used in naming the job file
# FILE="${PWD}/job/$(basename ${EXE} .sh)_${ARGS}.job"
FILE="${PWD}/job/job_${ARGS}.job"
cat << EOF > ${FILE}
universe           = vanilla
executable         = ${EXE}
arguments          = \$(Process) $*
initialdir         = ${PWD}

notification       = Never

output             = ${PWD}/out/out_\$(Process)_${ARGS}.out
log                = /tmp/${USR}_\$(Process)_${ARGS}.log

initialdir         = ${PWD}
request_memory     = ${MEM}
PeriodicHold       = (NumJobStarts >= 1 && JobStatus == 1)
concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100

queue 8
EOF

echo "submitting job file ${FILE}"
condor_submit ${FILE}

exit 0
