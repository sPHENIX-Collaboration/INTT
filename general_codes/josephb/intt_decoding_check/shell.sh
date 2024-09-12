#!/bin/bash

# MYINSTALL=""
MYINSTALL="/sphenix/user/jbertaux/MYINSTALL"

# One-liner to get pwd
PWD=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

INTT_FORMAT="/sphenix/lustre01/sphnxpro/physics/INTT/physics/physics_intt%d-%08d-*"
LIST_FORMAT="lst/run_%08d_intt%01d.list"
DATA_FORMAT="dat/run_%08d_intt%01d.root"

RUN_NUM=""         # required
NUM_EVT="10000000" # default if argument 2 is empty

show_help()
{
cat << EOF

	usage:
		$0 [which intt] [run number] [num events (optional)]
	The felix server this should run for is specified by [which intt]
	The default for [num events] is ${NUM_EVT}

EOF
}

# Arguments
if [[ $# -lt 1 || $1 == "-h" || $1 == "--help" ]]; then
	show_help
	exit 0
fi

WHICH_INTT="$1"

RUN_NUM="$2"

if [[ -n "$3" ]]; then
	NUM_EVT="$3"
fi

# Custom MYINSTALL for developement/debugging purposes
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
if [ -n "${MYINSTALL}" ] && [ -d "${MYINSTALL}" ]; then
	source /opt/sphenix/core/bin/setup_local.sh ${MYINSTALL}
fi

printf -v LIST ${LIST_FORMAT} ${RUN_NUM} ${WHICH_INTT}
printf -v FILE ${INTT_FORMAT} ${WHICH_INTT} ${RUN_NUM}
mkdir -p $(dirname ${LIST})

ls -1 ${FILE} > ${LIST} 2>/dev/null
if [ ! -s ${LIST} ]; then
	echo -e "Expansion of ${FILE} failed\n"
	echo -e "Exiting\n"
	exit 1
	rm ${LIST}
fi

printf -v DATA ${DATA_FORMAT} ${RUN_NUM} ${WHICH_INTT}
mkdir -p $(dirname ${DATA})

# Macro
# root -q -b "${PWD}/macro.C(${RUN_NUM}, ${NUM_EVT}, \"${INTT_LIST}\")"
gdb -ex run --args root.exe "${PWD}/macro.C(${WHICH_INTT}, ${RUN_NUM}, ${NUM_EVT}, \"${LIST_FORMAT}\", \"${DATA_FORMAT}\")"

EXIT_VALUE="$?"

# Remove list files after finishing
rm -f ${LIST}

exit ${EXIT_VALUE}
