open_project -reset prj 
add_files src/gemm_hw.cpp 
add_files src/fpga.h 
add_files src/main.cpp
set_top gemm_hw
open_solution -reset "solution1"
set_part {xczu28dr-ffvg1517-2-e}
create_clock -period 4 -name default
csynth_design
exit
