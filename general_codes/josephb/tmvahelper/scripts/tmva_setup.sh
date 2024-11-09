#! /bin/bash

# Setup variables that other scripts can use
export USER="$(id -u -n)"
export LOGNAME="${USER}"
export HOME="/sphenix/u/${USER}"
export TMVA_SOURCE_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && cd .. &> /dev/null && pwd)"

# These may need to be changed for your use case
export MYINSTALL="/sphenix/user/${USER}/MYINSTALL"
# export TMVA_DATA_DIR="/sphenix/tg/tg01/hf/${USER}/tmva_for_real"
export TMVA_DATA_DIR="/sphenix/tg/tg01/hf/${USER}/tmva_Lc_prod"

if [ -z "${TMVA_DATA_DIR}" ] || ! [ -d "${TMVA_DATA_DIR}" ] || ! [ -w "${TMVA_DATA_DIR}" ]; then
cat << EOF

	Define the variable
		TMVA_DATA_DIR
	in
		${TMVA_SOURCE_DIR}/scripts/$(basename $0)
	to an existing directory (where you have write permissions)

	This script will not create one for you.

	Note that you may also wish to change the value of MYINSTALL, which is
		${MYINSTALL}

EOF
	exit 1
fi

mkdir -p ${TMVA_DATA_DIR}/monte_carlo
mkdir -p ${TMVA_DATA_DIR}/tmva_models
mkdir -p ${TMVA_DATA_DIR}/tmva_models/factory
mkdir -p ${TMVA_DATA_DIR}/tmva_models/dataloader
mkdir -p ${TMVA_DATA_DIR}/real_data

