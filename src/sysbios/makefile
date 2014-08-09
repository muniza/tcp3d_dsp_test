
XOPTS = -I"/opt/ti/xdctools_3_25_05_94/packages/" -Dxdc_target_types__=/opt/ti/xdctools_3_25_05_94/packages/ti/targets/elf/std.h -Dxdc_target_name__=C66

vpath % /opt/ti/bios_6_37_00_20/packages/ti/sysbios/
vpath %.c /opt/ti/xdctools_3_25_05_94/packages/

CCOPTS    = -mv6600 --abi=eabi -q -mi10 -mo -pdr -pden -pds=238 -pds=880 -pds1110  --program_level_compile -o3 -g --optimize_with_debug 

XDC_ROOT  = /opt/ti/xdctools_3_25_05_94/packages/

BIOS_ROOT = /opt/ti/bios_6_37_00_20/packages/ti/sysbios/

BIOS_DEFS =  -Dti_sysbios_BIOS_swiEnabled__D=TRUE -Dti_sysbios_BIOS_taskEnabled__D=TRUE -Dti_sysbios_BIOS_clockEnabled__D=TRUE -Dti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS -Dti_sysbios_knl_Swi_DISABLE_ALL_HOOKS -Dti_sysbios_BIOS_smpEnabled__D=FALSE -Dti_sysbios_Build_useHwiMacros -Dti_sysbios_knl_Swi_numPriorities__D=16 -Dti_sysbios_knl_Task_deleteTerminatedTasks__D=TRUE -Dti_sysbios_knl_Task_numPriorities__D=16 -Dti_sysbios_knl_Clock_TICK_SOURCE=ti_sysbios_knl_Clock_TickSource_TIMER -Dti_sysbios_knl_Clock_TICK_MODE=ti_sysbios_knl_Clock_TickMode_PERIODIC -Dti_sysbios_hal_Core_delegate_getId=ti_sysbios_hal_CoreNull_getId__E -Dti_sysbios_hal_Core_delegate_interruptCore=ti_sysbios_hal_CoreNull_interruptCore__E -Dti_sysbios_hal_Core_delegate_lock=ti_sysbios_hal_CoreNull_lock__E -Dti_sysbios_hal_Core_delegate_unlock=ti_sysbios_hal_CoreNull_unlock__E -Dti_sysbios_hal_Core_numCores__D=1 -Dti_sysbios_hal_CoreNull_numCores__D=1 -Dti_sysbios_utils_Load_taskEnabled__D=TRUE -Dti_sysbios_utils_Load_swiEnabled__D=FALSE -Dti_sysbios_family_c64p_Hwi_dispatcherSwiSupport__D=TRUE -Dti_sysbios_family_c64p_Hwi_dispatcherTaskSupport__D=TRUE -Dti_sysbios_family_c64p_Hwi_dispatcherAutoNestingSupport__D=TRUE -Dti_sysbios_family_c64p_Hwi_dispatcherIrpTrackingSupport__D=TRUE -Dti_sysbios_knl_Semaphore_supportsEvents__D=FALSE -Dti_sysbios_family_c64p_Exception_enableExternalMPC__D=FALSE -Dti_sysbios_family_c64p_Exception_enablePrint__D=TRUE

BIOS_INC  = -I"/opt/ti/bios_6_37_00_20/packages/"

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
	@$(ASM) $(BIOS_DEFS) $(XOPTS) $(BIOS_INC) $<
endef

all: sysbios.ae66

c64p_Hwi_disp_always.obj: family/c64p/Hwi_disp_always.s64P makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=c64p_Hwi_disp_always.obj

c64p_Hwi_asm.obj: family/c64p/Hwi_asm.s62 makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=c64p_Hwi_asm.obj

c64p_Hwi_asm_switch.obj: family/c64p/Hwi_asm_switch.s62 makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=c64p_Hwi_asm_switch.obj

c64p_Clobber_asm.obj: family/c64p/Clobber_asm.s62 makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=c64p_Clobber_asm.obj

c64p_Exception_asm.obj: family/c64p/Exception_asm.s64P makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=c64p_Exception_asm.obj

c64p_MemoryProtect_asm.obj: family/c64p/MemoryProtect_asm.s64P makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=c64p_MemoryProtect_asm.obj

c62_TaskSupport_asm.obj: family/c62/TaskSupport_asm.s62 makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=c62_TaskSupport_asm.obj

timer64_Timer_asm.obj: timers/timer64/Timer_asm.s64P makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=timer64_Timer_asm.obj

dmtimer_Timer_asm.obj: timers/dmtimer/Timer_asm.s64P makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=dmtimer_Timer_asm.obj

sk_sk_cwrap.obj: ../sk/sk_cwrap.asm makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=sk_sk_cwrap.obj


BIOS.obj: BIOS.c family/c64p/EventCombiner.c family/c64p/Exception.c family/c64p/Hwi.c family/c64p/Hwi_startup.c family/c64p/TimestampProvider.c hal/Hwi.c hal/Hwi_stack.c hal/Hwi_startup.c hal/Timer.c hal/Cache.c knl/Clock.c knl/Idle.c knl/Intrinsics.c knl/Queue.c knl/Semaphore.c knl/Swi.c knl/Swi_andn.c knl/Task.c gates/GateHwi.c gates/GateAll.c gates/GateSwi.c gates/GateMutexPri.c gates/GateMutex.c syncs/SyncSem.c family/c66/Cache.c heaps/HeapBuf.c heaps/HeapMem.c family/c66/tci66xx/CpIntc.c xdcruntime/GateThreadSupport.c family/c62/TaskSupport.c family/c62/IntrinsicsSupport.c timers/timer64/Timer.c family/c64p/tci6488/TimerSupport.c makefile
	@-$(call RM, $@)
	@echo cle66 $< ...
	@$(CC) $(BIOS_DEFS) $(XOPTS) $(BIOS_INC) \
	$(BIOS_ROOT)BIOS.c \
	$(BIOS_ROOT)family/c64p/EventCombiner.c \
	$(BIOS_ROOT)family/c64p/Exception.c \
	$(BIOS_ROOT)family/c64p/Hwi.c \
	$(BIOS_ROOT)family/c64p/Hwi_startup.c \
	$(BIOS_ROOT)family/c64p/TimestampProvider.c \
	$(BIOS_ROOT)hal/Hwi.c \
	$(BIOS_ROOT)hal/Hwi_stack.c \
	$(BIOS_ROOT)hal/Hwi_startup.c \
	$(BIOS_ROOT)hal/Timer.c \
	$(BIOS_ROOT)hal/Cache.c \
	$(BIOS_ROOT)knl/Clock.c \
	$(BIOS_ROOT)knl/Idle.c \
	$(BIOS_ROOT)knl/Intrinsics.c \
	$(BIOS_ROOT)knl/Queue.c \
	$(BIOS_ROOT)knl/Semaphore.c \
	$(BIOS_ROOT)knl/Swi.c \
	$(BIOS_ROOT)knl/Swi_andn.c \
	$(BIOS_ROOT)knl/Task.c \
	$(BIOS_ROOT)gates/GateHwi.c \
	$(BIOS_ROOT)gates/GateAll.c \
	$(BIOS_ROOT)gates/GateSwi.c \
	$(BIOS_ROOT)gates/GateMutexPri.c \
	$(BIOS_ROOT)gates/GateMutex.c \
	$(BIOS_ROOT)syncs/SyncSem.c \
	$(BIOS_ROOT)family/c66/Cache.c \
	$(BIOS_ROOT)heaps/HeapBuf.c \
	$(BIOS_ROOT)heaps/HeapMem.c \
	$(BIOS_ROOT)family/c66/tci66xx/CpIntc.c \
	$(BIOS_ROOT)xdcruntime/GateThreadSupport.c \
	$(BIOS_ROOT)family/c62/TaskSupport.c \
	$(BIOS_ROOT)family/c62/IntrinsicsSupport.c \
	$(BIOS_ROOT)timers/timer64/Timer.c \
	$(BIOS_ROOT)family/c64p/tci6488/TimerSupport.c \

sysbios.ae66: BIOS.obj c64p_Hwi_disp_always.obj c64p_Hwi_asm.obj c64p_Hwi_asm_switch.obj c64p_Clobber_asm.obj c64p_Exception_asm.obj c64p_MemoryProtect_asm.obj c62_TaskSupport_asm.obj timer64_Timer_asm.obj dmtimer_Timer_asm.obj sk_sk_cwrap.obj
	@-$(call RM, $@)
	@echo are66 $^ ...
	@$(AR) $@ $^


clean:
	@-$(call RM, sysbios.ae66)
	@-$(call RM, BIOS.obj)
	@-$(call RM, c64p_Hwi_disp_always.obj)
	@-$(call RM, c64p_Hwi_asm.obj)
	@-$(call RM, c64p_Hwi_asm_switch.obj)
	@-$(call RM, c64p_Clobber_asm.obj)
	@-$(call RM, c64p_Exception_asm.obj)
	@-$(call RM, c64p_MemoryProtect_asm.obj)
	@-$(call RM, c62_TaskSupport_asm.obj)
	@-$(call RM, timer64_Timer_asm.obj)
	@-$(call RM, dmtimer_Timer_asm.obj)
	@-$(call RM, sk_sk_cwrap.obj)
