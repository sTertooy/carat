#include "typedef.h"
#include "gmp.h"
/* #include "gmp-impl.h" */
#include "longtools.h"
#include "matrix.h"
/**************************************************************************\
@---------------------------------------------------------------------------
@---------------------------------------------------------------------------
@ FILE: long_qbase.c
@---------------------------------------------------------------------------
@---------------------------------------------------------------------------
@
\**************************************************************************/


/**************************************************************************\
@---------------------------------------------------------------------------
@ matrix_TYP *long_qbase(Mat)
@ matrix_TYP *Mat;
@
@ long_qbase calculetes a matrix M1 with rows from Mat, such that
@ the rows of M1 form a qbase of the vectorspace generated by
@ the rows of Mat.
@---------------------------------------------------------------------------
@
\**************************************************************************/
matrix_TYP *long_qbase(Mat)
matrix_TYP *Mat;
{
  int i,j, rang, neurang, n;
  MP_INT **M;
  matrix_TYP *erg;
 
  n = Mat->cols;
  if(Mat->rows < Mat->cols)
   n = Mat->rows;
  M = init_MP_mat(n, Mat->cols);
  erg = init_mat(n, Mat->cols, "");
  rang = 0;
  for(i=0;i<Mat->rows && rang < n;i++)
  {
     for(j=0;j<Mat->cols;j++)
       mpz_set_si(&M[rang][j], Mat->array.SZ[i][j]);
     neurang = MP_row_gauss(M, rang+1, Mat->cols);
     if(neurang != rang)
     {
       for(j=0;j<Mat->cols;j++)
         erg->array.SZ[rang][j] = Mat->array.SZ[i][j];
       rang++;
     }
  }
  if(rang != n)
    real_mat(erg, rang, Mat->cols);
  free_MP_mat(M,n, Mat->cols);
  free(M);
  return(erg);
}
