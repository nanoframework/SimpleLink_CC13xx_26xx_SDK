#!/bin/bash

echo "Copy prebuilts..."

. ./common.sh

HERE=`pwd`

cd ${RELEASE_DIRECTORY}

PREBUILT_DIR=${RELEASE_DIRECTORY}/prebuilt

PREBUILT_BIN_DIR=${PREBUILT_DIR}/bin
PREBUILT_GATEWAY_DIR=${PREBUILT_DIR}/gateway

mkdir -p ${PREBUILT_BIN_DIR}
mkdir -p ${PREBUILT_GATEWAY_DIR}

cp -r  ${RELEASE_DIRECTORY}/example/gateway/.				${PREBUILT_GATEWAY_DIR}

cp ${RELEASE_DIRECTORY}/example/collector/host_collector		${PREBUILT_BIN_DIR}/.
cp ${RELEASE_DIRECTORY}/example/collector/bbb_collector			${PREBUILT_BIN_DIR}/.
cp ${RELEASE_DIRECTORY}/example/collector/collector.cfg			${PREBUILT_BIN_DIR}/.

cp ${RELEASE_DIRECTORY}/example/collector/run_collector.sh		${PREBUILT_BIN_DIR}/.

cp ${RELEASE_DIRECTORY}/example/npi_server2/apimac-msgs.cfg             ${PREBUILT_BIN_DIR}/.
cp ${RELEASE_DIRECTORY}/example/npi_server2/host_npi_server2		${PREBUILT_BIN_DIR}/.
cp ${RELEASE_DIRECTORY}/example/npi_server2/bbb_npi_server2		${PREBUILT_BIN_DIR}/.
cp ${RELEASE_DIRECTORY}/example/npi_server2/npi_server2.cfg		${PREBUILT_BIN_DIR}/.

cp ${RELEASE_DIRECTORY}/example/cc13xx-sbl/app/linux/host_cc13xx-sbl	${PREBUILT_BIN_DIR}/.
cp ${RELEASE_DIRECTORY}/example/cc13xx-sbl/app/linux/bbb_cc13xx-sbl	${PREBUILT_BIN_DIR}/.

