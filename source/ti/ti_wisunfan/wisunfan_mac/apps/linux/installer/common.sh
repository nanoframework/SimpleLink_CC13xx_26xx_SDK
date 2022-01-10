# This is a common SH script file
# used by all scripts in this directory

RELEASE_DIRECTORY=`pwd`/../release/`date +%Y-%b-%d`

NAME='TI-15.4-Stack'
VERSION_A=2
VERSION_B=03
VERSION_C=00
VERSION_D=06
VERSION_QUAL=''
#VERSION_QUAL='_eng'

export DOCS_BRANCH=simplelink_cc13x0_sdk_1_50_00

VERSION_DOT=${VERSION_A}.${VERSION_B}.${VERSION_C}.${VERSION_D}${VERSION_QUAL}
VERSION_UND=${VERSION_A}_${VERSION_B}_${VERSION_C}_${VERSION_D}${VERSION_QUAL}
VERSION_SHORT_UND=${VERSION_A}_${VERSION_B}_${VERSION_C}
VERSION=${VERSION_DOT}

LICENSE_FILE=${RELEASE_DIRECTORY}/license_ti15.4stack_linux_x64_${VERSION}.txt

export RELEASE_DIRECTORY
export VERSION
export LICENSE_FILE

# auto-exit if any command fails.
set -e
