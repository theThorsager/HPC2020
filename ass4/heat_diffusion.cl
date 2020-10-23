
__kernel void heatEq(
__global double* h,
__global double* nh,
double c,
int width,
int height
)
{
int ix = get_global_id(0);
int jx = get_global_id(1);

*(nh+width*ix+jx)=*(h+width*ix+jx) +  c*((*(h+width*(ix-1)+jx)+*(h+width*(ix+1)+jx)+	   *(h+width*ix+jx-1)+*(h+width*ix+jx+1))*0.25-*(h+width*ix+jx));
}