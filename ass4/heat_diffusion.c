#include <stdio.h>
#include <string.h>

#define MAX_SOURCE_SIZE (0x100000)

#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>


int
main(
     int argc,
     char* argv[])
{
  // parse command line arguments
  size_t iter = 1;
  float c = 0.01;
  if (argc >= 3) {
    if (argv[1][1] == 'n') {
	iter = atoi(argv[1] + 2);
	c = (float)(double)atof(argv[2] + 2);
      } else {
	iter = atoi(argv[2] + 2);
	c = (float)(double)atof(argv[1] + 2);
      }
    }
  // printf("-n%d -d%f\n", iter, c);
  
  // Parsing from the file
  // float** matrix;
  int dim[2];
  char* filePath = "./diffusion";
  FILE* file = fopen(filePath,"r");
  int dimsizeskip;
  {
    if ( file == NULL)
      {
	printf("Failed to open file sademoji!\n");
	exit(1);
      }

    int firstDataLineSize = 20;
    char* dimString = malloc(sizeof(char)* firstDataLineSize);
    fread(dimString, sizeof(char), firstDataLineSize,file);

    int dimsz = 2;
    // int dim[2] = {0, 0};
    int DimIndex = 0;

    int j = 0;
    
    for ( size_t i = 0; i < firstDataLineSize; ++i )
      {
	if( dimString[i] == ' ' || dimString[i] == '\n')
	  {
	    char dest[i-j+1];
	    dest[i-j] = '\0';
	    strncpy(dest, dimString + j,i-j);
	    dim[DimIndex] = atoi(dest);

	    DimIndex++;
	    if( DimIndex == 2)
	      {
		dimsizeskip=i+1;
		break;
	      }
	    j=i+1;
	  }
      }

  }
    //create temp matrix full of zeros
    float* tempEntries = calloc((dim[0]+2)*(dim[1]+2), sizeof(float));
    float** temp = malloc(sizeof(float*)*(dim[0]+2));
    for ( size_t ix = 0, jx = 0; ix <dim[0]+2; ++ix, jx+=dim[1]+2)
      temp[ix] = tempEntries + jx;

    {
    // find the length of the dimension part of the data

    //find the size of the data file without dimension line
    fseek(file,0,SEEK_END);
    int fileEnd = ftell(file);
    fseek(file,dimsizeskip,SEEK_SET);
    int fileStart = ftell(file);
    int filelength = fileEnd-fileStart;

    //read eveything in file
    char* data = malloc(sizeof(char) *filelength);
    fread(data, sizeof(char), filelength,file);

    int j = 0;
    int k = 0;

    int X;
    int Y;
    for( size_t i = 0; i < filelength; i++)
      {
	if( data[i] == ' ' || data[i] == '\n')
	  {
	    char dest[i-j+1];
	    dest[i-j] = '\0';
	    strncpy(dest, data + j,i-j);

	    switch (k)
	      {
	      case 0:
		X = atoi(dest);
		k=1;
		break;
	      case 1:
		Y = atoi(dest);
		k=2;
		break;
	      case 2:
		temp[X+1][Y+1]= (float)(double)atof(dest);
		k=0;
		break;
	      }
	    j=i+1;
	  }
      }
    free(data);
  }

  
  // Get platform
  cl_int error;
  cl_platform_id platform_id;
  cl_uint nmb_platforms;
  if (clGetPlatformIDs(1, &platform_id, &nmb_platforms) != CL_SUCCESS) {
      printf( "cannot get platform\n" );
      return 1;
      }

  // Get devices
  cl_device_id device_id;
  cl_uint nmb_devices;
  if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1,
                     &device_id, &nmb_devices) != CL_SUCCESS) {
      printf( "cannot get device\n" );
      return 1;
    }

  // Create Context
  cl_context context;
  cl_context_properties properties[] =
    {
     CL_CONTEXT_PLATFORM,
     (cl_context_properties) platform_id,
     0
    };
  context = clCreateContext(properties, 1, &device_id, NULL, NULL, &error);

  // Create command Queue
  cl_command_queue command_queue;
  command_queue = clCreateCommandQueue(context, device_id, 0, &error);
  if (error != CL_SUCCESS) {
    printf("cannot create context\n");
    return 1;
  }

  //Load things into buffers
  int width = dim[1];
  int height = dim[0];
  int sz_with_padding = (width+2)*(height+2);
  
  float* matrix_a = temp[0];
  float* matrix_b = calloc(sizeof(float),sz_with_padding);

  // Create memory buffers on the device for each matrix 
  cl_mem mem_matrix_a = clCreateBuffer(context, CL_MEM_READ_WRITE, sz_with_padding*sizeof(float), NULL, &error);
  cl_mem mem_matrix_b = clCreateBuffer(context, CL_MEM_READ_WRITE, sz_with_padding*sizeof(float), NULL, &error);

  

  //    load data to buffers
  error = clEnqueueWriteBuffer(command_queue, mem_matrix_a, CL_TRUE, 0,sz_with_padding*sizeof(float), matrix_a, 0, NULL, NULL);
  error = clEnqueueWriteBuffer(command_queue, mem_matrix_b, CL_TRUE, 0,sz_with_padding*sizeof(float), matrix_b, 0, NULL, NULL);
  
  // ---Load the kernel source code into the array source_str----
  FILE *fp;
  char *source_str;
  size_t source_size;
  
  fp = fopen("heat_diffusion.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    exit(1);
  }
  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose( fp );
  
  // Create a program from the kernel source
  cl_program program = clCreateProgramWithSource(context, 1,(const char **)&source_str, (const size_t *)&source_size, &error);
 
  // Build the program
  error = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  
  if (error != CL_SUCCESS)
    {

      printf("cannot build program. log:\n");
      size_t log_size = 0;
      clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
			    0, NULL, &log_size);
      char * log = calloc(log_size, sizeof(char));
      if (log == NULL)
	{
	  printf("could not allocate memory\n");
	  return 1;
	}
      clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
			    log_size, log, NULL);
      printf( "%s\n", log );
      free(log);
      return 1;
    }
      
     

  
  
  // Create the OpenCL kernel
  cl_kernel kernelE = clCreateKernel(program, "heatEq", &error);
  cl_kernel kernelO = clCreateKernel(program, "heatEq", &error);
  if (error != CL_SUCCESS)
    {
      printf("Failed to create the kernel, error code: %d\n", error);
      exit(1);
    }
  int pW = width + 2;
  int pH = height + 2;
 
  // Set arguments to kernel
  error = clSetKernelArg(kernelE,0,sizeof(cl_mem),(void*) &mem_matrix_a);
  error = clSetKernelArg(kernelE,1,sizeof(cl_mem),(void*) &mem_matrix_b);
  error = clSetKernelArg(kernelE,2,sizeof(c), &c);
  error = clSetKernelArg(kernelE,3,sizeof(pW), &pW);
  error = clSetKernelArg(kernelE,4,sizeof(pH), &pH);
  
  error = clSetKernelArg(kernelO,1,sizeof(cl_mem),(void*) &mem_matrix_a);
  error = clSetKernelArg(kernelO,0,sizeof(cl_mem),(void*) &mem_matrix_b);
  error = clSetKernelArg(kernelO,2,sizeof(c), &c);
  error = clSetKernelArg(kernelO,3,sizeof(pW),  &pW);
  error = clSetKernelArg(kernelO,4,sizeof(pH), &pH);
  
if (error != CL_SUCCESS)
    {
      printf("Failed to set the arguments, error code: %d\n", error);
      exit(1);
    }

  // Execute the OpenCL kernel on the list
  size_t global_item_size[2] = { height, width }; // Process the entire lists
  size_t local_item_size[2] = { 25, 25 }; // Divide work items into groups of 64

  const size_t offset[2] = {1, 1};

  
  
  // Loop for number of iterations
  size_t ix;
  for (ix = 0; ix < iter; ++ix) {
    // execute kernel
    error = clEnqueueNDRangeKernel(command_queue,
				   ix % 2 == 0 ? kernelE : kernelO,
				   2, offset,
				   global_item_size, local_item_size,
				   0, NULL, NULL);	   
  }

  // read results from buffer
  float* result = malloc(sizeof(float)*sz_with_padding);
  error = clEnqueueReadBuffer(command_queue,
			      ix % 2 == 0 ? mem_matrix_a : mem_matrix_b,   // Test which is right
			      CL_TRUE, 0,
			      sz_with_padding*sizeof(float), result,
			      0, NULL, NULL);

  if (error != CL_SUCCESS)
    {
      printf("Failed to read teh results, error code: %d\n", error);
      exit(1);
    }

  // post proccessing
  //    Calculate average temp
  const size_t N = (width+2)*(height+2);
  double averageT = 0;

  for (size_t ix = 0; ix < N; ++ix)
    averageT += (double)result[ix]; //its fine to add padding because it's 0
  averageT /= width*height; // we don't want the average to include padding

  printf("%f\n", averageT);
  //    Calculate differance from average temp

  double absAverageT = 0;
  for (size_t ix = 0; ix < N; ++ix)
    {
      if ( ix > width+2  &&  ix % (width+2) != 0  &&  ix % (width+2) != (width+2)-1  &&  ix < ((height+2)-1)*(width+2) )
	{
	  double abs = ((double)result[ix]) - averageT;
	  absAverageT += abs < 0 ? -abs : abs;
	}
    }
  absAverageT /= width*height; // used to be N
  printf("%f\n", absAverageT);
  
//Testing: making results 2d
/*
  float ** a_result = malloc(sizeof(float)*(height+2));
  for ( size_t ix = 0, jx = 0; ix < height+2; ++ix, jx += width+2 )
    a_result[ix] = result + jx;
  
    double absAverageT = 0;

    for ( size_t ix = 1; ix < height + 1; ++ix )
      for ( size_t jx = 1; jx < width + 1; ++jx )
	{
	  double abs = a_result[ix][jx] - averageT;
	  absAverageT += abs < 0 ? -abs : abs;
	}
    
  absAverageT /= width*height; // used to be N
  printf("%f\n", absAverageT);
  // end test
*/
  
  // Release Command Queue
  error = clFlush(command_queue);
  error = clFinish(command_queue);
  error = clReleaseKernel(kernelE);
  error = clReleaseKernel(kernelO);
  error = clReleaseMemObject(mem_matrix_a);
  error = clReleaseMemObject(mem_matrix_b);
  error = clReleaseCommandQueue(command_queue);
  //Release program
  error = clReleaseProgram(program);

  // Release Context
  error = clReleaseContext(context);

  //free(source_str);
  free(result);
  free(tempEntries);
  free(temp);
  //free(matrix_a);
  free(matrix_b);
  //free(a_result);
  // Free more things
  
  return 0;
}

