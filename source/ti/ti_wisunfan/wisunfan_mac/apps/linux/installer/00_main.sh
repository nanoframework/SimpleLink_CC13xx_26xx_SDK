#!/bin/bash


if [ "x${1}x" == "xfastx" ]
then
    FAST_BUILD=1
else
    FAST_BUILD=0
fi

. ./common.sh

echo "#---------------------------------------"
echo "# REMOVE OLD: ${RELEASE_DIRECTORY}"
echo "#---------------------------------------"

rm -rf   ${RELEASE_DIRECTORY}
mkdir -p ${RELEASE_DIRECTORY}

echo "#----------------------------------------"
echo "# Checkout installer tools"
echo "#----------------------------------------"
bash ./checkout_installer.sh

echo "#----------------------------------------"
echo "# Copy embedded hex files"
echo "#----------------------------------------"
bash ./copy_hexfiles.sh

echo "#----------------------------------------"
echo "# Execute copyfile script"
echo "#----------------------------------------"
bash ./copy_files.sh

echo "#----------------------------------------"
echo "# Build GOOGLE components"
echo "#----------------------------------------"

if [ ${FAST_BUILD} == 1 ]
then
    echo "FAST BUILD - skipping google build"
    for x in ${RELEASE_DIRECTORY}/example/collector/*.proto
    do
	touch $x
    done
else
    bash ./build_google.sh
fi

echo "#----------------------------------------"
echo "# Build prebuilt deliverables"
echo "#----------------------------------------"
bash ./build_prebuilts.sh

echo "#----------------------------------------"
echo "# Copy prebuilts in place"
echo "#----------------------------------------"
bash ./copy_prebuilts.sh

echo "#----------------------------------------"
echo "# Create BBB tarball"
echo "#----------------------------------------"
bash ./make_bbb_tarball.sh

echo "#----------------------------------------"
echo "# Create release notes HTML"
echo "#----------------------------------------"
bash ./release_notes.sh

echo "#----------------------------------------"
echo "# Final cleanup..."
echo "#----------------------------------------"
bash ./final_cleanup.sh

echo "#----------------------------------------"
echo "# Execute bitrock"
echo "#----------------------------------------"
bash ./run_bitrock.sh
