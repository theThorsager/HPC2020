#include <CL/cl.h>
#include<stdio.h>

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
  context = clCreateContext(properties, 1, &device_id, NULL, NULL, NULL, NULL, NULL, &error);

  // Create command Queue
  cl_command_queue command_queue;
  command_queue = clCreateCommandQueue(context, device_id, 0, &error);
  if (error != CL_SUCCESS) {
    printf("cannot create context\n");
    return 1;
  }

  


  // Release Command Queue
  clReleaseCommandQueue(command_queue);
  
  // Release Context
  clReleaseContext(context);
  return 0;
}

