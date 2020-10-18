#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>


#define TYPE double
#define DEG_T int
#define ITER_T int
#define ATT_T int

#define Tol_MIN 0.001
#define TOL_MAX 10000000000.
#define VALUE 0
#define DEFAULT_VALUE -1 // 0 ?
#define CAP 50



typedef struct {
  int val;
  char pad[60]; // cacheline - sizeof(int)
} int_padded;

typedef struct {
  const float **re_matrix;
  const float **im_matrix;
  int degree;
  double*arr_r;
  double* arr_i;
  
  int **root;
  int **iterations;
  int ib;
  int istep;
  int sz;
  int tx;
  mtx_t *mtx;
  cnd_t *cnd;
  int_padded *status;
} thrd_info_t;

typedef struct {
  int degree;
  int **root;
  int **iterations;
  int sz;
  int nthrds;
  mtx_t *mtx;
  cnd_t *cnd;
  int_padded *status;
} thrd_info_check_t;


int
PreCalcRoots(
	     int d,
	     TYPE* arr_r,
	     TYPE* arr_i
	     );


int
NewtonPoint(
	    TYPE a_r,
	    TYPE a_i,
	    DEG_T d,
	    TYPE* arr_r,
	    TYPE* arr_i,
	    ITER_T* i,
	    ATT_T* a);


void WritePPM2(	 int size,
		 int* a_attr,
		 char* AllAttrColours,
		 FILE* fpa,
		 int* a_conv,
		 char* AllConvColours,
		 FILE *fpc,
		 int Colours[10][3]
		 );

int
main_thrd(
    void *args
    )
{
  const thrd_info_t *thrd_info = (thrd_info_t*) args;
  //const float **re_matrix = thrd_info->re_matrix;
  //const float **im_matrix=thrd_info->im_matrix;
  const int degree =thrd_info->degree;
  double* arr_r=thrd_info->arr_r;
  double*arr_i=thrd_info->arr_i;
  int **root = thrd_info->root;
  int **iterations=thrd_info->iterations;
  const int ib = thrd_info->ib;
  const int istep = thrd_info->istep;
  const int sz = thrd_info->sz;
  const int tx = thrd_info->tx;
  mtx_t *mtx = thrd_info->mtx;
  cnd_t *cnd = thrd_info->cnd;
  int_padded *status = thrd_info->status;


  float gap = 4.f/(sz-1);
  
  for ( int ix = ib; ix < sz; ix += istep ) {
    //const float *vix=v[ix];
    // We allocate the rows of the result before computing, and free them in another thread.
    int *loc_root = (int*) malloc(sz*sizeof(int));
    int *loc_iterations=(int*) malloc(sz*sizeof(int));

    int iter;
    int attr;

    float im = ix * gap - 2.f;

    for ( int jx = 0; jx < sz; ++jx ){
      NewtonPoint(jx * gap - 2.f, im, degree, arr_r, arr_i, &iter, &attr);
      loc_iterations[jx]=iter;
      loc_root[jx]=attr;
    }
    mtx_lock(mtx);
    root[ix] = loc_root;
    iterations[ix]=loc_iterations;
    status[tx].val = ix+istep;
    mtx_unlock(mtx);
    cnd_signal(cnd);
   

   
  }

  return 0;
}

int
main_thrd_check(
    void *args
    )
{
  const thrd_info_check_t *thrd_info = (thrd_info_check_t*) args;
  int degree=thrd_info->degree;
  int **root = thrd_info->root;
  int **iterations = thrd_info->iterations;
  const int sz = thrd_info->sz;
  const int nthrds = thrd_info->nthrds;
  mtx_t *mtx = thrd_info->mtx;
  cnd_t *cnd = thrd_info->cnd;
  int_padded *status = thrd_info->status;

  const float eps = 1e-1;
  
  char *rgb_attr =  malloc(2*3*sz*sizeof(char));
  char *rgb_iter =  malloc(3*3*sz*sizeof(char));

  char fpi_name[30];
  char fpc_name[30];

  
  snprintf(fpi_name,sizeof(fpi_name),"newton_convergence_x%d.ppm",degree);
  snprintf(fpc_name,sizeof(fpc_name),"newton_attractors_x%d.ppm",degree); 

 
  
  FILE *fpi = fopen(fpi_name, "wb");
  FILE *fpc = fopen(fpc_name, "wb");
  
  fprintf(fpc, "P3\n%d %d\n%d\n", sz, sz, 2);
  fprintf(fpi, "P3\n%d %d\n%d\n", sz, sz, 50);

   //making colour matrix to pick from in the write function
  int Colours[10][3] = {{2, 0, 0}, {2, 1, 0}, {2, 2, 0},
			{1, 2, 0}, {0, 2, 0}, {0, 2, 1},
			{0, 2, 2}, {0, 1, 2}, {0, 0, 2},
			{1, 0, 2}};
  
  // We do not increment ix in this loop, but in the inner one.
  for ( int ix = 0, ibnd; ix < sz; ) {
   
    // If no new lines are available, we wait.
    for ( mtx_lock(mtx); ; ) {
      // We extract the minimum of all status variables.
      ibnd = sz;
      for ( int tx = 0; tx < nthrds; ++tx ){
	//printf("Thread %i: %i\n",tx,status[tx].val);
	if ( ibnd > status[tx].val )
          ibnd = status[tx].val;
      }
      if ( ibnd <= ix )
        cnd_wait(cnd,mtx);
      else {
        mtx_unlock(mtx);
        break;
      }

    
    }
      

    // We do not initialize ix in this loop, but in the outer one.
    for ( ; ix < ibnd; ++ix ) {
     
      WritePPM2(sz, root[ix],rgb_attr, fpc,iterations[ix],rgb_iter, fpi,Colours);
      free(root[ix]);
      free(iterations[ix]);
    }

     
  }

  free(rgb_attr);
  free(rgb_iter);

  fclose(fpi);
  fclose(fpc);
  return 0;
}

int
main(int argc, char* argv[])
{
  
  //Parse command line args

  int nthrds;
  int sz;
  
  if(argv[1][1]=='t'){
    nthrds=atoi(argv[1]+2);
    sz=atoi(argv[2]+2);
  }else if(argv[2][1]=='t'){
    nthrds=atoi(argv[2]+2);
    sz=atoi(argv[1]+2);
  }else{
    printf("Error parsin cmd line args\n");
    return -1;
  }

 

  const int degree=atoi(argv[3]);

  //printf("%i,%i, %i\n",nthrds,sz, degree); 
  //Precalculcate roots
  double arr_r[degree];
  double arr_i[degree];

  PreCalcRoots(degree,arr_r,arr_i);

  

  //Initialize coordinate matrices
  //  float **re_matrix=(float**) malloc(sz*sizeof(float*));
  //float **im_matrix=(float**) malloc(sz*sizeof(float*));
  // float *re_entries=(float*) malloc(sz*sz*sizeof(float));
  // float *im_entries=(float*) malloc(sz*sz*sizeof(float));

  
  //Calculating gap between coordinates
  // float gap=4./(sz-1);


  /*
  //Filling in coordinates
  for ( int ix = 0, jx = 0; ix < sz; ++ix, jx += sz ){
    re_matrix[ix]=re_entries+jx;
    im_matrix[ix]=im_entries+jx;

  }
  for(int ix=0;ix<sz;++ix){
    for(int jx=0;jx<sz;++jx){
      re_matrix[ix][jx]=-2+ix*gap;
      im_matrix[jx][ix]=2-ix*gap;
    }
  }
  */
  
  //Initialize result matrices
  int **root=(int**) malloc(sz*sizeof(int*));
  int **iterations=(int**) malloc(sz*sizeof(int*));



  //Initializing threads
  thrd_t thrds[nthrds];
  thrd_info_t thrds_info[nthrds];

  thrd_t thrd_check;
  thrd_info_check_t thrd_info_check;
  
  mtx_t mtx;
  mtx_init(&mtx, mtx_plain);

  cnd_t cnd;
  cnd_init(&cnd);

  int_padded status[nthrds];

  for ( int tx = 0; tx < nthrds; ++tx ) {
    //  thrds_info[tx].re_matrix = (const float**) re_matrix;
    // thrds_info[tx].im_matrix= (const float**) im_matrix;
    thrds_info[tx].arr_r=arr_r;
    thrds_info[tx].arr_i=arr_i;
    thrds_info[tx].root=root;
    thrds_info[tx].iterations=iterations;
    thrds_info[tx].degree=degree;
    thrds_info[tx].ib = tx;
    thrds_info[tx].istep = nthrds;
    thrds_info[tx].sz = sz;
    thrds_info[tx].tx = tx;
    thrds_info[tx].mtx = &mtx;
    thrds_info[tx].cnd = &cnd;
    thrds_info[tx].status = status;
    status[tx].val = 0;

    int r = thrd_create(thrds+tx, main_thrd, (void*) (thrds_info+tx));
    if ( r != thrd_success ) {
      fprintf(stderr, "failed to create thread\n");
      exit(1);
    }
    thrd_detach(thrds[tx]);
  }

  {
    thrd_info_check.degree=degree;
    thrd_info_check.root=root;
    thrd_info_check.iterations=iterations;
    thrd_info_check.sz = sz;
    thrd_info_check.nthrds = nthrds;
    thrd_info_check.mtx = &mtx;


    thrd_info_check.cnd = &cnd;
    // It is important that we have initialize status in the previous for-loop,
    // since it will be consumed by the check threads.
    thrd_info_check.status = status;

    int r = thrd_create(&thrd_check, main_thrd_check, (void*) (&thrd_info_check));
    if ( r != thrd_success ) {
      fprintf(stderr, "failed to create thread\n");
      exit(1);
    }
  }

  {
    int r;
    thrd_join(thrd_check, &r);

  }

  // free(re_matrix);
  // free(im_matrix);
  // free(re_entries);
  // free(im_entries);
  free(root);
  free(iterations);

  mtx_destroy(&mtx);
  cnd_destroy(&cnd);

  return 0;
}








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

  for ( conv = 0, attr = DEFAULT_VALUE; conv <= CAP; ++conv ) {
    TYPE sqNorm = a_r*a_r + a_i*a_i;
    
    if ( a_r > TOL_MAX || a_r < -TOL_MAX ||
	 a_i > TOL_MAX || a_i < -TOL_MAX ) { // Check upper bound
      attr = VALUE;
      break;
    } else if ( sqNorm < Tol_MIN * Tol_MIN ) { // Check lower bound
      attr = VALUE;
      break;
    }
    if (sqNorm <= 1.002001 && sqNorm >= 0.998001)
      {
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
      }
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



void WritePPM2(  int size,
		 int* a_attr,
		 char* AllAttrColours,
		 FILE* fpa,
		 int* a_conv,
		 char* AllConvColours,
		 FILE* fpc,
		 int Colours[10][3]
		 )
{
  size_t ja=0;
  size_t jc=0;
  
  for ( size_t i = 0; i < size; ++i )
	{
	  int k = a_attr[i]; 
	  AllAttrColours[ja] = Colours[k][0] + 48;
	  AllAttrColours[ja+1] = ' ';
	  AllAttrColours[ja+2] = Colours[k][1] + 48;
	  AllAttrColours[ja+3] = ' ';
	  AllAttrColours[ja+4] = Colours[k][2] + 48;
	  AllAttrColours[ja+5] = ' ';


	  
	  short ten = (a_conv[i] / 10) +48;
	  short one = (a_conv[i] % 10) +48;
	  AllConvColours[jc]   = ten;
	  AllConvColours[jc+1] = one;
	  AllConvColours[jc+2] = ' ';
	  AllConvColours[jc+3] = ten;
	  AllConvColours[jc+4] = one;
	  AllConvColours[jc+5] = ' ';
	  AllConvColours[jc+6] = ten;
	  AllConvColours[jc+7] = one;
	  AllConvColours[jc+8] = ' ';
	  
	  ja+=6;
	  jc+=9;
	}
  

  
  fwrite(AllAttrColours, sizeof(char), size*3*2, fpa);
  fwrite(AllConvColours, sizeof(char), size*3*3, fpc);
 
}

