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


#if P_SIZE == 0 // Pynq-Z1 @100MHz
#define VECTOR_WIDTH 16
#define NUM_PES 16
#define R_DEPTH 15 // A row depth
#define C_DEPTH 133 // A col depth
#define PSUM_DEPTH 256
#define TREE_STAGES 5
#define TPARAM 5 // the min latency parameter
#elif P_SIZE == 1
#define VECTOR_WIDTH 32 
#define NUM_PES 32 
#define R_DEPTH 15 // A row depth: NUM_PES*R_DEPTH <= 480/512
#define C_DEPTH 133 // A col depth: VECTOR_WIDTH*C_DEPTH <= 4480/4608
#define PSUM_DEPTH 1024 // Result[] Map to LUTRAM
#define TREE_STAGES 6 //
#define TPARAM 12
#else
#define VECTOR_WIDTH 8
#define NUM_PES 8
#define R_DEPTH 15
#define C_DEPTH 133
#define PSUM_DEPTH 1024
#define TREE_STAGES 4
#define TPARAM 5
#endif


typedef int8_t data_t;

typedef hls::stream< data_t > fifo_t;
typedef hls::stream< float > fifoB_t;

void hw_accel_float(float *A, int arow, float *B, int brow, float *C, int ccol, int batch);

#ifdef __cplusplus
extern "C" {
#endif
// Hardware functions
#pragma SDS data access_pattern(A:SEQUENTIAL, B:SEQUENTIAL, C:SEQUENTIAL, bscale:SEQUENTIAL)
#pragma SDS data copy(A[0:(acopy*brow)], B[0:(batch*brow*ccol)], C[0:(batch*arow*ccol)], bscale[0:(batch)])
#pragma SDS data buffer_depth(B:32)
#pragma SDS data data_mover(A:AXIDMA_SIMPLE, B:AXIDMA_SIMPLE, C:AXIDMA_SIMPLE, bscale:AXIDMA_SIMPLE)
#if P_CACHEABLE == 0
#pragma SDS data mem_attribute(A:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, \
	B:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, \
	C:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, \
	bscale:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
#else
#pragma SDS data mem_attribute(A:PHYSICAL_CONTIGUOUS, \
	B:PHYSICAL_CONTIGUOUS, \
	C:PHYSICAL_CONTIGUOUS, \
	bscale:PHYSICAL_CONTIGUOUS)
#endif
void gemm_hw(data_t *A, int arow, data_t *B, int brow, float *C, int ccol,
		int acopy, int batch, int ctrl, int TAw, int TAr, float ascale, float *bscale);

#ifdef __cplusplus
}
#endif
