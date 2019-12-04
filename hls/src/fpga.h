#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hls_stream.h>
#include <ap_int.h>
#include <ap_fixed.h>
using namespace std;

#define DO_PRAGMA_INNER(x) _Pragma(#x)
#define DO_PRAGMA(x) DO_PRAGMA_INNER(x)

#define BITWIDTH 8

// gemm_i8f32_preload_tile_16x16_tree

/*
#if P_SIZE == 0 // Pynq-Z1 @100MHz
#define VECTOR_WIDTH 4
#define NUM_PES 8
#define R_DEPTH 20 // A row depth
#define C_DEPTH 20 // A col depth
#define PSUM_DEPTH 40
#define TREE_STAGES 3
#define TPARAM 1 // the min latency parameter
#elif P_SIZE == 1
#define VECTOR_WIDTH 4
#define NUM_PES 16
#define R_DEPTH 20
#define C_DEPTH 20
#define PSUM_DEPTH 40
#define TREE_STAGES 3
#define TPARAM 1
#else
#define VECTOR_WIDTH 8
#define NUM_PES 16
#define R_DEPTH 20
#define C_DEPTH 20
#define PSUM_DEPTH 40
#define TREE_STAGES 4
#define TPARAM 1
#endif
*/
#define VECTOR_WIDTH 16 //8
#define NUM_PES 16 //32 //28
#define R_DEPTH 29 //16 // A row depth: NUM_PES*R_DEPTH <= 480/512
#define C_DEPTH 261 //576 // 160 // A col depth: VECTOR_WIDTH*C_DEPTH <= 4480/4608
#define PSUM_DEPTH 1024 // Result[] be NUM_PEs URAMS
#define TREE_STAGES 5 //
#define TPARAM 8


typedef int8_t data_t;

typedef hls::stream< data_t > fifo_t;
typedef hls::stream< float > fifoB_t;

void hw_accel_float(float *A, int arow, float *B, int brow, float *C, int ccol, int batch);

#ifdef __cplusplus
extern "C" {
#endif
// Hardware functions
//#pragma SDS data access_pattern(A:SEQUENTIAL, B:SEQUENTIAL, C:SEQUENTIAL, bscale:SEQUENTIAL)
//#pragma SDS data copy(A[0:(aload*brow)], B[0:(batch*brow*ccol)], C[0:(batch*arow*ccol)], bscale[0:(batch)])
//#pragma SDS data buffer_depth(B:32)
//#pragma SDS data data_mover(A:AXIDMA_SIMPLE, B:AXIDMA_SIMPLE, C:AXIDMA_SIMPLE, bscale:AXIDMA_SIMPLE)
//#if P_CACHEABLE == 0
//#pragma SDS data mem_attribute(A:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, \
//	B:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, \
//	C:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, \
//	bscale:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
//#else
//#pragma SDS data mem_attribute(A:PHYSICAL_CONTIGUOUS, \
//	B:PHYSICAL_CONTIGUOUS, \
//	C:PHYSICAL_CONTIGUOUS, \
//	bscale:PHYSICAL_CONTIGUOUS)
//#endif
void gemm_hw(data_t *A, int arow, data_t *B, int brow, float *C, int ccol,
		int aload, int batch, int ctrl, int TAw, int TAr, float ascale, float *bscale);

#ifdef __cplusplus
}
#endif
