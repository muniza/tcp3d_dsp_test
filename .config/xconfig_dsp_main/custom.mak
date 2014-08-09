## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd package/cfg/dsp_main.cfg_pe66.oe66

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/dsp_main.cfg_pe66.xdl
	$(SED) 's"^\"\(package/cfg/dsp_main.cfg_pe66cfg.cmd\)\"$""\"/home/muniza/mcsdk/pdk_keystone2_3_00_04_18/packages/exampleProjects/rmTCP3DTest/.config/xconfig_dsp_main/\1\""' package/cfg/dsp_main.cfg_pe66.xdl > $@
	-$(SETDATE) -r:max package/cfg/dsp_main.cfg_pe66.h compiler.opt compiler.opt.defs
