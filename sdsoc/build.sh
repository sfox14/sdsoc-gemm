#!/bin/bash

make -f top.mk BOARD=Pynq-Z1 PLATFORM=/opt/Xilinx/SDx/2018.3/platforms/zc702 PROJ=gemm_i8f32_preload_tile_8x8_tree_2x_z1 CLK_ID=2
#make -f top.mk BOARD=Pynq-Z1 PLATFORM=/opt/Xilinx/SDx/2018.3/platforms/zc702 PROJ=gemm_i8f32_preload_tile_8x8_tree_1x_z1 CLK_ID=2
#make -f top.mk BOARD=Pynq-Z1 PLATFORM=/opt/Xilinx/SDx/2018.3/platforms/zc702 PROJ=gemm_i8f32_preload_tile_16x16_tree_1x_z1 CLK_ID=4
#make -f top.mk BOARD=Pynq-Z1 PLATFORM=/opt/Xilinx/SDx/2018.3/platforms/zc702 PROJ=gemm_i8f32_preload_tile_8x8_tree_4x_z1 CLK_ID=4

make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_16x16_tree_4x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_16x16_tree_1x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_16x16_tree_2x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_16x16_tree_8x CLK_ID=1
make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_32x32_tree_1x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_32x32_tree_2x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_16x16_notree_4x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_16x16_notree_1x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_16x16_notree_2x CLK_ID=1
#make -f top.mk BOARD=RFSoC PLATFORM=/home/sean/PYNQ-derivative-overlays/sdx_platform/zcu111-slow/platforms/zcu111 PROJ=gemm_i8f32_preload_tile_32x32_notree_1x CLK_ID=1


