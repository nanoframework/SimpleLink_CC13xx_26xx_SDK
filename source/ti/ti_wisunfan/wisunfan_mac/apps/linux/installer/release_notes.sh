#!/bin/bash

. ./common.sh

#!/bin/sh

# Script to substitute version numbers in HTML and JSON documents
# some issues:
#
#    SDO tools *require* python 3.5 
#
#    This system also requires the SDO tools installed somewhere, aka: swtools
#    I have them installed here:  /opt/ti/swtools
#       Server machines often have them in /local/<something>
#
#----------------------------------------
#    This is not available on Ubuntu 14.04 :-(
#    So... I built and installed python from scratch (very simple)
#       step 1:  download python 3.5.2 tar.gz
#       step 2:  unpack
#       step 3:  cd SOURCE 
#       step 4:  ./configure
#       step 5:  make
#       step 6:  sudo make install
#       step 7:  Profit!
#----------------------------------------

# point to the SDO tools..
export SWTOOLS=/opt/ti/swtools/
export PATH=${SWTOOLS}/bin:${PATH}
export PYTHONPATH=${SWTOOLS}/etc:${PYTHONPATH}
export MANPATH=${SWTOOLS}/etc/man:${MANPATH}

# bring our common settings in.
. ./common.sh

# Name & version for TI-15.4-STACK product
MACNAM='TI 15.4 Stack Linux x64'

# JIRA filters for 'Fixed Issues' (FI), 'Known Issues' (KI), 'New Features' (NF)
# Standardized JIRA fiilter naming: <product>_<filter>_<version>, example: linux_timac_fi_2_00_01
JIRAFI=linux_timac_fi_${VERSION_SHORT_UND}
JIRAKI=linux_timac_ki_${VERSION_SHORT_UND}
JIRANF=linux_timac_nf_${VERSION_SHORT_UND}

# Build the MAC Release Notes, then update resource version numbers
NAMDOC='release_notes'
TITLE="${MACNAM} ${MACDOT} Release Notes"
SRCDOC=../install-root/release_notes.md
DSTDOC=${RELEASE_DIRECTORY}/release_notes_ti15.4stack_linux_x64_${VERSION_DOT}.html
#
# to enforce Python3.5 - we use this shell script wrapper.
set -x

python3.5 `which doc_rn` -f ${JIRAFI},${JIRAKI},${JIRANF} -T "${TITLE}" ${VERSION_DOT} ${SRCDOC} -o ${DSTDOC}
