
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

if(ix==0 || ix==height-1 || jx==0 || jx==width-1){
	 *(nh+width*ix+jx)=0;
}else{
*(nh+width*ix+jx)=*(h+width*ix+jx) +  c*((*(h+width*(ix-1)+jx)+*(h+width*(ix+1)+jx)+
											   *(h+width*ix+jx-1)+*(h+width*ix+jx+1))*0.25-*(h+width*ix+jx));
}
//nh[ix][jx] = h[ix][jx] +
//	    c[0]*(
//	      ( h[ix-1][jx] + h[ix+1][jx] + h[ix][jx-1] + h[ix][jx+1]
//	  	    ) * 0.25 - h[ix][jx] );
}