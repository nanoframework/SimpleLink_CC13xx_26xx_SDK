#! /bin/bash
#
# Script to create and upload a component package for a particular SDK.
#
# Requirements:
#    - Environment varialbes "CONAN_LOGIN_USERNAME"  and "CONAN_PASSWORD" have to be defined. 
#      "CONAN_LOGIN_USERNAME" has to be an existing user in artifactory.itg.ti.com with 
#      permisions to write to the "conan-epd-cmcu-software-release-local" repo.
#      "CONAN_PASSWORD" is the password for "CONAN_LOGIN_USERNAME"
#
#    - Virtual Box 5.2.12r122591 installed
#
#    - Vagrant 2.1.1 Installed
#
# Usage:
# ./gen_conan_pkg.sh
#
##########################################################################################
function print_usage(){
  echo -e "Usage:"
  echo -e "  gen_conan_pkg [-p USERNAME PASSWORD]"
  echo -e ""
  echo -e "Options:"
  echo -e "  -p --publish USERNAME PASSWORD  tags the commit used to create the conan package"
  echo -e "                                  and uploads the pacakge into artifactory."
  echo -e "                                  USERNAME PASSWORD credentials to upload pkg"
}

arg_parse() {
    # Argument parsing
    while (($#)); do
      case $1 in
        -p|--publish)
        if [ $# -gt '2' ];then
          shift
          CONAN_LOGIN_USERNAME="$1"
          shift
          CONAN_PASSWORD="$1"
          shift
          PUBLISH="YES"
        else
          print_usage
          echo -e "Error: insuficient arguements pass onto --publish \n" >&2
          exit 1
        fi
        ;;
        *)
        if [ $1 != '-h' ] && [ $1 != '--help' ]; then
                echo -e "\t$1 is an unknown arugement... "
        fi
        print_usage
        exit 0
        ;;
      esac
    done
}

REPO_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../../../../../" && pwd )"
LIN_APP_DIR="${REPO_DIR}/source/ti/ti154stack/apps/linux"

CONAN_LOGIN_USERNAME=''
CONAN_PASSWORD=''
PUBLISH=''
arg_parse "$@"

# ----- Verify Vagrant Virtualbox are installed --------------------------------
vagrant --version > "/dev/null"
if [ ! $? -eq "0" ];then
  echo "Error: no vagrant installation found (verify PATH env var)" >&2
  exit 1
fi
vboxmanage --version > "/dev/null"
if [ ! $? -eq "0" ];then
  echo "Error: no vagrant installation found (verify PATH env var)" >&2
  exit 1
fi

# ----- Clean Environment for Conan --------------------------------------------
echo -e "Running WET Removal Scripts"
find ${REPO_DIR}/environment -name "*rm*" -printf '%p\n' -exec {} &>/dev/null \;
find ${LIN_APP_DIR}/environment -name "*rm*" -printf '%p\n' -exec {} &>/dev/null \;
echo
rm -rf ${LIN_APP_DIR}/example/google/protobuf

# ----- Create Conan Pakg on Vagrant Machine -----------------------------------
if [ -n ${CONAN_LOGIN_USERNAME} ];then
  export CONAN_LOGIN_USERNAME
  export CONAN_PASSWORD
fi

# ----- Bootup Vagrant VM ------------------------------------------------------
vagrant box list | grep 'ti[\/\\]bionic64' || \
vagrant box add http://msp430.sc.ti.com/vagrant/trusty64.json

echo "provisioning vagrant"
vagrant up --provision
vagrant reload
vagrant rsync

vagrant ssh -c "\
chmod 600 /home/vagrant/.ssh/id_rsa;\
export PUBLISH=${PUBLISH};\
cd /home/vagrant/154stack-dev/lprf-dallas-154stack/source/ti/ti154stack/apps/linux;\
./vagrant_gen_conan_pkg.sh;\
"
