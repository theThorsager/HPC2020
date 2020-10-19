#include <CL/cl.h>
#include<stdio.h>

#define MAX_SOURCE_SIZE (0x100000)

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
  if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1,
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
  cl_program program = clCreateProgramWithSource(context, 1,(const char **)&source_str, (const size_t *)&source_size, &error/*&ret*/);
 
  // Build the program
  error = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    

  // Create the OpenCL kernel
  cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

  

  // Release Command Queue
  clReleaseCommandQueue(command_queue);

  //Release program
  clReleaseProgram(program);

  // Release Context
  clReleaseContext(context);

  free(source_str);

  return 0;
}

