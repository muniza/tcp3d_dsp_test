################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
CpIntc_local.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/CpIntc_local.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="CpIntc_local.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

dsp_main.obj: ../dsp_main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="dsp_main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: ../dsp_main.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"/opt/ti/xdctools_3_25_05_94/xs" --xdcpath="/opt/ti/edma3_lld_02_11_11_15/packages;/opt/ti/ipc_3_00_04_29/packages;/opt/ti/bios_6_37_00_20/packages;/opt/ti/pdk_keystone2_3_00_04_18/packages;/opt/ti/salld_keystone2_02_00_03_00/packages;/opt/ti/uia_1_03_02_10/packages;/opt/ti/ccsv5/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.elf.C66 -p ti.platforms.evmTCI6638K2K -r debug -c "/opt/ti/ccsv5/tools/compiler/c6000_7.4.4" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: | configPkg/linker.cmd
configPkg/: | configPkg/linker.cmd

sample_cfg.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/sample_cfg.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="sample_cfg.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sample_cs.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/sample_cs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="sample_cs.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sample_init.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/sample_init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="sample_init.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sample_int_reg.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/sample_int_reg.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="sample_int_reg.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_betaState.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/src/tcp3d_betaState.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_betaState.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_codeBlkSeg.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/tcp3d_codeBlkSeg.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_codeBlkSeg.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_data.obj: ../tcp3d_data.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_data.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_drv.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/src/tcp3d_drv.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_drv.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_drv_sample_init.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/tcp3d_drv_sample_init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_drv_sample_init.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_inputConfigPrep.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/tcp3d_inputConfigPrep.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_inputConfigPrep.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_itg.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/src/tcp3d_itg.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_itg.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_osal.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/example/k2k/c66/bios/tcp3d_osal.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_osal.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_reg.obj: /home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/src/tcp3d_reg.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_reg.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

tcp3d_test_functions.obj: ../tcp3d_test_functions.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -g --include_path="/opt/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/rm" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/qmss" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/k2k/c66/bios" --include_path="/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/ti/drv/tcp3d/test/src" --define=DEVICE_K2K --define=USE_TCP3D_DRIVER_TYPES --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tcp3d_test_functions.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


