#!/bin/bash

# WET Tool setup script, this script resolved WET dependencies for 15.4.
main() {
    # Import utility functions
    SETUP_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    LINUX_APP_DIR="$( cd ${SETUP_SCRIPT_DIR}/../ && pwd )"
    REPO_DIR="$( cd ${LINUX_APP_DIR}/../../../../../ && pwd )"
    WET_ROOT="$( cd ${REPO_DIR}/.. && pwd )"

    TI_PROXY=wwwgate.ti.com:80
    NO_PROXY=.ti.com

    # Setup TI Proxy settings
    export https_proxy=${TI_PROXY}
    export http_proxy=${TI_PROXY}
    export HTTP_PROXY=${TI_PROXY}
    export HTTPS_PROXY=${TI_PROXY}
    export FTPS_PROXY=${TI_PROXY}
    export FTP_PROXY=${TI_PROXY}
    export NO_PROXY=${NO_PROXY}
    export no_proxy=${NO_PROXY}


    mkdir -p ${SETUP_SCRIPT_DIR}/.ignore
    # Obtain and install latest WET module version
    unameOut="$(uname -s)"
    if [ ${unameOut} != "Linux" ] && \
       [ ${unameOut} != "Mac" ]
    then
        PY_CALL="py -3"
    else
        PY_CALL="python3"
    fi

    echo "Installing the latest version of WET"
    WET_DIR="${WET_ROOT}/wet"
    if [ -d "${WET_DIR}" ]; then
        pushd "${WET_DIR}"
        git clean -xdf;
        git reset --hard
        git pull
        popd
    else
        git clone "ssh://git@bitbucket.itg.ti.com/lprf/wet.git" "${WET_DIR}"
    fi
    ${PY_CALL} -m pip install "${WET_DIR}" --user

    # Run WET Tool for 15.4
    ${PY_CALL} -m environment -r ${REPO_DIR} \
        -d ${LINUX_APP_DIR}/environment/dependencies.xml \
        -m ${LINUX_APP_DIR}/environment/.ignore/ $@
}

main "$@"