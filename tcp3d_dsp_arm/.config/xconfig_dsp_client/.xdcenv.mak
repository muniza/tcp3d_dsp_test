#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /opt/ti/edma3_lld_02_11_11_15/packages;/opt/ti/ipc_3_00_04_29/packages;/opt/ti/bios_6_37_00_20/packages;/opt/ti/pdk_keystone2_3_00_04_18/packages;/opt/ti/salld_keystone2_02_00_03_00/packages;/opt/ti/uia_1_03_02_10/packages;/opt/ti/ccsv5/ccs_base;/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/exampleProjects/messageQ/.config
override XDCROOT = /opt/ti/xdctools_3_25_05_94
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /opt/ti/edma3_lld_02_11_11_15/packages;/opt/ti/ipc_3_00_04_29/packages;/opt/ti/bios_6_37_00_20/packages;/opt/ti/pdk_keystone2_3_00_04_18/packages;/opt/ti/salld_keystone2_02_00_03_00/packages;/opt/ti/uia_1_03_02_10/packages;/opt/ti/ccsv5/ccs_base;/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/exampleProjects/messageQ/.config;/opt/ti/xdctools_3_25_05_94/packages;..
HOSTOS = Linux
endif
