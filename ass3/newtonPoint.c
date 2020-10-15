#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define TYPE double
#define DEG_T int
#define ITER_T int
#define ATT_T int

#define Tol_MIN 0.001
#define Tol_MAX 10000000000.
#define VALUE 0
#define DEFAULT_VALUE -1 // 0 ?

int
PreCalcRoots(
	     int d,
	     TYPE* arr_r,
	     TYPE* arr_i
	     )
{
  *arr_r = 1;
  *arr_i = 0;
  
  for (size_t ix = 1; ix < d; ++ix)
    {
      arr_r[ix] = cos(2*3.14159*ix / d);
      arr_i[ix] = sin(2*3.14159*ix / d);
    }
  return 0;
}

int
NewtonPoint(
	    TYPE a_r,
	    TYPE a_i,
	    DEG_T d,
	    TYPE* arr_r,
	    TYPE* arr_i,
	    ITER_T* i,
	    ATT_T* a)
{
  // attractor is the index of the root we found
  // convergence is the iterations to the root we found
  ITER_T conv;
  ATT_T attr;

  for ( conv = 0, attr = DEFAULT_VALUE; conv <= 50; ++conv ) {
    TYPE sqNorm = a_r*a_r + a_i*a_i;
    
    if ( sqNorm > Tol_MAX * Tol_MAX ) { // Check upper bound
      attr = VALUE;
      break;
    } else if ( sqNorm < Tol_MIN * Tol_MIN ) { // Check lower bound
      attr = VALUE;
      break;
    }
    for ( size_t ir = 0; ir < d; ++ir ) { // all the roots
      TYPE b_r = arr_r[ir];
      TYPE b_i = arr_i[ir];
      b_r -= a_r;
      b_i -= a_i;
      
      if ( b_r*b_r + b_i*b_i < Tol_MIN * Tol_MIN ) {
	attr = ir + 1; // index of the root
	break;
      }
    }
    if ( attr != DEFAULT_VALUE ) // check if the previous root broke out;
      break;
    
    // computation
    // (d-1)/d *x + 1/(d*x^(d-1))
    double sqr;
    double a, b;
    switch ( d ) {
  case 1:
    a_r = 1.;
    a_i = 0.;
    break;
  case 2:
    a = a_r;
    b = a_i;
    
    sqr = a*a + b*b;
    a_r = 0.5 * a_r + 0.5 * a / sqr;
    a_i = 0.5 * a_i - 0.5 * b / sqr;
    break;
  case 3:
    a = a_r*a_r -
      a_i*a_i;
    b = 2 * a_r * a_i;
    
    sqr = a*a + b*b;
    a_r = (2./3.) * a_r + (1./3.) * a / sqr;
    a_i = (2./3.) * a_i - (1./3.) * b / sqr;
    break;
  case 4:
    a = a_r*a_r*a_r -
      3* a_r*a_i*a_i;
    b = -a_i*a_i*a_i +
      3*a_r*a_r*a_i;
    
    sqr = a*a + b*b;
    a_r = 0.75 * a_r + 0.25 * a / sqr;
    a_i = 0.75 * a_i - 0.25 * b / sqr;
    break;
  case 5:
    a = a_r*a_r*a_r*a_r -
      6*a_r*a_r*a_i*a_i +
      a_i*a_i*a_i*a_i;
    b = 4*a_r*a_r*a_r*a_i -
      4*a_r*a_i*a_i*a_i;
      
    sqr = a*a + b*b;
    a_r = 0.8 * a_r + 0.2 * a / sqr;
    a_i = 0.8 * a_i - 0.2 * b / sqr;
    break;
  case 6:
    a =    a_r*a_r*a_r*a_r*a_r -
        10*a_r*a_r*a_r*a_i*a_i  +
        5* a_r*a_i*a_i*a_i*a_i;
    b = a_i*a_i*a_i*a_i*a_i -
        10*a_r*a_r*a_i*a_i*a_i +
        5*a_i*a_r*a_r*a_r*a_r;
    
    sqr = a*a + b*b;
    a_r = (5./6.) * a_r + a / (6. * sqr);
    a_i = (5./6.) * a_i - b / (6. * sqr);
    break;
  case 7:
    a = a_r*a_r*a_r*a_r*a_r*a_r -
      15*a_r*a_r*a_r*a_r*a_i*a_i +
      15*a_i*a_i*a_i*a_i*a_r*a_r -
      a_i*a_i*a_i*a_i*a_i*a_i;
    b = -20*a_r*a_r*a_r*a_i*a_i*a_i +
      6*a_r*a_r*a_r*a_r*a_r*a_i +
      6*a_i*a_i*a_i*a_i*a_i*a_r;
    
    sqr = a*a + b*b;
    a_r = (6./7.) * a_r + a / (7. * sqr);
    a_i = (6./7.) * a_i - b / (7. * sqr);
    break;
  case 8:
    a = a_r*a_r*a_r*a_r*a_r*a_r*a_r -
      21*a_r*a_r*a_r*a_r*a_r*a_i*a_i +
      35*a_r*a_r*a_r*a_i*a_i*a_i*a_i -
      7*a_r*a_i*a_i*a_i*a_i*a_i*a_i;
    b = -a_i*a_i*a_i*a_i*a_i*a_i*a_i +
      21*a_r*a_r*a_i*a_i*a_i*a_i*a_i -
      35*a_r*a_r*a_r*a_r*a_i*a_i*a_i +
      7*a_r*a_r*a_r*a_r*a_r*a_r*a_i;

    sqr = a*a + b*b;
    a_r = (7./8.) * a_r + a / (8. * sqr);
    a_i = (7./8.) * a_i - b / (8. * sqr);
    break;
  case 9:
    a = a_r*a_r*a_r*a_r*a_r*a_r*a_r*a_r -
      28*a_r*a_r*a_r*a_r*a_r*a_r*a_i*a_i +
      70*a_r*a_r*a_r*a_r*a_i*a_i*a_i*a_i -
      28*a_r*a_r*a_i*a_i*a_i*a_i*a_i*a_i +
      a_i*a_i*a_i*a_i*a_i*a_i*a_i*a_i;
    b = 56*a_r*a_r*a_r*a_i*a_i*a_i*a_i*a_i -
      56*a_r*a_r*a_r*a_r*a_r*a_i*a_i*a_i +
      8*a_r*a_r*a_r*a_r*a_r*a_r*a_r*a_i -
      8*a_r*a_i*a_i*a_i*a_i*a_i*a_i*a_i;

    sqr = a*a + b*b;
    a_r = (8./9.) * a_r + a / (9. * sqr);
    a_i = (8./9.) * a_i - b / (9. * sqr);
    break;
  case 10:
    a = a_r*a_r*a_r*a_r*a_r*a_r*a_r*a_r*a_r -
      36*a_r*a_r*a_r*a_r*a_r*a_r*a_r*a_i*a_i +
      126*a_r*a_r*a_r*a_r*a_r*a_i*a_i*a_i*a_i -
      84*a_r*a_r*a_r*a_i*a_i*a_i*a_i*a_i*a_i +
      9*a_r*a_i*a_i*a_i*a_i*a_i*a_i*a_i*a_i;
    b = a_i*a_i*a_i*a_i*a_i*a_i*a_i*a_i*a_i +
      126*a_r*a_r*a_r*a_r*a_i*a_i*a_i*a_i*a_i -
      36*a_r*a_r*a_i*a_i*a_i*a_i*a_i*a_i*a_i -
      84*a_r*a_r*a_r*a_r*a_r*a_r*a_i*a_i*a_i +
      9*a_r*a_r*a_r*a_r*a_r*a_r*a_r*a_r*a_i;

    sqr = a*a + b*b;
    a_r = 0.9 * a_r + 0.1 * a / sqr;
    a_i = 0.9 * a_i - 0.1 * b / sqr;
    break;
  default:
    fprintf(stderr, "unexpected degree\n");
    exit(1);
  }

    //printf("%f + %fi\n", a_r, a_i);
  
  }

  // set the output
  *i = conv;
  *a = attr;
  
  return 0;
}

/*
int
main(
     int argc,
     char* argv[])
{

  size_t d = 7;

  TYPE arr_r[d];
  TYPE arr_i[d];
  PreCalcRoots(d, arr_r, arr_i);


  TYPE a_r = -3.;
  TYPE a_i = 1.;

  for (a_r = -2.; a_r < 2.; a_r += 0.1)
    {
  ITER_T i;
  ATT_T a;

NewtonPoint(a_r, a_i, d,
	    arr_r, arr_i, &i, &a);

 printf("num of iterations: %d\nwhich attractor: %d\n", i, a);
    }

  // Test Precalcing
  /*
  for (size_t ix = 1; ix < 11; ++ix)
    {
      TYPE arr_r[ix];
      TYPE arr_i[ix];
      PreCalcRoots(ix, arr_r, arr_i);

      for (size_t i = 0; i < ix; ++i)
	printf("%f+%fi\n", arr_r[i], arr_i[i]); 

      printf("\n");
    }
  /
  return 0;
}
*/
