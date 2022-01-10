#!/bin/bash

. ./common.sh

rm -rf lprf-sd-installers software-developer-guides

git clone ssh://git@bitbucket.itg.ti.com/lprf/lprf-sd-installers.git

# Clone and generate documentation
git clone --depth 1 ssh://git@bitbucket.itg.ti.com/wcsdocs/software-developer-guides.git -b ${DOCS_BRANCH}
pushd ./software-developer-guides
make -f Makefile.ti154stack.cc13x0 html;
make -f Makefile.ti154stack.cc13x0 install INSTALL_ROOT=../../
make -f Makefile.ti154stack.linux html install INSTALL_ROOT=../../
popd
