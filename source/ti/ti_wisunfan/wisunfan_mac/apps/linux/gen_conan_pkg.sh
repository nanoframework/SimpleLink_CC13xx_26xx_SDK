#! /bin/bash
#
# Script to create and upload a component package for a particular SDK.
#
# Environment Variable:
#    - CONAN_LOGIN_USERNAME
#    - CONAN_PASSWORD
#    - PUBLISH
#
# Usage:
# ./gen_conan_pkg.sh
#
set -e

echoerr() { echo -e "$@" 1>&2; }

print_usage(){
  echo -e "build the 154stack conan package for a given SDK and MAJ_MIN_VER"
  echo;
  echo -e "Usage:"
  echo -e "    gen_conan_pkg.sh [OPTIONS] SDK MAJ_MIN_VER"
  echo;
  echo -e "Arguments:"
  echo -e "    SDK           {cc13x2_26x2,cc2652rb}"
  echo -e "    MAJ_MIN_VER   (M|M.mm|M.mm.pp) where M is a 1 digit major number, mm is"
  echo -e "                  a 2 digit minor number, and pp is a 2 digit patch number"
  echo;
  echo -e "Options:"
  echo -e "   -c CHANNEL --channel CHANNEL"
  echo -e "                  Conan channel [default ga]. If channel is not \"eng\" or"
  echo -e "                  \"ga\" the repo will not be tagged and MAJ_MIN_VER will be"
  echo -e "                  ignored, instead the commit hash will be used as the version"
  echo -e "   --alt-ver VER  when CHANNEL != \"eng\" OR CHANNEL != \"ga\" specify an"
  echo -e "                  alternate package version, to override the default git hash"
  echo;
  echo -e "\nEnvironmental varaible \"PUBLISH\" must be defined for pkg uploads"
}

arg_parse() {
  POSARGS="0"
  while (($#)); do
    case $1 in
      -c|--channel)
        shift
        CHANNEL=$1
        shift
      ;;
      --alt-ver)
        shift
        ALTVER=$1
        shift
      ;;
      -h|--help)
        print_usage
        exit 0
      ;;
      *)
      if [ $POSARGS -eq "0" ]; then
        SDK_TYPE=$1
        shift
        POSARGS=$((POSARGS+1))
      elif [ $POSARGS -eq "1" ];then
        MAJ_MIN_VER=$1
        shift
        POSARGS=$((POSARGS+1))
      else
        echoerr "\nERROR: \"$1\" is an unknown arguement "
        exit 1
      fi
      ;;
    esac
  done
  if [ -z "${SDK_TYPE}" ]; then
    echoerr "Error: please spcify an SDK_TYPE\n"
    exit 1
  fi
  if [ -z "${MAJ_MIN_VER}" ]; then
    echoerr "Error: please spcify a MAJ_MIN_VER\n"
    exit 1
  fi
}

source ~/.profile

# ----- Build Variables --------------------------------------------------------
REPO_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../../../../../" && pwd )"
LIN_APP_DIR="${REPO_DIR}"/source/ti/ti154stack/apps/linux
COMPONENT_NAME="linux_gw"

# ----- Script Arguements Parse ------------------------------------------------
# SDK_TYPE is IGNORED, the linux_gw pkg is only meant to be consumed by as
# SDK for now. It's is being kept to keep a consistent gen_conan_pkg API
SDK_TYPE=""
MAJ_MIN_VER=""
CHANNEL="ga"
ALTVER=""
arg_parse "$@"

# ----- Setup Conan ------------------------------------------------------------
# Need Env Vars: CONAN_LOGIN_USERNAME & CONAN_PASSWORD
source conan_setup.sh

# ----- Clean Environment for Conan --------------------------------------------
echo -e "Running WET Removal Scripts"
find "${REPO_DIR}"/environment -name "*rm*" -printf '%p\n' -exec {} \; &>/dev/null
find "${LIN_APP_DIR}"/environment -name "*rm*" -printf '%p\n' -exec {} \; &>/dev/null
git clean -xdf > /dev/null
rm -rf ${LIN_APP_DIR}/example/google/protobuf

# ----- intsalling conan tool packages -----------------------------------------
conan install "${LIN_APP_DIR}" --no-imports -g virtualenv
source activate.sh
#source "${CONAN_TOOLS_INSTALL_DIR}"/tools/conan_utility.sh
#record_wet_hashes "${LIN_APP_DIR}"

if [ "${CHANNEL}" = "eng" ] || [ "${CHANNEL}" = "ga" ]; then
  # ----- Update Conang Package Build Number -----------------------------------
  git fetch --tags --prune
  VERSION=$(python3 "${CONAN_TOOLS_INSTALL_DIR}"/tools/conan_ver_update.py \
    "$COMPONENT_NAME" "${MAJ_MIN_VER}")
  echo "Version = ${VERSION}"
  source deactivate.sh
  # ----- Create and Push Conan Package Version Tag ----------------------------
  if [[ -n "${PUBLISH:-}" ]]; then
    git tag -a "${COMPONENT_NAME}-${VERSION}" -m "CONAN $VERSION"
    git push origin --tags
  fi
else
  if [ -z "${ALTVER}" ]; then
    VERSION=$( git rev-parse --short HEAD )
  else
    VERSION=${ALTVER}
  fi
fi

# ----- Run WET ----------------------------------------------------------------
# WET cannot be run form conan cuz conan only cps the working dir to the ws,
# not the whole repo (six dirs up)
echo -e "Running WET"
"${LIN_APP_DIR}"/environment/setup.sh -ij

# ----- Create Packages for all build types ------------------------------------
git rev-parse HEAD > ./git_commit_id.txt

export CONAN_LOGGING_LEVEL=10
export CONAN_PRINT_RUN_COMMANDS=1
conan create ./ "${COMPONENT_NAME}"/"${VERSION}"@library-lprf/"${CHANNEL}"

# ----- Upload Packages --------------------------------------------------------
if [[ -n "${PUBLISH:-}" ]]; then
  echo -e "\nUploading built packages to conan remote"
  conan upload -r art --all --confirm "${COMPONENT_NAME}"/"${VERSION}"*
fi

