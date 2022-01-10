#!/bin/bash

. ./common.sh

builder_exe=/opt/installbuilder-17.3.0/bin/builder

export RELEASE_DIRECTORY
OUTPUT_DIRECTORY=${RELEASE_DIRECTORY}/..
export OUTPUT_DIRECTORY

$builder_exe  


