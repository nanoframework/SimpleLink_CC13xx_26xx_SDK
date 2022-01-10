#!/bin/bash

echo "Final cleanups before bitrock..."

. ./common.sh

cd ${RELEASE_DIRECTORY}

bash ./build_all.sh clean
rm -f ./*.log

cd ${RELEASE_DIRECTORY}/example/google 
make dist-clean


