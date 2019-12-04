#include "fpga.h"
#include <math.h>

template <class T>  T reg(T x) {
#pragma HLS pipeline
#pragma HLS inline self off
#pragma HLS interface ap_ctrl_none register port=return

	return x;
}

template <class T> T sreg(T x, int n){
#pragma HLS pipeline
#pragma HLS inline self off
#pragma HLS interface ap_ctrl_none register port=return

	int i;
	T val = x;
	for (i=0; i<n; i++){
		T val = reg(val);
	}
	return val;
}

void hw_accel_float(float *A, int arow, float *B, int brow, float *C, int ccol, int batch)
{

    /*
    GEMM:
     - preload matrix A
     - stream B, reuse A, stream C
     - optionally transpose A
     - optionally transpose B
     - compute C=AB
     - **Future: Tile matrix A to handle any problem size**
    */

    // compute

	for (int bt=0; bt<batch; bt++){

		float *Cf = C + bt*ccol*arow;
		float *Bf = B + bt*brow*ccol;

		// reset
		for (int i=0; i<arow*ccol; i++){
			Cf[i] = 0;
		}

		for (int j = 0; j < arow; j++) {
			for (int i = 0; i < ccol; i++) {
			  for (int k = 0; k < brow; k++) {

				Cf[i*arow + j] += A[j*brow+k] * Bf[i*brow+k];

			  }
			}
		 }

	}
}

void readA(data_t *A, data_t Amem[R_DEPTH*NUM_PES][C_DEPTH*VECTOR_WIDTH], int arow, int acol, int TAw, int TAr)
{

    int ii,jj,i,j;

    int temp = arow;
    if (TAw == 1 & TAr == 0){
      arow = acol;
      acol = temp;
    }


    int outer_loop = (arow + NUM_PES - 1)/NUM_PES;
    int inner_loop = (acol + VECTOR_WIDTH -1)/VECTOR_WIDTH;

    for (ii=0; ii<outer_loop; ii++){
        for (i=0; i<NUM_PES; i++){
            for (jj=0; jj<inner_loop; jj++){
                for (j=0; j<VECTOR_WIDTH; j++){
                    #pragma HLS PIPELINE
                    int idx = ii*NUM_PES + i;
                    int jdx = jj*VECTOR_WIDTH + j;

                    // Zero padding is necessary. Paralellism = VECTOR_WIDTH*NUM_PES
                    data_t aval = (idx < arow && jdx < acol) ? A[idx*acol + jdx] : 0;


                    if (TAw==0) {Amem[idx][jdx] = aval;}
                    else {Amem[jdx][idx] = aval;}

                    //printf("Access: A[%d], Read: %.6f, Write: Amem[%d][%d]\n", (idx*acol + jdx), aval, idx, jdx);

                }
            }
        }
    }
    
}


void readB(data_t *B, fifo_t Bmem[VECTOR_WIDTH], int brow, int ccol)
{

  data_t tile[VECTOR_WIDTH];

  int inner_loop = (brow + VECTOR_WIDTH -1)/VECTOR_WIDTH;

  int i,j,k,l,bt;

  for (i=0; i<ccol; i++){
	for (j=0; j<inner_loop; j++){
	  for (k=0; k<VECTOR_WIDTH; k++){
		#pragma HLS PIPELINE

		int jdx = j*VECTOR_WIDTH + k;

		data_t bval = (i < ccol && jdx < brow) ? B[i*brow + jdx] : 0;

		tile[k] = bval;

		//printf("Access: B[%d], Read: %.6f, Write: Bmem[%d][%d]\n", (i*brow + jdx), bval, i, jdx);
	  }

	  for (l=0; l<VECTOR_WIDTH; l++){
		#pragma HLS PIPELINE
		Bmem[l] << tile[l];
	  }

	}
  }


}



void writeC(fifoB_t &Cmem, float *C, int arow, int ccol)
{

  int i,j,k,l;

  for (i=0; i<ccol; i++){
	for (j=0; j<arow; j++){
	  Cmem >> C[i*arow + j];
	}
  }

}


void compute(data_t Amem[R_DEPTH*NUM_PES][C_DEPTH*VECTOR_WIDTH], fifo_t Bmem[VECTOR_WIDTH],
  fifoB_t &Cmem, int arow, int brow, int ccol, int TAr, float cscale)
{

  int outer_loop = (arow + NUM_PES - 1)/NUM_PES;
  int inner_loop = (brow + VECTOR_WIDTH -1)/VECTOR_WIDTH;

  int ii,jj,kk,i,j,k;

  static int psum[NUM_PES][VECTOR_WIDTH][TREE_STAGES];
  static int result[PSUM_DEPTH*NUM_PES];
  static data_t in_data[VECTOR_WIDTH];
  static data_t pipe_data[NUM_PES][VECTOR_WIDTH];
  static int pipe_arow[NUM_PES];
  static float pipe_cscale[NUM_PES];
  static int memfg[NUM_PES];
  static float fres[NUM_PES];


//#pragma HLS array_partition variable=result cyclic factor=8 dim=1
DO_PRAGMA (HLS array_partition variable=result cyclic factor=NUM_PES dim=1)
#pragma HLS array_partition variable=psum complete dim=1
#pragma HLS array_partition variable=psum complete dim=2
#pragma HLS array_partition variable=psum complete dim=3
#pragma HLS array_partition variable=in_data complete
#pragma HLS array_partition variable=pipe_data complete dim=1
#pragma HLS array_partition variable=pipe_data complete dim=2
#pragma HLS array_partition variable=pipe_arow complete
#pragma HLS array_partition variable=pipe_cscale complete 
#pragma HLS array_partition variable=memfg complete
#pragma HLS array_partition variable=fres complete

//#pragma HLS RESOURCE variable=result core=XPM_MEMORY uram
#pragma HLS RESOURCE variable=result core=RAM_2P_LUTRAM


fm:  for (int k=0; k<ccol; k++){
#pragma HLS LOOP_TRIPCOUNT min=1 max=10

in_ch:    for (int i=0; i<inner_loop; i++){
#pragma HLS LOOP_TRIPCOUNT min=1 max=7

      // dequeue FIFO
deq:      	 for (int iq=0; iq<VECTOR_WIDTH; iq++){
#pragma HLS UNROLL
        		Bmem[iq] >> in_data[iq];
      	  	 }

out_ch:      for (int j=0; j<outer_loop; j++){
#pragma HLS LOOP_TRIPCOUNT min=1 max=5

#pragma HLS PIPELINE
pe:		        for (int jj=0; jj<NUM_PES; jj++){
#pragma HLS UNROLL
//#pragma HLS LATENCY min=10
DO_PRAGMA (HLS LATENCY min=TPARAM)
pipe:				for (int pp=0; pp<VECTOR_WIDTH; pp++){
#pragma HLS UNROLL
						  pipe_data[jj][pp] = in_data[pp]; // pipeline register in broadcast tree
					   }
		
					   int jdx = j*NUM_PES + jj;
					   memfg[jj] = TAr;

vec:	          	for (int ii=0; ii<VECTOR_WIDTH; ii++){

						int idx = i*VECTOR_WIDTH+ii;

						data_t op1 = (memfg[jj]==1) ? Amem[idx][jdx] : Amem[jdx][idx];
						//data_t op1 = Amem[jdx][idx];
						data_t op2 = pipe_data[jj][ii]; //in_data[ii];
						int imm = op1 * op2;

						psum[jj][ii][0] = imm;


			  	  	}

tree: 				for (int col=1; col<(TREE_STAGES); col++) {
#pragma HLS UNROLL
add: 					for (int row=0; row < (VECTOR_WIDTH)>>col; row++) {
#pragma HLS UNROLL
							psum[jj][row][col] = psum[jj][2*row][col-1] + psum[jj][2*row +1][col-1];
						}
					}


					if (i==0){
						result[jdx] = psum[jj][0][TREE_STAGES-1]; //imm2;
					}else{
						result[jdx] += psum[jj][0][TREE_STAGES-1]; //imm2;
					}


				}
      	  }

    }



write_1:    for (int jw=0; jw<outer_loop; jw++){
write_0:      for (int jjw=0; jjw<NUM_PES; jjw++){
        #pragma HLS PIPELINE
				pipe_cscale[jjw] = cscale;
				pipe_arow[jjw] = arow;

				// multiply int result by float scale factor
				int imm = result[jw*NUM_PES + jjw]; // >> (ACCWIDTH-16);
				float immA = (float)imm; // this is expensive!
				fres[jjw] = pipe_cscale[jjw]*immA;
				float imm2 = fres[jjw];

				int jdx = jw*NUM_PES + jjw;

				if (jdx< pipe_arow[jjw]){ // don't append the hw padded results to output stream
				  Cmem << imm2;
				}

      	  }
    		}
    
  	  }

}



void gemm_hw(data_t *A, int arow, data_t *B, int brow, float *C, int ccol,
		int aload, int batch, int ctrl, int TAw, int TAr, float ascale, float *bscale)
{

    /*
    GEMM:
     - preload matrix A
     - stream B, reuse A, stream C
     - optionally transpose A (TAw=1, TAr=0)
     - optionally switch memory banks of A (TAw=1, TAr=1)
     - compute C=AB
     - **Future: Tile matrix A to handle any problem size**
    */
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL
    #pragma HLS INTERFACE ap_fifo port=A
    #pragma HLS INTERFACE ap_fifo port=B
    #pragma HLS INTERFACE ap_fifo port=C
    #pragma HLS INTERFACE ap_fifo port=bscale

    #pragma HLS INTERFACE s_axilite port=arow bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=acol bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=brow bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=ccol bundle=CTRL
    static data_t Amem[R_DEPTH*NUM_PES][C_DEPTH*VECTOR_WIDTH];
    fifo_t Bmem[VECTOR_WIDTH];
    fifoB_t Cmem;

//#pragma HLS array_partition variable=Amem cyclic factor=8 dim=1
//#pragma HLS array_partition variable=Amem cyclic factor=4 dim=2
DO_PRAGMA (HLS array_partition variable=Amem cyclic factor=NUM_PES dim=1)
DO_PRAGMA (HLS array_partition variable=Amem cyclic factor=VECTOR_WIDTH dim=2)


  int sig = ctrl;
  if (sig == 1){
	  readA(A, Amem, arow, brow, TAw, TAr);
  }

  for (int i=0; i<batch; i++){

#pragma HLS DATAFLOW

	  data_t *Bf = B + i*brow*ccol;
	  float *Cf = C + i*arow*ccol;

	  float as = ascale; //define A_scale and B_scale factors
	  float bs = bscale[i];
//#pragma HLS resource variable=cs core=FMul_meddsp
	  float cs = as*bs;
	  //printf("C scale factor = %.6f\n", cs);

	  readB(Bf, Bmem, brow, ccol);
	  compute(Amem, Bmem, Cmem, arow, brow, ccol, TAr, cs);
	  writeC(Cmem, Cf, arow, ccol);

  }

}
