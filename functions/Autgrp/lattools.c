/*****	This file contains routines for lattice reduction and the
	calculation of shortest vectors	*****/
#include "typedef.h"

/*********************************************************************\
| changes v according to the transformation matrix T, i.e. v := T*v
\*********************************************************************/
static void change(v, nv, T, n)
int	**v, nv, **T, n;
{
	int	i, j, k, **w, *wi, *Ti, *vi, *vj, fac;

	if ((w = (int**)malloc(nv * sizeof(int*))) == 0)
		exit (2);
	for (i = 0; i < nv; ++i)
	{
		if ((w[i] = (int*)malloc(n * sizeof(int))) == 0)
			exit (2);
		wi = w[i];
		for (j = 0; j < n; ++j)
			wi[j] = 0;
	}
	for (i = 0; i < nv; ++i)
	{
		wi = w[i];
		Ti = T[i];
		for (j = 0; j < nv; ++j)
		{
			if ((fac = Ti[j]) != 0)
			{
				vj = v[j];
				for (k = 0; k < n; ++k)
					wi[k] += fac * vj[k];
			}
		}
	}
	for (i = 0; i < nv; ++i)
	{
		vi = v[i];
		wi = w[i];
		for (j = 0; j < n; ++j)
			vi[j] = wi[j];
		free(wi);
	}
	free(w);
}

/**********************************************************************\
|	LLL-reduction of the positive semidefinite 
|	Gram-matrix F with transformation matrix T
\**********************************************************************/
static int lll(F, T, dim)
int	**F, **T, dim;
{
	int	**gram, i, j, m, l, rank, *ptmp, tmp;
	double	**mu, *B;

/* the weights */
	if ((B = (double*)malloc(dim * sizeof(double))) == 0)
		exit (2);
/* the model matrix */
	if ((mu = (double**)malloc(dim * sizeof(double*))) == 0)
		exit (2);
/* F is copied on gram and remains unchanged */
	if ((gram = (int**)malloc(dim * sizeof(int*))) == 0)
		exit (2);
	for (i = 0; i < dim; ++i)
	{
		if ((mu[i] = (double*)malloc(dim * sizeof(double))) == 0)
			exit (2);
		if ((gram[i] = (int*)malloc(dim * sizeof(int))) == 0)
			exit (2);
		for (j = 0; j < dim; ++j)
		{
			T[i][j] = 0;
			mu[i][j] = 0.0;
			gram[i][j] = F[i][j];
		}
		T[i][i] = 1;
		mu[i][i] = 1.0;
	}
/* the first basis-vector should not have norm 0 */
	for (i = 0; i < dim  &&  gram[i][i] == 0; ++i);
	if (i == dim)
		return(0);
	else if (i > 0)
	{
		ptmp = gram[i];
		gram[i] = gram[0];
		gram[0] = ptmp;
		for (j = 0; j < dim; ++j)
		{
			tmp = gram[j][i];
			gram[j][i] = gram[j][0];
			gram[j][0] = tmp;
		}
		T[0][0] = 0;
		T[0][i] = 1;
		T[i][i] = 0;
		T[i][0] = 1;
	}
	rank = dim;
	initialize(0, mu, B, gram);
	if (dim > 1)
		initialize(1, mu, B, gram);
/* recursively go through the matrix */
	m = 1;
	l = 0;
	while (m < rank)
		m = red(&m, &l, gram, T, mu, B, dim, &rank);
	free(B);
	for (i = 0; i < dim; ++i)
	{
		free(mu[i]);
		free(gram[i]);
	}
	free(mu);
	free(gram);
	return(rank);
}

/**********************************************************************\
|	initialization of the model
\**********************************************************************/
static void initialize(i, mu, B, gram)
int	i, **gram;
double	**mu, *B;
{
	int	j, k;
	double	muh, *mui, *muj;

	B[i] = (double)gram[i][i];
	mui = mu[i];
	for (j = 0; j < i; ++j)
	{
		muh = (double)gram[i][j];
		muj = mu[j];
		for (k = 0; k < j; ++k)
			muh -= B[k] * mui[k] * muj[k];
		muh /= B[j];
		B[i] -= B[j] * muh * muh;
		mui[j] = muh;
	}
}

/**********************************************************************\
|	pair-reduction with vectors m and l,
|	changes Gram-matrix, transformation matrix and model appropriately
\**********************************************************************/
static int red(m, l, gram, T, mu, B, dim, rank)
int	*m, *l, **gram, **T, dim, *rank;
double	**mu, *B;
{
	double	r;
	int	j, ir, *ptmp, jump, *Tm, *Tl, *gramm, *graml;

	jump = 0;
	r = mu[*m][*l];
	if (r > 0.5  ||  r < -0.5)
	{
		ir = iround(r);
		Tm = T[*m];
		Tl = T[*l];
		gramm = gram[*m];
		graml = gram[*l];
		for (j = 0; j < dim; ++j)
		{
			Tm[j] -= ir * Tl[j];
			gramm[j] -= ir * graml[j];	
		}
		for (j = 0; j < dim; ++j)
			gram[j][*m] -= ir * gram[j][*l];	
		initialize(*m, mu, B, gram); 
	}
	gramm = gram[*m];
	for (j = 0; j < *rank  &&  gramm[j] == 0; ++j);
	if (j == *rank)
	{
		ptmp = T[*m];
		T[*m] = T[*rank-1];
		T[*rank-1] = ptmp;
		for (j = 0; j < dim; ++j)
		{
			gram[*m][j] = gram[*rank-1][j];
			gram[*rank-1][j] = 0;
		}
		for (j = 0; j < dim; ++j)
		{
			gram[j][*m] = gram[j][*rank-1];
			gram[j][*rank-1] = 0;
		}
		*rank -= 1;
		if (*m < *rank)
		{
			initialize(*m, mu, B, gram); 
			*l = *m-1;
			jump = 1;
		}
		else
			jump = 1;
	}
	if (*l == *m-1  &&  jump == 0)
		check(B, m, l, gram, T, mu, dim, rank);
	else if (jump == 0)
		decrease(m, l, gram, mu, B, rank);
	return(*m);
}

/**********************************************************************\
| check the LLL-condition
\**********************************************************************/
static void check(B, m, l, gram, T, mu, dim, rank)
int	*m, *l, **gram, **T, dim, *rank;
double	*B, **mu;
{
	if (B[*m] < (LLL_CONST - mu[*m][*m-1]*mu[*m][*m-1]) * B[*m-1])
		interchange(m, l, gram, T, mu, B, dim);
	else
	{
		*l = *m-1;
		decrease(m, l, gram, mu, B, rank);	
	}
}

/**********************************************************************\
|	go back in the recursion and change the model
\**********************************************************************/
static void decrease(m, l, gram, mu, B, rank)
int	*m, *l, **gram, *rank;
double	**mu, *B;
{
	*l -= 1;
	if (*l < 0)
	{
		*m += 1;
		if (*m < *rank)
		{
			*l = *m-1;
			initialize(*m, mu, B, gram);
		}
	}
}

/**********************************************************************\
|	interchange the vectors nr. m and m-1
|	and change the model appropriately
\**********************************************************************/
static void interchange(m, l, gram, T, mu, B, dim)
int	*m, *l, **gram, **T, dim;
double	**mu, *B;
{
	int	i, tmp, *ptmp;
	/* these variables weren't used at all, delete 02/12/97
        double	Mu, b; */

	ptmp = T[*m];
	T[*m] = T[*m-1];
	T[*m-1] = ptmp;
	ptmp = gram[*m];
	gram[*m] = gram[*m-1];
	gram[*m-1] = ptmp;
	for (i = 0; i < dim; ++i)
	{
		tmp = gram[i][*m];
		gram[i][*m] = gram[i][*m-1];
		gram[i][*m-1] = tmp;
	}
	initialize(*m-1, mu, B, gram);
	if (*m > 1)
	{
		*m -= 1;
		*l = *m-1;
	}
	else
	{
		initialize(*m, mu, B, gram);
		*l = *m-1;
	}
}

/**********************************************************************\
|	rounds x to an integer	
\**********************************************************************/
static int iround(x)
double	x;
{
	if (x >= 0)
		return((int)(2.0*x + 1.0) / 2);
	else
		return(-((int)(-2.0*x + 1.0) / 2));
}
