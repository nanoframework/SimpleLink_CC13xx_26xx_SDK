#! /bin/bash

# usage: conan_setup.sh
#
# scritp to check for conan installation and setup.
#
# Environment Variables:
#  - CONAN_USER must be defined, conan user for art repo
#  - CONAN_PASS must be defiend, conan pass for art repo
#

echoerr() { echo -e "$@" 1>&2; }

set -e

echo "########################################"
echo "conan_setup.sh"
echo "########################################"

# Check for conan installation
CONAN_VER=$(conan --version || true)
if [[ -z "${CONAN_VER}" ]]; then
  echo "Conan Installation not Detected..."
  echo "Attempting to install Conan..."
  echo "CMD: python3 -m pip install conan==1.22 --user --proxy=http://webproxy.ext.ti.com:80"
  python3 -m pip install conan==1.22 \
    --user \
    --proxy=http://webproxy.ext.ti.com:80
  source ~/.profile
  echo ""
fi

# setup conan
CONAN_SETUP_TEST=$(conan remote list | grep "art:" || true)
if [[ -z "${CONAN_SETUP_TEST}" ]]; then
  echo "Conan Config not setup as expected..."
  echo "Setting up Conan Config..."
  echo "CMD: conan config install ssh://git@bitbucket.itg.ti.com/conan/conan-config.git"
  conan config install ssh://git@bitbucket.itg.ti.com/conan/conan-config.git
  echo ""
fi

# setup conan ART credentials
CONAN_CRED_TEST=$(conan user -r art | grep 'anonymous' || true)
if [[ ! -z "${CONAN_CRED_TEST}" ]]; then
  echo "Conan Credentials for art are not setup"
  if [[ -z "${CONAN_USER}" ]] || [[ -z "${CONAN_PASS}" ]]; then
    echoerr "No conan credentials found, Please define CONAN_USER and CONAN_PASS"
    exit 10
  fi
  echo "Setting up credentials..."
  echo "conan user -p \${CONAN_PASS} -r art \${CONAN_USER}"
  conan user -p "${CONAN_PASS}" -r art "${CONAN_USER}"
fi

echo -e "Done, Conan is setup\n"

