#include <stdio.h>

#define MAX_SOURCE_SIZE (0x100000)

#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

void READ(float** temp, int* dim);

int
main(
     int argc,
     char* argv[])
{
  // parse command line arguments
  size_t iter = 1;
  float c = 0.01f;
  if (argc >= 3) {
      if (argv[1][1] == 'n') {
	iter = atoi(argv[1] + 2);
	c = atof(argv[2] + 2);
      } else {
	iter = atoi(argv[2] + 2);
	c = atof(argv[1] + 2);
      }
    }
  // printf("-n%d -d%f\n", iter, c);
  

  float** matrix;
  int dim[2];
  READ(matrix, dim);
 
  



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
  
  // Create memory buffers on the device for each matrix 
  cl_mem mem_matrix_a = clCreateBuffer(context, CL_MEM_WRITE_READ, width*height*sizeof(float), NULL, &error);
  cl_mem mem_matrix_b = clCreateBuffer(context, CL_MEM_WRITE_READ, width*height*sizeof(float), NULL, &error);
  cl_mem mem_c = clCreateBuffer(context, CL_MEM_READ_ONLY,sizeof(float), NULL, &error);
  //    load data to buffers
  error = clEnqueueWriteBuffer(command_queue, mem_matrix_a, CL_TRUE, 0,width*height*sizeof(float), matrix_a, 0, NULL, NULL);
  error = clEnqueueWriteBuffer(command_queue, mem_matrix_b, CL_TRUE, 0,width*height*sizeof(float), matrix_b, 0, NULL, NULL);
  error = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,sizeof(float), c, 0, NULL, NULL);

  // Load the kernel source code into the array source_str
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
    
  // Create the OpenCL kernel
  cl_kernel kernelE = clCreateKernel(program, "heat_diffusion", &error);
  cl_kernel kernelO = clCreateKernel(program, "heat_diffusion", &error);

  // Set arguments to kernel
  error=clSetKernelArg(kernelE,0,sizeof(matrix_a),(void*) &mem_matrix_a);
  error=clSetKernelArg(kernelE,1,sizeof(matrix_b),(void*) &mem_atrix_b);
  error=clSetKernelArg(kernelE,2,sizeof(float), (void*) &mem_c);

  error=clSetKernelArg(kernelO,1,sizeof(matrix_a),(void*) &mem_matrix_a);
  error=clSetKernelArg(kernelO,0,sizeof(matrix_b),(void*) &mem_atrix_b);
  error=clSetKernelArg(kernelO,2,sizeof(float), (void*) &mem_c);


  // Execute the OpenCL kernel on the list
  size_t global_item_size = LIST_SIZE; // Process the entire lists
  size_t local_item_size = 64; // Divide work items into groups of 64

  clu_int offset = {1, 1};
  
  // Loop for number of iterations
  for (size_t ix = 0; ix < iter; ++ix) {
    // execute kernel
    error = clEnqueueNDRangeKernel(command_queue,
				   ix % 2 == 0 ? kernelE : kernelO,
				   2, offset,
				   &global_item_size, &local_item_size,
				   0, NULL, NULL);	   
  }

  // read results from buffer

  float* result; // add some reading from buffer here

  ret = clEnqueueReadBuffer(command_queue, mem_c, CL_TRUE, 0,
			    LIST_SIZE * sizeof(float), result, 0, NULL, NULL);
  
  // post proccessing
  //    Calculate average temp
  const size_t N = width*height;
  float averageT = 0;

  for (size_t ix = 0; ix < N; ++ix)
    averageT += result[ix];
  averageT /= (N + 4 - width*2 - height*2);
  
  //    Calculate differance from average temp
  float absAverageT = 0;
  for (size_t ix = 0; ix < N; ++ix)
    absAverageT += result[ix] - averageT;
  absAverageT /= (N + 4 - width*2 - height*2);
  
  // Release Command Queue
  clReleaseCommandQueue(command_queue);

  //Release program
  clReleaseProgram(program);

  // Release Context
  clReleaseContext(context);

  free(source_str);
  // Free more things

  return 0;
}

