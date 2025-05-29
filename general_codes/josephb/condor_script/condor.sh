#!/bin/bash

USR="$(id -u -n)"
PWD="$(pwd)"

# PWD=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
# The location of this shell script

show_help() {
cat << EOF

	Usage:
		./$0 [executable] [args...]
	Submits a condor job for one instance of [executable] with arguments [args...]
	[executable] must have execute permissions be in the local directory \$(pwd):
		${PWD}
	[args...] are passed to [executable] in the job file
	Job file is created locally under a subdir \$(pwd)/job/
	Out file is created locally under a subdir \$(pwd)/out/

EOF
}

if [[ $# -lt 1 || $1 == "-h" || $1 == "--help" ]]; then
	show_help
	exit 0
fi

if [ ! -x $1 ]; then
cat << EOF

	Argument \$1 must be the path to an executable file

EOF
	exit 1
fi

[[ -d ${PWD}/job ]] || mkdir -p ${PWD}/job
[[ -d ${PWD}/out ]] || mkdir -p ${PWD}/out

# underscore-deliminated list of command line args
# keep suffix-pruned basenames in case arguments contain other file paths
ARGS=""
COUNT="0"
for ARG in "$@"; do
	ARGS="${ARGS}$(basename "${ARG%.*}")"
	COUNT=$((COUNT+1))
	[ "${COUNT}" -lt $# ] || continue;
	ARGS="${ARGS}_"
done

# used in naming the job file
# FILE="${PWD}/job/$(basename ${EXE} .sh)_${ARGS}.job"
FILE="${PWD}/job/job_${ARGS}.job"
cat << EOF > ${FILE}
universe           = vanilla
executable         = $1
arguments          = ${@:2}
initialdir         = ${PWD}

notification       = Never

output             = ${PWD}/out/${ARGS}.out
log                = /tmp/${USR}_${ARGS}.log

initialdir         = ${PWD}
request_memory     = 8192MB
PeriodicHold       = (NumJobStarts >= 1 && JobStatus == 1)
concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100

queue
EOF

echo "submitting job file ${FILE}"
condor_submit ${FILE}

exit 0
