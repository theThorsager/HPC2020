
__kernel void heatEq(
__global float* h,
__global float* nh,
float c,
int width,
int height
)
{
int ix = get_global_id(0);
int jx = get_global_id(1);

int wix = width*ix;
float temp = *(h+wix+jx);


*(nh+wix+jx) = temp +  c*((*(h-width+wix+jx)+*(h+width+wix+jx)+	   *(h+wix+jx-1)+*(h+wix+jx+1))*0.25 - temp);
}