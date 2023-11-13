#!/bin/bash

show_help()
{
	printf "\n"
	printf "\tScript should be called as\n"
	printf "\t\tsh WriteCondorJob.sh [run number]\n"
	printf "\tWhere [run number] is the run number\n"
	printf "\n"
	printf "\tThis creates a condor job file under jobs\\\n"
	printf "\tWhich in turn calls InttUnpackerJb.sh [run number]\n"
	printf "\tTo edit the input and output files, edit InttUnpackerJb.sh\n"
	printf "\n"
	printf "If you are seeing this message, no job was submitted.\n"
	printf "\n"
}

if [[ $# -ne 1 ]]
then
	show_help
	exit 1
fi

if [[ $1 == "-h" ]]
then
	show_help
	exit 0
fi

export USER="$(id -u -n)"
export LOGNAME=${USER}
export DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export FILE=${DIR}/job/job_${1}.job

printf "" > ${FILE} 

printf "Universe	= vanilla\n" >>						${FILE}
printf "Executable	= ${DIR}/InttUnpackerShellJb.sh\n" >>			${FILE}
printf "Arguments	= %s\n" "$1" >>						${FILE}
printf "notification	= Never\n" >>						${FILE}

printf "Output		= %s\n" "${DIR}/out/out_${1}.txt" >>			${FILE}
printf "Error		= %s\n" "${DIR}/err/err_${1}.txt" >>			${FILE}
printf "Log		= /tmp/condor\$(process)_%s.log\n" "${LOGNAME}" >>	${FILE}

printf "Initialdir	= %s\n" "${DIR}" >>					${FILE}
printf "request_memory	= 4096MB\n" >>						${FILE}
printf "PeriodicHold	= (NumJobStarts >= 1 && JobStatus == 1)\n" >>		${FILE}

printf "Queue\n" >>								${FILE}

echo "condor_submit ${FILE}"
condor_submit ${FILE}
