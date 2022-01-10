#!/bin/bash

echo "Copy Hexfiles..."

. ./common.sh

HERE=`pwd`

OUTDIR=${HERE}/../firmware/.
INDIR=${HERE}/../../hexfiles/. 

# Exclude non-working hex directories
LRMBEACON=lrm_beacon_secure
LRMNONBEACON=lrm_nonbeacon_secure

#cp -r ${INDIR} ${OUTDIR}

#rsync -av --exclude='path1/to/exclude' --exclude='path2/to/exclude' source destination
rsync -av --exclude=${LRMBEACON} --exclude=${LRMNONBEACON} ${INDIR} ${OUTDIR}

for x in `find ${OUTDIR} -type f -name '*.hex'`
do
#    echo "Dos2Unix for: $x"
    unix2dos -f $x
done

