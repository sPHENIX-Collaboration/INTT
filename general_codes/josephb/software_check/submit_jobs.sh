#! /bin/bash

mkdir -p out
mkdir -p job 

for I in {0..1}; do
	mkdir -p "check_$I"
	ln -sf "$(pwd)/check.sh" "$(pwd)/check_$I/."
	ln -sf "$(pwd)/list" "$(pwd)/check_$I/."
	for MACRO in $(ls *.C); do
		ln -sf "$(pwd)/${MACRO}" "$(pwd)/check_$I/."
	done
done

JOB="job/job.txt"
cat << EOF > ${JOB}
universe           = vanilla
initialdir         = $(pwd)/check_\$(process)
executable         = $(pwd)/check_\$(process)/check.sh
arguments          = \$(process)

notification       = Never

output             = $(pwd)/out/out_\$(process).txt
error              = $(pwd)/out/out_\$(process).txt
log                = /tmp/${USER}_check_\$(process).log

request_memory     = 8192MB
PeriodicHold       = (NumJobStarts >= 1 && JobStatus == 1)
concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100

queue 2
EOF
condor_submit ${JOB}

cat << EOF

	submitting job file ${JOB}

EOF

