#!/bin/bash

set -e

DEST=collector4windows
DEST_ZIP=${DEST}.zip
DEST_GATEWAY=./${DEST}/gateway
SRC_GATEWAY=./gateway

DEST_COLLECTOR=${DEST}/collector
SRC_COLLECTOR=./collector

echo "Clean up... "
rm -rf ${DEST}
rm -rf ${DEST_ZIP}

echo "Creating: ${DEST}"
mkdir -p ${DEST}
VERFILE=${DEST}/version.txt
echo "   Created: " `date` >> ${VERFILE}
echo "Created By: ${USERNAME}" >> ${VERFILE}
if git diff --quiet HEAD
then
	echo "Git Status: DIRTY" >> ${VERFILE}
else
	echo "Git Status: CLEAN" >> ${VERFILE}
fi

echo "Git Rev: " `git rev-parse HEAD` >> ${VERFILE}

mkdir -p ${DEST_COLLECTOR}
mkdir -p ${DEST_GATEWAY}


echo "Copy gateway..."
(cd ${SRC_GATEWAY} && tar cf - .) | (cd ${DEST_GATEWAY} ; tar xf -)
echo "Fetch node.exe (windows)"
curl -o ${DEST_GATEWAY}/node.exe https://nodejs.org/dist/v4.3.1/win-x64/node.exe


echo "Copy Collector (windows)"
cp ${SRC_COLLECTOR}/x64/Release/collector.exe ${DEST_COLLECTOR}
cp ${SRC_COLLECTOR}/appsrv.cfg                ${DEST_COLLECTOR}

cp ${SRC_COLLECTOR}/00_run_collector.bat      ${DEST_COLLECTOR}

echo "Creating ZIP: ${DEST_ZIP}"
/c/Program\ Files/7-Zip/7z.exe a ${DEST_ZIP} ${DEST}

echo "Copy to network"
NETLOC=//buffy.sanc.design.ti.com/LPRF/Public/Duane/TIMAC-20-WINDOWS/
cp ${DEST_ZIP} ${NETLOC}/CURRENT
today=`date +%Y%b%d_%H_%M`
cp ${DEST_ZIP} ${NETLOC}/ARCHIVE/${today}_${DEST_ZIP}

echo "Done"

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: sh
#  End:
#  vim:set  filetype=sh

