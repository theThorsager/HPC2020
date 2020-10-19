

__kernel void heatEq(
__global const float** h,
__global float** nh,
const float c,
const int width,
const int height
)
{
// h(i,j) + c * ( (h(i-1,j) + h(i+1,j) + h(i,j-1) + h(i,j+1))/4 - h(i,j) );
int ix = get_global_id(0);
int jx = get_global_id(1);

float left;
float right;
float up;
float down;

if (jx == 0) {
   left = 0;
   right = h[ix,jx+1];
} else if (jx == width - 1) {
   left = h[ix,jx-1];
   right = 0;
} else {
   left = h[ix,jx-1];
   right = h[ix,jx+1];
}

if (ix == 0) {
   up = 0;
   down = h[ix+1,jx];
} else if (ix == height - 1) {
   up = h[ix-1,jx];
   down = 0;
} else {
   up = h[ix-1,jx];
   down = h[ix+1,jx];
}

nh[ix,jx] = h[ix,jx] +
	    c*(
	      ( up + down + left + right
	  	    ) * 0.25 - h[ix,jx] );
}