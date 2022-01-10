## Version: ${VERSION} 
This document is for version __${VERSION}__ ${RELEASE_TYPE} (GA= General Availability) build.

## Introduction
The TI 15.4-Stack released as part of the SimpleLink CC13x0 SDK supports a Star network topology for Sub-1GHz applications. TI 15.4-Stack software runs on TI’s SimpleLink&trade; Sub-1GHz wireless microcontrollers, including the CC1310 and CC1350. Devices running TI 15.4-Stack offer key benefits, such as longer range in the FCC band and better protection against in-band interference through use of frequency hopping. TI 15.4-Stack provides a complete end-to-end long range wireless application development solution to reduce time and cost of getting your product to market quickly.

Developers targeting products based on TI 15.4-Stack-2.3.0 have two architecture choices for their product development. First option is to have entire TI 15.4-Stack application and stack can run within the SimpleLink CC13x0 MCU (as an embedded application) while the second option is to have the application running on a host (running Linux OS, etc) interfacing to the SimpleLink ULP CC13x0 MCU running the MAC Co-Processor Application.
 
This TI 15.4-Stack Linux SDK provides example Linux host applications, tools, documentation that allow to develop products using the architecture option two using a Linux host interfacing with the CC13x0 running MAC CoProcessor as described above. 

## Documentation
The following documentation is available for this release.
* TI 15.4-Stack [(overview/web-site)](http://www.ti.com/tool/ti-15.4-stack)
* TI 15.5-Stack Linux® Gateway Example Application – Quick Start Guide [(web-site)](./docs/ti-15.4-stack-linux-quick-start-guide.html)
* CC13x0 SimpleLink&trade; TI 15.4-Stack 2.x.x Linux® Developer's Guide [(web-site)](./docs/ti-15.4-stack-linux-users-guide.html)
* TI 15.4-Stack CoProcessor Interface Guide [(local PDF)](./docs/ti-15.4-stack-cop-interface-guide.pdf)
* [Software License](./license_ti15.4stack_linux_x64_${VERSION}.txt)
* [Software Manifest](./manifest_ti15.4stack_linux_x64_${VERSION}.html)


## New Features
${linux_timac_nf_2_03_00}


## Fixed Issues
${linux_timac_fi_2_03_00}


## Known Issues
${linux_timac_ki_2_03_00}


## Upgrade and Compatibility Information
* To migrate your current Linux Application based on TI 15.4-Stack-2.2.0 Linux Collector Example Application to TI 15.4-Stack-2.3.0, it is preferred that developers bring in the application specific changes done over the out of box collector example application into the new project files. The changes/updates in the TI 15.4-Stack-2.3.0 release primarily are in the files collector.c/h, cllc.c/h, csf.c/h.
* It is recommended to use the MAC CoP application from the TI 15.4-Stack-2.3.0 included in the SimpleLink CC13x0 SDK. 

## Operating System & Dependencies
The TI 15.4-Stack Linux SDK requires:
* Ubuntu 14.04 LTS (64bit, not 32bit)
* [TI Processor SDK Linux AM335x](http://www.ti.com/tool/PROCESSOR-SDK-AM335x)
    (Tested with: ti-processor-sdk-linux-am335x-evm-04.00.00.04)

## Device and Development Board Support
Supported Devices
* [SimpleLink ULP CC1310](http://www.ti.com/product/CC1310)
* [SimpleLink Dual Band ULP CC1350](http://www.ti.com/product/CC1350)
* [Sitara AM335x](http://www.ti.com/lsds/ti/processors/sitara/arm_cortex-a8/am335x/overview.page)

Supported Development Platforms
* [CC1310 LaunchPad](http://www.ti.com/tool/LAUNCHXL-CC1310)
* [CC1350 LaunchPad](http://www.ti.com/tool/LAUNCHXL-CC1350)
* [BeagleBone Black - Sitara AM335x](http://www.ti.com/tool/beaglebk)


## Versioning
This product follows a version format, **M.mm.pp.bb**, where:
* **M** is a 1 digit major number,
* **mm** is a 2 digit minor number,
* **pp** is a 2 digit patch number,
* **bb** is a 2 digit incrementing build counter.

## Technical Support and Product Updates
* Visit the [TI 15.4-Stack Linux SDK](http://www.ti.com/tool/ti-15.4-stack)
* Visit the [TI Processor SDK E2E page](https://e2e.ti.com/support/arm/sitara_arm)
* Visit the [TI Sub-1GHz Overview](http://www.ti.com/lsds/ti/wireless_connectivity/sub-1_ghz/overview.page)
* Visit the [TI 15.4-Stack Wiki](http://processors.wiki.ti.com/index.php/TI_15.4-Stack_Wiki)
* Visit the [Sub-1GHz E2E Forum](https://e2e.ti.com/support/wireless_connectivity/proprietary_sub_1_ghz_simpliciti)
* Visit the [TI E2E Forum](https://e2e.ti.com)
