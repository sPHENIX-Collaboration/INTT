#! /bin/bash

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
source tmva_setup.sh
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
if [ -n "${MYINSTALL}" ] && [ -d "${MYINSTALL}" ]; then
	source /opt/sphenix/core/bin/setup_local.sh ${MYINSTALL}
fi

cd ${TMVA_SOURCE_DIR}/macro
root "test.C(\"${TMVA_DATA_DIR}/monte_carlo\")"
RV=$?
exit ${RV}
