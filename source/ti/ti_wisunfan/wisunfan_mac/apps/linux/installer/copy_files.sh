#!/bin/bash

. ./common.sh

# Pick one

# NOTE: pudb = a text based python debugger
#PYTHON=pudb
PYTHON=python

TODAY=`date +%Y-%b-%d`

OUTDIR=../release/$TODAY

SUB_MIG_GUIDE='s|ti154stack-users-guide\/ti154stack\/migration-guide.html|docs\/ti154stack-users-guide\/ti154stack\/migration-guide.html|g'

rm -rf $OUTDIR

$PYTHON ./lprf-sd-installers/common/linux/main.py --srcdir=.. --dstdir=${OUTDIR} --varfile=keyword_variables.ini --inifile=copy_rules.ini --logfile=copy_file_log.txt 

# Manually Copy HTML Documentation to /docs directory
#INDIR=../../../lprf-timac-2-docs/Ti154_Doc/SDK_Rel/ti154stack/*
INDIR=../docs/ti154stack/*

rsync -av ${INDIR} ${OUTDIR}/docs

mv ${OUTDIR}/docs/simplelink-cc13x0-sdk-migration-guide.html ${OUTDIR}

sed -i -e ${SUB_MIG_GUIDE} ${OUTDIR}/simplelink-cc13x0-sdk-migration-guide.html

exit 0
