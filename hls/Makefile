XILINX_INCLUDE := -I/opt/Xilinx/Vivado/2018.3/include
CFLAGS := -D_EN_TESTBENCH_

all : main

main :
	@vivado_hls -f sim.tcl 

hls :
	@vivado_hls -f synth.tcl

.PHONY : clean

clean :
	@rm -rf bin

