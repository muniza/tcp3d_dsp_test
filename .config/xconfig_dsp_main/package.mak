#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
MK_NOGENDEPS := $(filter clean,$(MAKECMDGOALS))
override PKGDIR = xconfig_dsp_main
XDCINCS = -I. -I$(strip $(subst ;, -I,$(subst $(space),\$(space),$(XPKGPATH))))
XDCCFGDIR = package/cfg/

#
# The following dependencies ensure package.mak is rebuilt
# in the event that some included BOM script changes.
#
ifneq (clean,$(MAKECMDGOALS))
/opt/ti/xdctools_3_25_05_94/packages/xdc/utils.js:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/utils.js
/opt/ti/xdctools_3_25_05_94/packages/xdc/xdc.tci:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/xdc.tci
/opt/ti/xdctools_3_25_05_94/packages/xdc/template.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/template.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/om2.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/om2.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/xmlgen.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/xmlgen.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/xmlgen2.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/xmlgen2.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/Warnings.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/Warnings.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/IPackage.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/IPackage.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/package.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/package.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/services/global/Clock.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/services/global/Clock.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/services/global/Trace.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/services/global/Trace.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/bld.js:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/bld.js
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/BuildEnvironment.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/BuildEnvironment.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/PackageContents.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/PackageContents.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/_gen.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/_gen.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Library.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Library.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Executable.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Executable.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Repository.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Repository.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Configuration.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Configuration.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Script.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Script.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Manifest.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Manifest.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Utils.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/Utils.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITarget.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITarget.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITarget2.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITarget2.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITarget3.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITarget3.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITargetFilter.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/ITargetFilter.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/bld/package.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/bld/package.xs
package.mak: config.bld
/opt/ti/xdctools_3_25_05_94/packages/ti/targets/ITarget.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/ti/targets/ITarget.xs
/opt/ti/xdctools_3_25_05_94/packages/ti/targets/C28_large.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/ti/targets/C28_large.xs
/opt/ti/xdctools_3_25_05_94/packages/ti/targets/C28_float.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/ti/targets/C28_float.xs
/opt/ti/xdctools_3_25_05_94/packages/ti/targets/package.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/ti/targets/package.xs
/opt/ti/xdctools_3_25_05_94/packages/ti/targets/elf/ITarget.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/ti/targets/elf/ITarget.xs
/opt/ti/xdctools_3_25_05_94/packages/ti/targets/elf/package.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/ti/targets/elf/package.xs
package.mak: package.bld
/opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/compiler.opt.xdt:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/compiler.opt.xdt
/opt/ti/xdctools_3_25_05_94/packages/xdc/services/io/File.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/services/io/File.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/services/io/package.xs:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/services/io/package.xs
/opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/compiler.defs.xdt:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/compiler.defs.xdt
/opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/custom.mak.exe.xdt:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/custom.mak.exe.xdt
/opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/package.xs.xdt:
package.mak: /opt/ti/xdctools_3_25_05_94/packages/xdc/tools/configuro/template/package.xs.xdt
endif

ti.targets.elf.C66.rootDir ?= /opt/ti/ccsv5/tools/compiler/c6000_7.4.4
ti.targets.elf.packageBase ?= /opt/ti/xdctools_3_25_05_94/packages/ti/targets/elf/
.PRECIOUS: $(XDCCFGDIR)/%.oe66
.PHONY: all,e66 .dlls,e66 .executables,e66 test,e66
all,e66: .executables,e66
.executables,e66: .libraries,e66
.executables,e66: .dlls,e66
.dlls,e66: .libraries,e66
.libraries,e66: .interfaces
	@$(RM) $@
	@$(TOUCH) "$@"

.help::
	@$(ECHO) xdc test,e66
	@$(ECHO) xdc .executables,e66
	@$(ECHO) xdc .libraries,e66
	@$(ECHO) xdc .dlls,e66


all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.inc package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.inc package/%_xconfig_dsp_main.c package/%.defs.h: %.xdc $(PKGCFGS)
	@$(MSG) generating interfaces for package xconfig_dsp_main" (because $@ is older than $(firstword $?))" ...
	$(XSRUN) -f xdc/services/intern/cmd/build.xs $(MK_IDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

.dlls,e66 .dlls: dsp_main.cfg.pe66

-include package/cfg/dsp_main.cfg_pe66.mak
-include package/cfg/dsp_main.cfg_pe66.cfg.mak
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/dsp_main.cfg_pe66.dep
endif
dsp_main.cfg.pe66: package/cfg/dsp_main.cfg_pe66.xdl
	@


ifeq (,$(wildcard .libraries,e66))
dsp_main.cfg.pe66 package/cfg/dsp_main.cfg_pe66.c: .libraries,e66
endif

package/cfg/dsp_main.cfg_pe66.c package/cfg/dsp_main.cfg_pe66.h package/cfg/dsp_main.cfg_pe66.xdl: override _PROG_NAME := dsp_main.cfg.xe66
package/cfg/dsp_main.cfg_pe66.c: package/cfg/dsp_main.cfg_pe66.cfg

clean:: clean,e66
	-$(RM) package/cfg/dsp_main.cfg_pe66.cfg
	-$(RM) package/cfg/dsp_main.cfg_pe66.dep
	-$(RM) package/cfg/dsp_main.cfg_pe66.c
	-$(RM) package/cfg/dsp_main.cfg_pe66.xdc.inc

clean,e66::
	-$(RM) dsp_main.cfg.pe66
.executables,e66 .executables: dsp_main.cfg.xe66

dsp_main.cfg.xe66: |dsp_main.cfg.pe66

-include package/cfg/dsp_main.cfg.xe66.mak
dsp_main.cfg.xe66: package/cfg/dsp_main.cfg_pe66.oe66 
	$(RM) $@
	@$(MSG) lnke66 $@ ...
	$(RM) $(XDCCFGDIR)/$@.map
	$(ti.targets.elf.C66.rootDir)/bin/lnk6x -w -q -u _c_int00 -fs $(XDCCFGDIR)$(dir $@)  -q -o $@ package/cfg/dsp_main.cfg_pe66.oe66   package/cfg/dsp_main.cfg_pe66.xdl --abi=eabi -c -m $(XDCCFGDIR)/$@.map -l $(ti.targets.elf.C66.rootDir)/lib/libc.a
	
dsp_main.cfg.xe66: export C_DIR=
dsp_main.cfg.xe66: PATH:=$(ti.targets.elf.C66.rootDir)/bin/:$(PATH)

dsp_main.cfg.test test,e66 test: dsp_main.cfg.xe66.test

dsp_main.cfg.xe66.test:: dsp_main.cfg.xe66
ifeq (,$(_TESTLEVEL))
	@$(MAKE) -R -r --no-print-directory -f $(XDCROOT)/packages/xdc/bld/xdc.mak _TESTLEVEL=1 dsp_main.cfg.xe66.test
else
	@$(MSG) running $<  ...
	$(call EXEC.dsp_main.cfg.xe66, ) 
endif

clean,e66::
	-$(RM) .tmp,dsp_main.cfg.xe66,0,*


clean:: clean,e66

clean,e66::
	-$(RM) dsp_main.cfg.xe66
clean:: 
	-$(RM) package/cfg/dsp_main.cfg_pe66.pjt
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@
dsp_main.cfg_pe66.oe66,copy : package/cfg/dsp_main.cfg_pe66.oe66
dsp_main.cfg_pe66.se66,copy : package/cfg/dsp_main.cfg_pe66.se66

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg .interfaces $(XDCROOT)/packages/xdc/cfg/Main.xs
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_XSOPTS) xdc.cfg $(_PROG_NAME) $(XDCCFGDIR)$*.cfg $(XDCCFGDIR)$*

.PHONY: release,xconfig_dsp_main
xconfig_dsp_main.tar: package/package.bld.xml
xconfig_dsp_main.tar: package/build.cfg
xconfig_dsp_main.tar: package/package.xdc.inc
ifeq (,$(MK_NOGENDEPS))
-include package/rel/xconfig_dsp_main.tar.dep
endif
package/rel/xconfig_dsp_main/xconfig_dsp_main/package/package.rel.xml: .force
	@$(MSG) generating external release references $@ ...
	$(XS) $(JSENV) -f $(XDCROOT)/packages/xdc/bld/rel.js $(MK_RELOPTS) . $@

xconfig_dsp_main.tar: package/rel/xconfig_dsp_main.xdc.inc package/rel/xconfig_dsp_main/xconfig_dsp_main/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKRELTAR,package/rel/xconfig_dsp_main.xdc.inc,package/rel/xconfig_dsp_main.tar.dep)


release release,xconfig_dsp_main: all xconfig_dsp_main.tar
clean:: .clean
	-$(RM) xconfig_dsp_main.tar
	-$(RM) package/rel/xconfig_dsp_main.xdc.inc
	-$(RM) package/rel/xconfig_dsp_main.tar.dep

clean:: .clean
	-$(RM) .libraries .libraries,*
clean:: 
	-$(RM) .dlls .dlls,*
#
# The following clean rule removes user specified
# generated files or directories.
#

ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard package))
    $(shell $(MKDIR) package)
endif
ifeq (,$(wildcard package/cfg))
    $(shell $(MKDIR) package/cfg)
endif
ifeq (,$(wildcard package/lib))
    $(shell $(MKDIR) package/lib)
endif
ifeq (,$(wildcard package/rel))
    $(shell $(MKDIR) package/rel)
endif
ifeq (,$(wildcard package/internal))
    $(shell $(MKDIR) package/internal)
endif
endif
clean::
	-$(RMDIR) package

include custom.mak
clean:: 
	-$(RM) package/xconfig_dsp_main.pjt
