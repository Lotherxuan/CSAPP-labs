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

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  if (M == 32 && N == 32) {
    for (int out_i = 0; out_i < 4; out_i++) {
      for (int out_j = 0; out_j < 4; out_j++) {
        int out_row = out_i * 8;
        int out_column = out_j * 8;
        for (int row = out_row; row < out_row + 8; row++) {
          int tmp = -1;
          for (int column = out_column; column < out_column + 8; column++) {
            if (row != column) {
              B[column][row] = A[row][column];
            } else {
              tmp = row;
            }
          }
          if (tmp != -1) {
            int t = A[tmp][tmp];
            B[tmp][tmp] = t;
          }
        }
      }
    }
  } else if (M == 64 && N == 64) {
    int a0, a1, a2, a3, a4, a5, a6, a7, temp;
    for (int out_i = 0; out_i < 8; out_i++) {
      for (int out_j = 0; out_j < 8; out_j++) {
        int out_row = out_i * 8;
        int out_column = out_j * 8;
        for (int i = 0; i < 4; i++) {
          a0 = A[out_row + i][out_column + 0];
          a1 = A[out_row + i][out_column + 1];
          a2 = A[out_row + i][out_column + 2];
          a3 = A[out_row + i][out_column + 3];
          a4 = A[out_row + i][out_column + 4];
          a5 = A[out_row + i][out_column + 5];
          a6 = A[out_row + i][out_column + 6];
          a7 = A[out_row + i][out_column + 7];

          B[out_column + 0][out_row + i] = a0;
          B[out_column + 1][out_row + i] = a1;
          B[out_column + 2][out_row + i] = a2;
          B[out_column + 3][out_row + i] = a3;

          B[out_column + 0][out_row + i + 4] = a4;
          B[out_column + 1][out_row + i + 4] = a5;
          B[out_column + 2][out_row + i + 4] = a6;
          B[out_column + 3][out_row + i + 4] = a7;
        }
        for (int i = 0; i < 4; i++) {
          a0 = A[out_row + 4][out_column + i];
          a1 = A[out_row + 5][out_column + i];
          a2 = A[out_row + 6][out_column + i];
          a3 = A[out_row + 7][out_column + i];

          a4 = A[out_row + 4][out_column + i + 4];
          a5 = A[out_row + 5][out_column + i + 4];
          a6 = A[out_row + 6][out_column + i + 4];
          a7 = A[out_row + 7][out_column + i + 4];

          temp = B[out_column + i][out_row + 4];
          B[out_column + i][out_row + 4] = a0;
          a0 = temp;
          temp = B[out_column + i][out_row + 5];
          B[out_column + i][out_row + 5] = a1;
          a1 = temp;
          temp = B[out_column + i][out_row + 6];
          B[out_column + i][out_row + 6] = a2;
          a2 = temp;
          temp = B[out_column + i][out_row + 7];
          B[out_column + i][out_row + 7] = a3;
          a3 = temp;

          B[out_column + 4 + i][out_row + 0] = a0;
          B[out_column + 4 + i][out_row + 1] = a1;
          B[out_column + 4 + i][out_row + 2] = a2;
          B[out_column + 4 + i][out_row + 3] = a3;
          B[out_column + 4 + i][out_row + 4] = a4;
          B[out_column + 4 + i][out_row + 5] = a5;
          B[out_column + 4 + i][out_row + 6] = a6;
          B[out_column + 4 + i][out_row + 7] = a7;
        }
      }
    }
  } else if (N == 67 && M == 61) {
    for (int out_i = 0; out_i < 5; out_i++) {
      for (int out_j = 0; out_j < 4; out_j++) {
        int out_row = out_i * 16;
        int out_column = out_j * 16;
        for (int in_row = 0; in_row < 16; in_row++) {
          for (int in_column = 0; in_column < 16; in_column++) {
            int row = out_row + in_row;
            int column = out_column + in_column;
            if (row < 67 && column < 61) {
              B[column][row] = A[row][column];
            }
          }
        }
      }
    }
  }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
  /* Register your solution function */
  registerTransFunction(transpose_submit, transpose_submit_desc);

  /* Register any additional transpose functions */
  registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
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
