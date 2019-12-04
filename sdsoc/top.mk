#+-----------------------------------------------------------------------------
# Makefile for building SDSoC design
#
# Requirements:
#	1. source SDx
#	2. source Vivado
# 
#+-----------------------------------------------------------------------------

BOARD := Pynq-Z1
PLATFORM := 

P_SIZE := 0
TOOL_VERSION := 2018.3
ECHO := @echo

ifeq ($(BOARD),Pynq-Z1)
# non-cacheable buffers, PL clock 100MHz
PROJ := gemm_i8i32
P_CACHEABLE = 0
P_SIZE = 0
CLK_ID = 2 
endif
ifeq ($(BOARD),RFSoC)
# cacheable buffers, PL clock 250MHz
PROJ := gemm_i8f32_preload_tile_16x16_tree_4x
P_CACHEABLE = 1
P_SIZE = 1
CLK_ID = 1
endif

.PHONY: all
all : help check_env design
	$(ECHO) "Projects for $(BOARD) built successfully!"

design:
	make -f example.mk BOARD=$(BOARD) PLATFORM=$(PLATFORM) NAME=$(PROJ) \
	CLK_ID=$(CLK_ID) P_SIZE=$(P_SIZE) \
	P_CACHEABLE=$(P_CACHEABLE)

info:
	sds++ -sds-pf-info $(PLATFORM)

clean: 
	rm -rf .Xil

cleanall: clean
	rm -rf Pynq-Z1
	rm -rf RFSoC

.SILENT:
check_env:
ifeq ($(shell "env" | grep "XILINX" | grep "SDX" | grep $(TOOL_VERSION)),)
	$(ECHO) "ERROR: Please source SDx $(TOOL_VERSION) settings."
	exit 1
endif
ifeq ($(shell "env" | grep "XILINX" | grep "Vivado" | grep $(TOOL_VERSION)),)
	$(ECHO) "ERROR: Please source Vivado $(TOOL_VERSION) settings."
	exit 1
endif

help:
	$(ECHO) "Makefile Usage:"
	$(ECHO)
	$(ECHO) "example"
	$(ECHO) "-------"
	$(ECHO) "make BOARD=<target_board> PLATFORM=<platform_location>"
	$(ECHO) "   Compile source for the specified SDx platform." 
	$(ECHO) "   A shared object is created for direct use on the target board."
	$(ECHO)
	$(ECHO) "options"
	$(ECHO) "-------"
	$(ECHO) "info"
	$(ECHO) "   Get platform information, including available clock ID's"
	$(ECHO)
	$(ECHO) "clean"
	$(ECHO) "   Remove generated files for the specified board"
	$(ECHO)
	$(ECHO) "cleanall"
	$(ECHO) "   Remove all generated files"
	$(ECHO)
	$(ECHO) "Internal Arguments"
	$(ECHO) "------------------"
	$(ECHO) "P_SIZE"
	$(ECHO) "   hardware configuration parameter. P_SIZE=0 to fit on Pynq-Z1"
	$(ECHO) "   eg. 0 or 1 -> increases resource utilisation if equal to 1"
	$(ECHO) "   In general, set P_SIZE to 1 for Zynq Ultrascale"
	$(ECHO) "P_CACHEABLE"
	$(ECHO) "   whether to allocate cacheable or non-cacheable for sds calls"
	$(ECHO) "   In general, set P_CACHEABLE to 1 for Zynq Ultrascale"
	$(ECHO) "CLK_ID"
	$(ECHO) "   platform clock id"
	$(ECHO) "   ranging from 0 to the number of clocks specified in platform"
	$(ECHO) "TOOL_VERSION"
	$(ECHO) "   Version of Xilinx tools (SDx and Vivado)"
	$(ECHO)
	$(ECHO) "User Arguments"
	$(ECHO) "--------------"
	$(ECHO) "PLATFORM" 
	$(ECHO) "   path to the SDx platform (.pfm) file" 
	$(ECHO) "   eg. home/usr/sdx-platforms/Pynq-Z1/bare"
	$(ECHO) "BOARD"
	$(ECHO) "   name of the board (default is Pynq-Z1)"
	$(ECHO) "   Currently supported board: "
	$(ECHO) "       Pynq-Z1"
	$(ECHO) "       RFSoC"
	$(ECHO)
