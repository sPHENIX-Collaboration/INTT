#!/bin/bash

show_help()
{
	printf "\n"
	printf "\tScript should be called as\n"
	printf "\t\tsh write_condor_job.sh [run number]\n"
	printf "\twhere [run number] is the run number to be analyzed\n"
	printf "\tThis submits the wrapper script \"channel_classify.sh\" to condor\n"
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
export DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )	# The location of this shell script

if [[ ! -d ${DIR}/job ]]; then mkdir -p ${DIR}/job; fi
if [[ ! -d ${DIR}/out ]]; then mkdir -p ${DIR}/out; fi
if [[ ! -d ${DIR}/err ]]; then mkdir -p ${DIR}/err; fi

export FILE=${DIR}/job/job_${1}.job

printf "" > ${FILE} 

printf "Universe	= vanilla\n"					>> ${FILE}
printf "Executable	= ${DIR}/channel_classify.sh\n"			>> ${FILE}
printf "Arguments	= %s\n" "$1"					>> ${FILE}
printf "notification	= Never\n"					>> ${FILE}
printf "\n"								>> ${FILE}
printf "Output		= %s\n" "${DIR}/out/out_${1}.txt"		>> ${FILE}
printf "Error		= %s\n" "${DIR}/err/err_${1}.txt"		>> ${FILE}
printf "Log		= /tmp/condor\$(process)_%s.log\n" "${LOGNAME}"	>> ${FILE}
printf "\n"								>> ${FILE}
printf "Initialdir	= %s\n" "${DIR}"				>> ${FILE}
printf "request_memory	= 4096MB\n"					>> ${FILE}
printf "PeriodicHold	= (NumJobStarts >= 1 && JobStatus == 1)\n"	>> ${FILE}
printf "\n"								>> ${FILE}
printf "Queue\n"							>> ${FILE}
printf "\n"								>> ${FILE}

echo "condor_submit ${FILE}"
condor_submit ${FILE}

exit 0
