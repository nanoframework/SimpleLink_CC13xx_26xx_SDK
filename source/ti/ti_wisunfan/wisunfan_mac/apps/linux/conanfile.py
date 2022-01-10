#!/usr/bin/env python3

from conans import ConanFile, tools
import fileinput
import os
import re
import requests
import shutil
import subprocess
import sys
import tarfile

## Helper Function #############################################################
def request_download(file_url,file_name=""):
    if not file_name:
        file_name = file_url.split('/')[-1]

    # ToDo: Add some form of progress bar to download process
    r = requests.get(file_url,stream = True)
    with open(file_name,'wb') as f:
        for chunk in r.iter_content(chunk_size=1024*1024):
            if chunk:
                f.write(chunk)

def isProcInstallationValid(install_pth):
    if os.path.isdir(install_pth):
        return True
    else:
        return False

def make_tarfile(output_filename, source_dir):
    with tarfile.open(output_filename, "w:gz") as tar:
        tar.add(source_dir, arcname=os.path.basename(source_dir))

def setup_build_env(import_dir):
    if os.path.isdir(import_dir):
        shutil.rmtree(import_dir)
    os.mkdir(import_dir)

def fetch_proc_sdk_version():

    srch_ptr = r'(?<=^bbb_TI_PROC_SDK_DIR=\${HOME}[\/\\]ti-processor-sdk-linux-am335x-evm-)\d+\.\d+\.\d+\.\d+'
    target_file = "scripts/front_matter.mak"

    proc_ver = ''
    with open(target_file,'r') as f:
        for line in f:
            ver = re.search(srch_ptr,line)
            if ver:
                proc_ver = ver.group(0)
                break

    assert (len(proc_ver) > 0),"No Processor SDK version found!"

    return proc_ver

def install_proc_sdk(version,import_dir):
    proc_ver_dot = version
    proc_ver_und = proc_ver_dot.replace('.','_')

    proc_inst_pth = ("/home/vagrant/ti-processor-sdk-linux-am335x-"
                    "evm-{}".format(proc_ver_dot))
    proc_dld_url = ("http://software-dl.ti.com/processor-sdk-linux/esd/AM335X/"
                "{}/exports/ti-processor-sdk-linux-am335x-evm-{}-Linux-x86-"
                "Install.bin".format(proc_ver_und,proc_ver_dot))
    proc_installer = os.path.join(import_dir,proc_dld_url.split('/')[-1])

    if isProcInstallationValid(proc_inst_pth) == False:
        if os.path.isfile(proc_installer) == False:
            print("Downloading Processor SDK... standby")
            request_download(proc_dld_url,proc_installer)
            print("Download Finished")

        subprocess.call("chmod +x {}".format(proc_installer),shell=True)
        subprocess.call("echo 'Y' | {}".format(proc_installer),shell=True)
    else:
        print("Prcocessor SDK installation found. skipping isntalation...")

def install_linux_req_pkg(pkg_list):
    print("Installing Required Packages...")

    for pkg in pkg_list:
        subprocess.call("sudo apt-get -y install {}".format(pkg), shell=True)

def build_google_component(build_root):
    print("Building Google Component...")
    exmp_google_dir = os.path.join(build_root,"example/google")
    subprocess.call("cd {} && make dist-clean".format(exmp_google_dir),shell=True)
    subprocess.call("cd {} && make all".format(exmp_google_dir),shell=True)

# Two flavors of the collector app need to be built: sug_g and 2_4g
# currently acomplised by modifying the make file and seding in compiler flags
# Todo: Ask ti154stack team to add another target on the make file (too messy)
def create_host_collector_bin(build_root):
    print("Building Collector Binaries...")

    ## Building 2_4g collector app
    col_make = os.path.join(build_root,"example/collector/Makefile")
    col_make_bu = os.path.join(build_root,"example/collector/Makefile_Backup")

    shutil.copyfile(col_make,col_make_bu)

    srch_ptrn="CFLAGS += -I${SDK_HOME}"
    rplc_str="CFLAGS += -I${SDK_HOME}\nCFLAGS += -DOAD_BLOCK_SIZE=64"
    for line in fileinput.input(col_make,inplace=True):
        # inside this loop the STDOUT will be redirected to the file
        # the comma after each print statement is needed to avoid double line breaks
        print(line.replace(srch_ptrn,rplc_str),end="")

    subprocess.call("cd {} && ./build_all.sh clean".format(build_root),shell=True)
    subprocess.call("cd {} && ./build_all.sh host".format(build_root),shell=True)

    col_out = os.path.join(build_root,"example/collector/host_collector")
    col_out_2_4g = os.path.join(build_root,"example/collector/host_collector_2_4g")
    shutil.copy2(col_out,col_out_2_4g)

    os.remove(col_make)
    os.rename(col_make_bu,col_make)

    ## Build sub_1 collector app
    subprocess.call("cd {} && ./build_all.sh clean".format(build_root),shell=True)
    subprocess.call("cd {} && ./build_all.sh host".format(build_root),shell=True)
    subprocess.call("cd {} && ./build_all.sh bbb".format(build_root),shell=True)

def copy_prebuilts(build_root,prebuilt_dir):
    print("Copying prebuilt files int prebuilt dir")
    prebuilt_bin_dir = os.path.join(prebuilt_dir,"bin")
    prebuilt_gw_dir = os.path.join(prebuilt_dir,"gateway")

    if os.path.isdir(prebuilt_bin_dir):
        shutil.rmtree(prebuilt_bin_dir)

    if os.path.isdir(prebuilt_gw_dir):
        shutil.rmtree(prebuilt_gw_dir)

    src_dir = os.path.join(build_root,"example/gateway")
    shutil.copytree(src_dir,prebuilt_gw_dir)

    os.mkdir(prebuilt_bin_dir)
    src_dir = os.path.join(build_root,"example/collector")
    shutil.copy2(os.path.join(src_dir,"ship-collector.cfg"),
                 os.path.join(src_dir,"collector.cfg"))
    shutil.copy2(os.path.join(src_dir,"ship-collector-2_4.cfg"),
                 os.path.join(src_dir,"collector-2_4.cfg"))
    shutil.copy2(os.path.join(src_dir,"host_collector"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"host_collector_2_4g"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"bbb_collector"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"collector.cfg"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"collector-2_4.cfg"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"run_collector.sh"),prebuilt_bin_dir)

    src_dir = os.path.join(build_root,"example/npi_server2")
    shutil.copy2(os.path.join(src_dir,"apimac-msgs.cfg"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"host_npi_server2"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"bbb_npi_server2"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"npi_server2.cfg"),prebuilt_bin_dir)

    src_dir = os.path.join(build_root,"example/cc13xx-sbl/app/linux")
    shutil.copy2(os.path.join(src_dir,"host_cc13xx-sbl"),prebuilt_bin_dir)
    shutil.copy2(os.path.join(src_dir,"bbb_cc13xx-sbl"),prebuilt_bin_dir)

def create_prebuilt_tar(build_root,prebuilt_dir):
    tar_name = os.path.join(build_root,"bbb_prebuilt.tar.gz")
    tar_dest = os.path.join(prebuilt_dir,"bbb_prebuilt.tar.gz")

    if os.path.isfile(tar_name):
        os.remove(tar_name)
    if os.path.isfile(tar_dest):
        os.remove(tar_dest)

    make_tarfile(tar_name, prebuilt_dir)
    os.rename(tar_name,tar_dest)

class Ti154stackConan(ConanFile):
    name = "linux_gw"
    url = "ssh://git@bitbucket.itg.ti.com/lpc_sw_rnd/lprf-dallas-154stack.git"
    description = "Main content for the ti154stack linux GW SDK"

    # include git information in build
    # scm = {
    #    "type": "git",
    #    "url": "ssh://git@bitbucket.itg.ti.com/lpc_sw_rnd/lprf-dallas-154stack.git",
    #    "revision": "auto"
    # }

    build_requires = ("conan_tools/1.02.00.03@library-lprf/test")

    # required package for build process
    pkg_list = ["dh-autoreconf" , "pkg-config"]

    # point to the root dir to say what content is needed to complete build phase
    exports_sources = "*"

    def build(self):
        # ----- Define build varaibles -----------------------------------------
        build_root = str(self.build_folder)
        import_dir = os.path.join(build_root,".sdk/imports")
        prebuilt_dir = os.path.join(build_root,"prebuilt")

        # ----- Run WET --------------------------------------------------------
        # self.run("cd ./environment && bash ./setup.sh -ij")

        # ----- Run build fuctions ---------------------------------------------
        setup_build_env(import_dir)
        proc_ver = fetch_proc_sdk_version()
        self.output.info("Prcocessor SDK version {}".format(proc_ver))
        install_proc_sdk(proc_ver,import_dir)
        install_linux_req_pkg(self.pkg_list)
        build_google_component(build_root)
        create_host_collector_bin(build_root)
        copy_prebuilts(build_root, prebuilt_dir)
        create_prebuilt_tar(build_root, prebuilt_dir)

        # ----- bring in pre-built hex files -----------------------------------
        self.output.info("Fetching prebuilds from firmware directory")
        self.run(sys.executable + " " +
                 "${CONAN_TOOLS_INSTALL_DIR}/tools/swet.py " +
                 "./.sdk/swet_agama_firmware.json ./")

        # ----- Updating legal header ------------------------------------------
        self.output.info("Updating legal headers...")
        header_dirs = ["./example", "./scripts", "./components"]
        for dir in header_dirs:
            self.run(
                sys.executable
                + " ${CONAN_TOOLS_INSTALL_DIR}/tools/update_hdr.py"
                + " --enable-inline-comment"
                + " {}".format(dir)
                + " linux_gateway"
            )

        # ----- Create SDK Image -----------------------------------------------
        whitelist_folder = '.' +os.path.sep+ '.sdk' +os.path.sep+ 'whitelists'
        whitelist_yaml_file = whitelist_folder +os.path.sep+ 'linux_gw.yaml'
        wl_path = os.path.abspath(whitelist_yaml_file)

        # srch_ptrn="\\"
        # rplc_str="/"
        # for line in fileinput.input(wl_path,inplace=True):
        #     # inside this loop the STDOUT will be redirected to the file
        #     # the comma after each print statement is needed to avoid double line breaks
        #     print(line.replace(srch_ptrn,rplc_str),end="")
        
        tools.rmdir('./.sdk/exports/image')

        print("Starting Yaml2Dir Image Creation")
        self.run(sys.executable + ' ${CONAN_TOOLS_INSTALL_DIR}/tools/yaml2dir.py '+ 
                 wl_path + " . ./.sdk/exports/image Release Linux")

    def package(self):
        self.copy("*", src="./.sdk/exports/image")

