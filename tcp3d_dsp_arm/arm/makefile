#
# Macro definitions referenced below
#

empty =
space =$(empty) $(empty)

PDK_INSTALL_PATH ?= ../../..
# IPC_DEVKIT_INSTALL_PATH ?= need/to/set/path
# export IPC_DEVKIT_INSTALL_PATH if IPC is not in /usr/lib
# Currently requiring IPC from linux-devkit because that package is compiled with Linaro tools

ARMV7OBJDIR ?= ./obj/$(DEVICE)
ARMV7BINDIR ?= ./bin/$(DEVICE)
ARMV7LIBDIR ?= ./lib

ARMV7OBJDIR := $(ARMV7OBJDIR)/rm/test
ARMV7BINDIR := $(ARMV7BINDIR)/rm/test

#Cross tools
ifdef CROSS_TOOL_INSTALL_PATH
# Support backwards compatibility with KeyStone1 approach
 CC = $(CROSS_TOOL_INSTALL_PATH)/$(CROSS_TOOL_PRFX)gcc
 AC = $(CROSS_TOOL_INSTALL_PATH)/$(CROSS_TOOL_PRFX)as
 AR = $(CROSS_TOOL_INSTALL_PATH)/$(CROSS_TOOL_PRFX)ar
 LD = $(CROSS_TOOL_INSTALL_PATH)/$(CROSS_TOOL_PRFX)gcc
endif

ifdef IPC_DEVKIT_INSTALL_PATH
  IPCLIBDIRS := -L$(IPC_DEVKIT_INSTALL_PATH)/lib/
else
  IPCLIBDIRS :=
endif

# INCLUDE Directories
RM_INC_DIR = $(PDK_INSTALL_PATH)/ti/drv/rm

RM_SRC_DIR ?= $(PDK_INSTALL_PATH)/ti/drv/rm
RM_TEST_DIR = $(RM_SRC_DIR)/test
RM_ARM_LIN_TEST_DIR = $(RM_TEST_DIR)/$(DEVICE)/armv7/linux

INCDIR := $(PDK_INSTALL_PATH);$(RM_INC_DIR);$(RM_ARM_LIN_TEST_DIR);$(IPC_DEVKIT_INSTALL_PATH)/include;$(INTC_DIR);$(INTC_DIR2)

# Libraries
RM_LIB = -lrm
DAEMON_LIB = -ldaemon
IPC_LIBS = -ltiipc -ltiipcutils -lticmem

ifeq ($(USEDYNAMIC_LIB), yes)
#presuming ARM executable would depend on dynamic library dependency
EXE_EXTN = _so
LIBS     = $(RM_LIB) $(DAEMON_LIB)
else
#forcing ARM executable to depend on static LLD libraries
EXE_EXTN =
LIBS     = -static $(RM_LIB) -Wl,-Bdynamic $(DAEMON_LIB)
endif

# Compiler options
INTERNALDEFS = $(DEBUG_FLAG) -D__ARMv7 -DDEVICE_K2K -D_VIRTUAL_ADDR_SUPPORT -D__LINUX_USER_SPACE -D_LITTLE_ENDIAN=1 -DMAKEFILE_BUILD

# Linker options
INTERNALLINKDEFS = -Wl,--start-group -L $(ARMV7LIBDIR) $(LIBS) -lrt -Wl,--end-group -pthread $(LDFLAGS)
IPC_INTERNALLINKDEFS = -Wl,--start-group -L $(ARMV7LIBDIR) $(IPCLIBDIRS) $(IPC_LIBS) -lrt -Wl,--end-group -pthread $(LDFLAGS)

RM_SERVER_EXE=rmServer$(EXE_EXTN).out
RM_LINUX_CLIENT_TEST_EXE=rmLinuxClientTest$(EXE_EXTN).out
RM_DSP_CLIENT_TEST_EXE=rmDspClientTest$(EXE_EXTN).out
TCP3D_ARM_TEST_EXE=tcp3d_arm_test$(EXE_EXTN).out

OBJEXT = o 

SRCDIR = $(RM_ARM_LIN_TEST_DIR)

INCS = -I. -I$(strip $(subst ;, -I,$(INCDIR)))

VPATH=$(SRCDIR)

#List the Source Files
RM_SERVER_SRC =     \
    rm_server.c     \
    rm_server_osal.c \
    sockutils.c
    
RM_LINUX_CLIENT_TEST_SRC = \
    rm_linux_client_test.c \
    rm_linux_osal.c        \
    sockutils.c

RM_DSP_CLIENT_TEST_SRC = \
    rm_dsp_client_test.c \
    sockutils.c    

TCP3D_ARM_TEST_SRC = \
    tcp3d_arm_test.c \
    sockuitls.c
    
# FLAGS for the SourceFiles
SRC_CFLAGS = -I. $(CFLAGS)

# Make Rule for the SRC Files
RM_SERVER_SRC_OBJS = $(patsubst %.c, $(ARMV7OBJDIR)/%.$(OBJEXT), $(RM_SERVER_SRC))
RM_LINUX_CLIENT_TEST_SRC_OBJS = $(patsubst %.c, $(ARMV7OBJDIR)/%.$(OBJEXT), $(RM_LINUX_CLIENT_TEST_SRC))
RM_DSP_CLIENT_TEST_SRC_OBJS = $(patsubst %.c, $(ARMV7OBJDIR)/%.$(OBJEXT), $(RM_DSP_CLIENT_TEST_SRC))
TCP3D_ARM_TEST_SRC_OBJS = $(patsubst %.c, $(ARMV7OBJDIR)/%.$(OBJEXT), $(TCP3D_ARM_TEST_SRC))

all:$(ARMV7BINDIR)/$(RM_SERVER_EXE) $(ARMV7BINDIR)/$(RM_LINUX_CLIENT_TEST_EXE) $(ARMV7BINDIR)/$(RM_DSP_CLIENT_TEST_EXE) $(ARMV7BINDIR)/$(TCP3D_ARM_TEST_EXE)

$(ARMV7BINDIR)/$(RM_SERVER_EXE): $(RM_SERVER_SRC_OBJS) $(ARMV7BINDIR)/.created
	@echo linking $(RM_SERVER_SRC_OBJS) into $@ ...
	@$(CC) $(RM_SERVER_SRC_OBJS) $(INTERNALLINKDEFS) -o $@
$(ARMV7BINDIR)/$(RM_LINUX_CLIENT_TEST_EXE): $(RM_LINUX_CLIENT_TEST_SRC_OBJS) $(ARMV7BINDIR)/.created
	@echo linking $(RM_LINUX_CLIENT_TEST_SRC_OBJS) into $@ ...
	@$(CC) $(RM_LINUX_CLIENT_TEST_SRC_OBJS) $(INTERNALLINKDEFS) -o $@
$(ARMV7BINDIR)/$(RM_DSP_CLIENT_TEST_EXE): $(RM_DSP_CLIENT_TEST_SRC_OBJS) $(ARMV7BINDIR)/.created
	@echo linking $(RM_DSP_CLIENT_TEST_SRC_OBJS) into $@ ...
	@$(CC) $(RM_DSP_CLIENT_TEST_SRC_OBJS) $(IPC_INTERNALLINKDEFS) -o $@	
$(ARMV7BINDIR)/$(TCP3D_ARM_TEST_EXE): $(TCP3D_ARM_TEST_SRC_OBJS) $(ARMV7BINDIR)/.created
	@echo linking $(TCP3D_ARM_TEST_SRC_OBJS) into $@ ...
	@$(CC) $(TCP3D_ARM_TEST_SRC_OBJS) $(IPC_INTERNALLINKDEFS) -o $@	

$(ARMV7OBJDIR)/%.$(OBJEXT): %.c $(ARMV7OBJDIR)/.created
	@echo compiling $< ...
	@$(CC) -c $(SRC_CFLAGS) $(INTERNALDEFS) $(INCS)  $< -o $@

$(ARMV7OBJDIR)/.created:
	@mkdir -p $(ARMV7OBJDIR)
	@touch $(ARMV7OBJDIR)/.created

$(ARMV7BINDIR)/.created:
	@mkdir -p $(ARMV7BINDIR)
	@touch $(ARMV7BINDIR)/.created

clean:
	@rm -fr $(ARMV7OBJDIR)
	@rm -fr $(ARMV7BINDIR)

