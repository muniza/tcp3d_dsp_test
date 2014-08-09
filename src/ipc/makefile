vpath % /opt/ti/ipc_3_00_04_29/packages/ti/sdo/
vpath %.c /opt/ti/xdctools_3_25_05_94/packages/

XOPTS = -I"/opt/ti/xdctools_3_25_05_94/packages/" -Dxdc_target_types__=/opt/ti/xdctools_3_25_05_94/packages/ti/targets/elf/std.h -Dxdc_target_name__=C66

CCOPTS ?= -mv6600 --abi=eabi -q -mi10 -mo -pdr -pden -pds=238 -pds=880 -pds1110  --program_level_compile -o3 -g --optimize_with_debug 

XDC_ROOT  = /opt/ti/xdctools_3_25_05_94/packages/

IPC_ROOT = /opt/ti/ipc_3_00_04_29/packages/ti/sdo/

BUILD_DEFS =  -Dti_sdo_ipc_MessageQ_traceFlag__D=FALSE -Dti_sdo_ipc_family_ti81xx_InterruptDucati_videoProcId__D=65535 -Dti_sdo_ipc_family_ti81xx_InterruptDucati_hostProcId__D=65535 -Dti_sdo_ipc_family_ti81xx_InterruptDucati_vpssProcId__D=65535 -Dti_sdo_ipc_family_ti81xx_InterruptDucati_dspProcId__D=65535 -Dti_sdo_ipc_family_ti81xx_InterruptDucati_ducatiCtrlBaseAddr__D=1073745920 -Dti_sdo_ipc_family_ti81xx_InterruptDucati_mailboxBaseAddr__D=1208778752

BIOS_INC  = -I"/opt/ti/bios_6_37_00_20/packages"

IPC_INC    = -I"/opt/ti/ipc_3_00_04_29/packages"

UTILS_INC    = -I"/opt/ti/ipc_3_00_04_29/packages"

CC = /opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x -c $(CCOPTS) -I /opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include

ASM = /opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x -c $(CCOPTS) -I /opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include

AR = /opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/ar6x rq

DEL = rm -f
CP = cp -f

define RM
	$(if $(wildcard $1),$(DEL) $1,:)
endef

define ASSEMBLE
	@echo asme66 $< ...
	@$(ASM) $(BUILD_DEFS) $(XOPTS) $(IPC_INC) $<
endef


all: ipc.ae66


Ipc.obj: ipc/Ipc.c utils/NameServer.c utils/NameServerRemoteNull.c utils/MultiProc.c utils/List.c ipc/GateMP.c ipc/MessageQ.c ipc/SharedRegion.c ipc/Notify.c ipc/heaps/HeapMemMP.c ipc/notifyDrivers/NotifyDriverCirc.c ipc/family/tci663x/Interrupt.c ipc/family/tci663x/MultiProcSetup.c ipc/family/tci663x/NotifyCircSetup.c ipc/gates/GateHWSem.c ipc/gates/GatePeterson.c ipc/gates/GateMPSupportNull.c makefile
	@-$(call RM, $@)
	@echo cle66 $< ...
	@$(CC) $(BUILD_DEFS) $(XOPTS) $(BIOS_INC) \
	$(IPC_INC) $(UTILS_INC) \
	$(IPC_ROOT)ipc/Ipc.c \
	$(IPC_ROOT)utils/NameServer.c \
	$(IPC_ROOT)utils/NameServerRemoteNull.c \
	$(IPC_ROOT)utils/MultiProc.c \
	$(IPC_ROOT)utils/List.c \
	$(IPC_ROOT)ipc/GateMP.c \
	$(IPC_ROOT)ipc/MessageQ.c \
	$(IPC_ROOT)ipc/SharedRegion.c \
	$(IPC_ROOT)ipc/Notify.c \
	$(IPC_ROOT)ipc/heaps/HeapMemMP.c \
	$(IPC_ROOT)ipc/notifyDrivers/NotifyDriverCirc.c \
	$(IPC_ROOT)ipc/family/tci663x/Interrupt.c \
	$(IPC_ROOT)ipc/family/tci663x/MultiProcSetup.c \
	$(IPC_ROOT)ipc/family/tci663x/NotifyCircSetup.c \
	$(IPC_ROOT)ipc/gates/GateHWSem.c \
	$(IPC_ROOT)ipc/gates/GatePeterson.c \
	$(IPC_ROOT)ipc/gates/GateMPSupportNull.c \

ipc.ae66: Ipc.obj 
	@echo are66 $^ ...
	@-$(call RM, $@)
	@$(AR) $@ $^


clean:
	@-$(call RM, ipc.ae66)
	@-$(call RM, Ipc.obj)
