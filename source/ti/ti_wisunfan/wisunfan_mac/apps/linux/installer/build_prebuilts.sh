#!/bin/bash

. ./common.sh

cd ${RELEASE_DIRECTORY}

bash ./build_all.sh clean
bash ./build_all.sh host
bash ./build_all.sh bbb

rm *.log

