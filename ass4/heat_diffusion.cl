
__kernel void heatEq(
__global const float** h,
__global float** nh,
const float c,
)
{
// h(i,j) + c * ( (h(i-1,j) + h(i+1,j) + h(i,j-1) + h(i,j+1))/4 - h(i,j) );
int ix = get_global_id(0);
int jx = get_global_id(1);

nh[ix,jx] = h[ix,jx] +
	    c*(
	      ( h[ix-1,jx] + h[ix+1,jx] + h[ix,jx-1] + h[ix,jx+1]
	  	    ) * 0.25 - h[ix,jx] );
}