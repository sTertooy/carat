#include "typedef.h"
#include "gmp.h"
/* #include "gmp-impl.h" */
/**************************************************************************\
@---------------------------------------------------------------------------
@---------------------------------------------------------------------------
@ FILE: MP_conv_mat.c
@---------------------------------------------------------------------------
@---------------------------------------------------------------------------
@
\**************************************************************************/



/**************************************************************************\
@---------------------------------------------------------------------------
@ MP_INT **matrix_to_MP_mat(M)
@ matrix_TYP *M;
@
@  allocated an 2-dimensional array of MP_INT of the size
@  Mat->rows x Mat->cols,
@  converts the entries of Mat->array.SZ to MP_INt and writes 
@  them into the array.
@---------------------------------------------------------------------------
@
\**************************************************************************/
MP_INT **matrix_to_MP_mat(M)
matrix_TYP *M;
{
  int i,j,m,n;
  MP_INT **erg;

  m = M->cols;
  n = M->rows;
  
  if((erg = (MP_INT **) malloc(n *sizeof(MP_INT *))) == NULL)
  {
    printf("malloc of 'erg' in mat_to_MP_mat failed\n");
    exit(2);
  }
  for(i=0;i<n;i++)
  {
    if((erg[i] = (MP_INT *) malloc(m *sizeof(MP_INT))) == NULL)
    {
      printf("malloc of 'erg[%d]' in mat_to_MP_mat failed\n", i);
      exit(2);
    }
  }
  for(i=0;i<n;i++)
    for(j=0;j<m;j++)
     mpz_init_set_si(&erg[i][j], M->array.SZ[i][j]);
  return(erg);
}


/**************************************************************************\
@---------------------------------------------------------------------------
@ matrix_TYP *MP_mat_to_matrix(M, rows, cols)
@ MP_INT **M;
@ int rows, cols;
@
@ converts the 2-dimensional array M of MP_INT to a matrix_TYP.
@ if the entries in M are to big, the function exits with an error message.
@---------------------------------------------------------------------------
@
\**************************************************************************/
matrix_TYP *MP_mat_to_matrix(M, rows, cols)
MP_INT **M;
int rows, cols;
{
  int i,j;
  matrix_TYP *erg;

  extern matrix_TYP *init_mat();

  erg = init_mat(rows, cols, "");

  for(i=0;i<rows;i++){
   for(j=0;j<cols;j++)
   {
     if(abs(M[i][j]._mp_size) > 1)
     {
       printf("Error: Integer overflow in 'MP_mat_to_matrix'\n");
       exit(2);
     }
     erg->array.SZ[i][j] = mpz_get_si(&M[i][j]);
   }
  }

  return(erg);
}



/**************************************************************************\
@---------------------------------------------------------------------------
@ void write_MP_mat_to_matrix(Mat, mp)
@ matrix_TYP *Mat;
@ MP_INT **mp;
@
@ converts the entries of 'mp' and write them to mat->array.SZ
@ If the entries are to big, the programm exits
@---------------------------------------------------------------------------
@
\**************************************************************************/
void write_MP_mat_to_matrix(Mat, mp)
matrix_TYP *Mat;
MP_INT **mp;
{
  int i,j;
  matrix_TYP *erg;

  extern matrix_TYP *init_mat();

  for(i=0;i<Mat->rows;i++)
   for(j=0;j<Mat->cols;j++)
   {
     if(abs(mp[i][j]._mp_size) > 1)
     {
       printf("Error: Integer overflow in 'write_MP_mat_to_matrix'\n");
       exit(3);
     }
     Mat->array.SZ[i][j] = mpz_get_si(&mp[i][j]);
   }
}


/**************************************************************************\
@---------------------------------------------------------------------------
@ MP_INT **init_MP_mat(rows, cols)
@ int rows, cols;
@
@ intitializes a 2-dimensional array of MP_INT of size rows x cols
@ and sets all entries 0.
@---------------------------------------------------------------------------
@
\**************************************************************************/
MP_INT **init_MP_mat(rows, cols)
int rows, cols;
{
   MP_INT **E;
   int i,j;

   if( (E = (MP_INT **)malloc(rows *sizeof(MP_INT *))) == NULL)
   {
     printf("malloc of 'E' in 'init_MP_mat' failed\n");
     exit(2);
   }
   for(i=0;i<rows;i++)
   {
     if( (E[i] = (MP_INT *)malloc(cols *sizeof(MP_INT))) == NULL)
     {
       printf("malloc of 'E[%d]' in 'init_MP_mat' failed\n", i);
       exit(2);
     }
     for(j=0;j<cols;j++)
        mpz_init(&E[i][j]);
   }
   return(E);
}



/**************************************************************************\
@---------------------------------------------------------------------------
@ void free_MP_mat(M, rows, cols)
@ MP_INT **M;
@ int rows, cols;
@
@ Clears the entries in the 2-dimensional array 'M' and frees
@ M[i] for 0 <= i < rows
@---------------------------------------------------------------------------
@
\**************************************************************************/
void free_MP_mat(M, rows, cols)
MP_INT **M;
int rows, cols;
{
 int i,j;
 for(i=0;i<rows;i++)
 {
    for(j=0;j<cols;j++)
      mpz_clear(&M[i][j]);
    free(M[i]);
 }
}
