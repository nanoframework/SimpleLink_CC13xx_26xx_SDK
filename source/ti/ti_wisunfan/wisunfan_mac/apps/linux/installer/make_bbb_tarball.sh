#!/bin/bash

. ./common.sh

cd ${RELEASE_DIRECTORY}

tar cfz bbb_prebuilt.tar.gz  prebuilt

mv bbb_prebuilt.tar.gz prebuilt



