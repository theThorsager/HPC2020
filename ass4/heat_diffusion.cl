
__kernel void heatEq(
__global float* h,
__global float* nh,
__global const float* c,
__global const int* width
)
{
int ix = get_global_id(0);
int jx = get_global_id(1);

*(nh+width[0]*ix+jx)=*(h+width[0]*ix+jx) +  c[0]*((*(h+width[0]*(ix-1)+jx)+*(h+width[0]*(ix+1)+jx)+
											   *(h+width[0]*ix+jx-1)+*(h+width[0]*ix+jx+1))*0.25-*(h+width[0]*ix+jx));

//nh[ix][jx] = h[ix][jx] +
//	    c[0]*(
//	      ( h[ix-1][jx] + h[ix+1][jx] + h[ix][jx-1] + h[ix][jx+1]
//	  	    ) * 0.25 - h[ix][jx] );
}