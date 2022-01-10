import fileinput
import re
import wget
import os
import stat
import shutil
# ToDo: add warpper for subprocess the changes the call depending ont python
#       version.
import subprocess
import tarfile

## Helper Function #############################################################
def isProcInstallationValid(install_pth):
    if os.path.isdir(install_pth):
        return True
    else:
        return False

def make_tarfile(output_filename, source_dir):
    with tarfile.open(output_filename, "w:gz") as tar:
        tar.add(source_dir, arcname=os.path.basename(source_dir))

## Global Variables ############################################################
# build_root = os.path.abspath("./")
build_root = os.path.dirname(os.path.realpath(__file__))
import_dir = os.path.join(build_root,".sdk/imports")
exprot_dir = os.path.join(build_root,".sdk/exports")
prebuilt_dir = os.path.join(build_root,"prebuilt")

## Update Build Root ###########################################################
def update_build_root(new_build_root):
    global build_root
    global import_dir
    global exprot_dir
    global prebuilt_dir

    build_root = os.path.abspath(new_build_root)
    import_dir = os.path.join(build_root,".sdk/imports")
    exprot_dir = os.path.join(build_root,".sdk/exports")
    prebuilt_dir = os.path.join(build_root,"prebuilt")


## Setup Build ENV #############################################################
def setup_build_env():
    if os.path.isdir(import_dir):
        shutil.rmtree(import_dir)
    os.mkdir(import_dir)

    if os.path.isdir(exprot_dir):
       shutil.rmtree(exprot_dir)
    os.mkdir(exprot_dir)

## Determine Processor SDK version #############################################
def fetch_proc_sdk_version():
    proc_page_url = ("http://software-dl.ti.com/processor-sdk-linux/esd/AM335X/"
                    "latest/index_FDS.html")
    proc_page_pth = os.path.join(import_dir,proc_page_url.split('/')[-1])

    if os.path.isfile(proc_page_pth):
        os.remove(proc_page_pth)
    print("Downloading Processor SDK page...")
    wget.download(proc_page_url,out=proc_page_pth,bar=wget.bar_thermometer)
    print("")

    proc_ver_und=''
    proc_re="PROCESSOR\\-SDK\\-LINUX\\-AM335X\\s(\\d+_\\d+_\\d+_\\d+)"
    with open(proc_page_pth,'r') as f:
        for line in f:
            match = re.search(proc_re,line)
            if match:
                proc_ver_und = match.group(1)
                break

    proc_ver_dot =proc_ver_und.replace('_','.')

    if len(proc_ver_und) == 0:
        print("Error Match Not found Error OUT!!")
    else:
        print("Prcocessor SDK version {}".format(proc_ver_dot))

    return proc_ver_dot

## Download & Install Processor SDK ############################################
def install_proc_sdk(version):
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
            wget.download(proc_dld_url,
                          out=proc_installer,
                          bar=wget.bar_thermometer)
            print("")

        subprocess.call("chmod +x {}".format(proc_installer),shell=True)
        subprocess.call("echo 'Y' | {}".format(proc_installer),shell=True)
    else:
        print("Prcocessor SDK installation found. skipping isntalation...")

## Install Required Packages ###################################################
def install_linux_req_pkg(pkg_list):
    print("Installing Required Packages...")

    for pkg in pkg_list:
        subprocess.call("sudo apt-get -y install {}".format(pkg), shell=True)

## Build Google Component ######################################################
def build_google_component():
    print("Building Google Component...")
    exmp_google_dir = os.path.join(build_root,"example/google")
    subprocess.call("cd {} && make dist-clean".format(exmp_google_dir),shell=True)
    subprocess.call("cd {} && make all".format(exmp_google_dir),shell=True)

## Create Host Collector Binaries ##############################################
# Two flavors of the collector app need to be built: sug_g and 2_4g
# currently acomplised by modifying the make file and seding in compiler flags
# Todo: Ask ti154stack team to add another target on the make file (too messy)
def create_host_collector_bin():
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


## Copy Prebuits to Prebuilt Dir ###############################################
def copy_prebuilts():
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

## create BBB tarball ##########################################################
def create_prebuilt_tar():
    tar_name = os.path.join(build_root,"bbb_prebuilt.tar.gz")
    tar_dest = os.path.join(prebuilt_dir,"bbb_prebuilt.tar.gz")

    if os.path.isfile(tar_name):
        os.remove(tar_name)
    if os.path.isfile(tar_dest):
        os.remove(tar_dest)

    make_tarfile(tar_name, prebuilt_dir)
    os.rename(tar_name,tar_dest)
