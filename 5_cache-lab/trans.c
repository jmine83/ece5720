// Names: Joel Meine & Paul Roylance
// IDs: A01764207 & A01875212

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans_32_32(int M, int N, int A[N][M], int B[M][N]);
void trans_64_64(int M, int N, int A[N][M], int B[M][N]);
void trans_61_67(int M, int N, int A[N][M], int B[M][N]);
void trans(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *will be graded on for Part B of the assignment. Do not change
 *the description string "Transpose submission", as the driver
 *searches for that string to identify the transpose function to
 *be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	// Cases for 32 X 32 matrices
	if (M == 32){
		trans_32_32(M,N,A,B);
	}
	
	// Cases for 64 X 64 matrices
	else if (M == 64){
		trans_64_64(M,N,A,B);
	}
	
	// Cases for 61 X 67 matrices
	else if (M == 61){
		trans_61_67(M,N,A,B);
	}
	
	// All other cases do a  simple row-wise scan transpose
	else{
		trans(M,N,A,B);
	}
}

/** 
 * trans_32_32 - Transpose function case when matrix is 32 X 32
 */ 
char trans_32_32_desc[] = "Matrix 32 X 32 transpose";
void trans_32_32(int M, int N, int A[N][M], int B[M][N])
{
	/*
	 * block is best in 8 X 8 size
	 * one set of nested for loops to cycle through blocks
	 * a nested set of nested for loops to do block transpose
	 */
	
	int row, col, t; // iterators
	int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; // temporary values
	
	for (row = 0; row < M; row += 8){
		for (col = 0; col < N; col += 8){
			for (t = row; (t < row + 8) && (t < M); ++t) {
				// loop unrolling the column
				tmp0 = A[col][t];
				tmp1 = A[col + 1][t];
				tmp2 = A[col + 2][t];
				tmp3 = A[col + 3][t];
				tmp4 = A[col + 4][t];
				tmp5 = A[col + 5][t];
				tmp6 = A[col + 6][t];
				tmp7 = A[col + 7][t];
				
				B[t][col] = tmp0;
				B[t][col + 1] = tmp1;
				B[t][col + 2] = tmp2;
				B[t][col + 3] = tmp3;
				B[t][col + 4] = tmp4;
				B[t][col + 5] = tmp5;
				B[t][col + 6] = tmp6;
				B[t][col + 7] = tmp7;
			}
		}
	}
}

/** 
 * trans_64_64 - Transpose function case when matrix is 64 X 64
 */
char trans_64_64_desc[] = "Matrix 64 X 64 transpose";
void trans_64_64(int M, int N, int A[N][M], int B[M][N])
{
	/*
	 * block in 8 X 8 size
	 * submatrices A, B, C, D in 4 X 4 size
	 * method of sorting out block as follows
	 * 	 -----		 -----		 -----		 -----
	 * 	|A |B |		|AT|BT|		|AT|  |		|AT|CT|
	 * 	|-----|	-->	|-----|	-->	|-----|	-->	|-----|
	 * 	|C |D |		|  |  |		|BT|  |		|BT|DT|
	 * 	 -----		 -----		 -----		 -----
	 */
	
	
	int row, col, t; // iterators
	int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; // temporary values
	
	for (row = 0; row < M; row += 8){
		for (col = 0; col < N; col += 8){
			
			// transpose submatrices A and B
			// loop unrolling value setting to optimize matrix
			
			for (t = 0; t < 4; ++t){
				tmp0 = A[row + t][col];
				tmp1 = A[row + t][col + 1];
				tmp2 = A[row + t][col + 2];
				tmp3 = A[row + t][col + 3];
				tmp4 = A[row + t][col + 5];
				tmp5 = A[row + t][col + 6];
				tmp6 = A[row + t][col + 7];
				tmp7 = A[row + t][col + 4];
				
				B[col][row + t] = tmp0;
				B[col + 1][row + t] = tmp1;
				B[col + 2][row + t] = tmp2;
				B[col + 3][row + t] = tmp3;
				B[col][row + t + 4] = tmp4;
				B[col + 1][row + t + 4] = tmp5;
				B[col + 2][row + t + 4] = tmp6;
				B[col + 3][row + t + 4] = tmp7;
			}
			
			
			// move submatrix B to place
			// loop unrolling value setting to optimize matrix
			
			tmp0 = A[row + 4][col + 3];
			tmp1 = A[row + 5][col + 3];
			tmp2 = A[row + 6][col + 3];
			tmp3 = A[row + 7][col + 3];
			tmp4 = A[row + 4][col + 4];
			tmp5 = A[row + 5][col + 4];
			tmp6 = A[row + 6][col + 4];
			tmp7 = A[row + 7][col + 4];
			
			B[col + 4][row + 0] = B[col + 3][row + 4];
			B[col + 4][row + 1] = B[col + 3][row + 5];
			B[col + 4][row + 2] = B[col + 3][row + 6];
			B[col + 4][row + 3] = B[col + 3][row + 7];
			B[col + 3][row + 4] = tmp0;
			B[col + 3][row + 5] = tmp1;
			B[col + 3][row + 6] = tmp2;
			B[col + 3][row + 7] = tmp3;
			B[col + 4][row + 4] = tmp4;
			B[col + 4][row + 5] = tmp5;
			B[col + 4][row + 6] = tmp6;
			B[col + 4][row + 7] = tmp7;
			
			
			// do transposes of submatrices C and D
			// loop unrolling value setting to optimize matrix
			
			for (int t = 0; t < 3; ++t){
				tmp0 = A[row + 4][col + t];
				tmp1 = A[row + 5][col + t];
				tmp2 = A[row + 6][col + t];
				tmp3 = A[row + 7][col + t];
				tmp4 = A[row + 4][col + t + 5];
				tmp5 = A[row + 5][col + t + 5];
				tmp6 = A[row + 6][col + t + 5];
				tmp7 = A[row + 7][col + t + 5];
				
				B[col + t + 5][row + 0] = B[col + t][row + 4];
				B[col + t + 5][row + 1] = B[col + t][row + 5];
				B[col + t + 5][row + 2] = B[col + t][row + 6];
				B[col + t + 5][row + 3] = B[col + t][row + 7];
				B[col + t][row + 4] = tmp0;
				B[col + t][row + 5] = tmp1;
				B[col + t][row + 6] = tmp2;
				B[col + t][row + 7] = tmp3;
				B[col + t + 5][row + 4] = tmp4;
				B[col + t + 5][row + 5] = tmp5;
				B[col + t + 5][row + 6] = tmp6;
				B[col + t + 5][row + 7] = tmp7;
			}
		}
	}
}

/** 
 * trans_61_67 - Transpose function case when matrix is 61 X 67
 */
char trans_61_67_desc[] = "Matrix 61 X 67 transpose";
void trans_61_67(int M, int N, int A[N][M], int B[M][N])
{
	/*
	 * block is best in 16 X 16 size
	 * one set of nested for loops to cycle through blocks
	 * a nested set of nested for loops to do block transpose
	 * the odd dimensions require conditioning to keep within bounds
	 */
	
	int row, col; // location of blocks
	int i, j; // location within blocks
	int size = 16; // Size of a block
	
	for (row = 0; row < M; row += size){
		for (col = 0; col < N; col += size){
			for (i = row; (i < row + size) && (i < M); ++i){
				for (j = col; (j < col + size) && (j < N); ++j)
					B[i][j] = A[j][i];
			}
		}
	}
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
	int i, j, tmp;
	
	for (i = 0; i < N; ++i) {
		for (j = 0; j < M; ++j) {
			tmp = A[i][j];
			B[j][i] = tmp;
		}
	}
}

/*
 * registerFunctions - This function registers your transpose
 *functions with the driver.  At runtime, the driver will
 *evaluate each of the registered functions and summarize their
 *performance. This is a handy way to experiment with different
 *transpose strategies.
 */
void registerFunctions()
{
	/* Register your solution function */
	registerTransFunction(transpose_submit, transpose_submit_desc); 
	
	/* Register any additional transpose functions */
	//registerTransFunction(trans_32_32, trans_32_32_desc);
	//registerTransFunction(trans_64_64, trans_64_64_desc);
	//registerTransFunction(trans_61_67, trans_61_67_desc);
	//registerTransFunction(trans, trans_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *A. You can check the correctness of your transpose by calling
 *it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
	int i, j;
	
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; ++j) {
			if (A[i][j] != B[j][i]) {
				return 0;
			}
		}
	}
	return 1;
}

