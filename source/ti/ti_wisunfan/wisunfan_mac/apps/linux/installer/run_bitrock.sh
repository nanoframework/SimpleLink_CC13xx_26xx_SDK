#!/bin/bash

. ./common.sh

builder_exe=/opt/installbuilder-17.3.0/bin/builder

OUTPUT_DIRECTORY=${RELEASE_DIRECTORY}/..
export OUTPUT_DIRECTORY

VERSION_NO_DOTS=`echo ${VERSION} | sed -e 's/\./_/g'`

INSTALLER_FILENAME=ti154stack_linux_x64
export INSTALLER_FILENAME

inst_gen -l ${INSTALLER_FILENAME} -t "TI-15.4Stack Linux SDK Version ${VERSION}" -i left.png ${VERSION_NO_DOTS} ${LICENSE_FILE} ${RELEASE_DIRECTORY} -d ${OUTPUT_DIRECTORY}
