open_project -reset prj
set_top gemm_hw
add_files src/gemm_hw.cpp
add_files src/fpga.h
add_files -tb src/main.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu28dr-ffvg1517-2-e} -tool vivado
create_clock -period 4 -name default
csim_design
exit
