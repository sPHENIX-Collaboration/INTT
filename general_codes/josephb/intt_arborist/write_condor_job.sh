#!/bin/bash

export USR="$(id -u -n)"
export PWD=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
# The location of this shell script

export EXE="/direct/sphenix+u/jbertaux/sphnx_software/INTT/general_codes/josephb/intt_arborist/intt_arborist_shell.sh"
export DIR="${PWD}"
export MEM="4096MB"

show_help() {
	printf "\n"
	printf "\tusage:\n"
	printf "\t\t./write_condor_job.sh [args]\n"
	printf "\tsubmits a condor job.sh to run \${EXE} variable with the passed [args]\n"
	printf "\tyou may wish to check and edit the \${EXE} and \${DIR} variables\n"
	printf "\n"
}

if [[ $1 == "-h" ]]
then
	show_help
	exit 0
fi

if [[ ! -d ${DIR}/job ]]; then mkdir -p ${DIR}/job; fi
if [[ ! -d ${DIR}/out ]]; then mkdir -p ${DIR}/out; fi
if [[ ! -d ${DIR}/err ]]; then mkdir -p ${DIR}/err; fi

PRV=${IFS}
IFS="_"
ARGS="$*"
IFS=${PRV}
export FILE="${DIR}/job/$(basename ${EXE} .sh)_${ARGS}.job"

printf "" > ${FILE} 

printf "Universe        = vanilla\n"                                    >> ${FILE}
printf "Executable      = ${EXE}\n"                                     >> ${FILE}
printf "Arguments       = $*\n"                                         >> ${FILE}
printf "Notification    = Never\n"                                      >> ${FILE}
printf "\n"                                                             >> ${FILE}
printf "Output          = ${DIR}/out/out_${1}.txt\n"                    >> ${FILE}
printf "Error           = ${DIR}/err/err_${1}.txt\n"                    >> ${FILE}
printf "Log             = /tmp/condor\$(process)_${USR}.log\n"          >> ${FILE}
printf "\n"                                                             >> ${FILE}
printf "Initialdir      = ${DIR}\n"                                     >> ${FILE}
printf "request_memory  = ${MEM}\n"                                     >> ${FILE}
printf "PeriodicHold    = (NumJobStarts >= 1 && JobStatus == 1)\n"      >> ${FILE}
printf "\n"                                                             >> ${FILE}
printf "Queue\n"                                                        >> ${FILE}
printf "\n"                                                             >> ${FILE}

# echo "submitting job file ${FILE}"
# condor_submit ${FILE}

exit 0
