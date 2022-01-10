#! /bin/bash

KLOCWORK_DIR=~/klocwork

PROJECT=LPRF_TIMAC2_LINUX

export PATH=~/klocwork/Klocwork_10.1.2/bin:${PATH}
bash ./build_all.sh clean
${KLOCWORK_DIR}/run_kw.sh ${PROJECT} "bash ./build_all.sh" 10.1.2
