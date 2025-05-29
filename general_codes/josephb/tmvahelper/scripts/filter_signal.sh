#! /bin/bash

SHOW(){
cat << EOF
Running in directory:
$(pwd)

Directory contents:
$(ls -1a)

EOF
}

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
source tmva_setup.sh
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
if [ -n "${MYINSTALL}" ] && [ -d "${MYINSTALL}" ]; then
	source /opt/sphenix/core/bin/setup_local.sh ${MYINSTALL}
fi

cd ${TMVA_DATA_DIR}/tmva_models
rsync -av ${TMVA_SOURCE_DIR}/macro/* .
SHOW

root -q -b "filter_signal.C(\"${TMVA_DATA_DIR}/monte_carlo\")"
# root -q -b "filter_signal.C(\"${TMVA_DATA_DIR}/monte_carlo_bak\")"
# gdb -ex run --args root.exe -q -b  "filter_signalC(\"${TMVA_DATA_DIR}/monte_carlo\")"
RV=$?

echo "$0" done
exit $RV

