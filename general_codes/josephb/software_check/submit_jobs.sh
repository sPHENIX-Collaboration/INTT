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

mkdir -p out
mkdir -p job 

mkdir -p "check_$1"
ln -sf "$(pwd)/check.sh" "$(pwd)/check_$1/."
for MACRO in $(ls *.C); do
	ln -sf "$(pwd)/${MACRO}" "$(pwd)/check_$1/."
done
exit 0

JOB="job/job_$1.txt"
cat << EOF > ${JOB}
universe           = vanilla
executable         = $(pwd)/check_$1/check.sh
arguments          = $1
initialdir         = $(pwd)/check_$1

notification       = Never

output             = $(pwd)/out/out_$1.txt
error              = $(pwd)/out/out_$1.txt
log                = /tmp/${USER}_check_$1.log

request_memory     = 8192MB
PeriodicHold       = (NumJobStarts >= 1 && JobStatus == 1)
concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100
EOF
condor_submit ${JOB}


cat << EOF

	submitting job file ${JOB}

EOF

