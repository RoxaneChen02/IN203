##SUM DE DEUX VECTEURS

mod1 = SourceModule("""
__global__ void sum( int dim_vec, float *a, float* b, float* c )
{
	int idx = blockIdx.x*blockDim.x + threadIdx.x;
  if (idx < dim_vec)
  {
    c[idx]  = a[idx] + b[idx];
  }
}
""")

import numpy
numpy.random.seed(1729)
a = numpy.random.randn(4,1)
b = numpy.random.randn(4,1)
c= numpy.random.randn(4,1)
a = a.astype(numpy.float32)
print(a.nbytes)
a_gpu = cuda.mem_alloc(a.nbytes)
cuda.memcpy_htod(a_gpu, a)
b = b.astype(numpy.float32)
print(b.nbytes)
b_gpu = cuda.mem_alloc(b.nbytes)
cuda.memcpy_htod(b_gpu, b)

c = c.astype(numpy.float32)
print(c.nbytes)
c_gpu = cuda.mem_alloc(c.nbytes)
cuda.memcpy_htod(c_gpu, c)



func = mod1.get_function("sum")
dim = numpy.int32(4)
func(dim, a_gpu,b_gpu,c_gpu, block=(4,1,1))

cuda.memcpy_dtoh(c, c_gpu)

print(f"a : {a}")
print(f"b : {b}")
print(f"c : {c}")



##SOMME DE DEUX MATRICES

mod1 = SourceModule("""
__global__ void sum( int dim_x, int dim_y, float *a, float* b, float* c )
{
	int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy  =blockIdx.y*blockDim.y + threadIdx.y;
  if ((idx < dim_x)&&(idy < dim_y))
  {
    c[idy*dim_x + idx]  = a[idy*dim_x+idx] + b[idy*dim_x+idx];
  }
}
""")


import numpy
numpy.random.seed(1729)
a = numpy.random.randn(4,4)
b = numpy.random.randn(4,4)
c= numpy.random.randn(4,4)
a = a.astype(numpy.float32)
print(a.nbytes)
a_gpu = cuda.mem_alloc(a.nbytes)
cuda.memcpy_htod(a_gpu, a)
b = b.astype(numpy.float32)
print(b.nbytes)
b_gpu = cuda.mem_alloc(b.nbytes)
cuda.memcpy_htod(b_gpu, b)

c = c.astype(numpy.float32)
print(c.nbytes)
c_gpu = cuda.mem_alloc(c.nbytes)
cuda.memcpy_htod(c_gpu, c)



func = mod1.get_function("sum")
dim = numpy.int32(4)
func(dim,dim, a_gpu,b_gpu,c_gpu, block=(4,4,1), grid = ())

cuda.memcpy_dtoh(c, c_gpu)

print(f"a : {a}")
print(f"b : {b}")
print(f"c : {c}")



________________________________________________________________________


###EXERCICE FINAL

import pylab as plt
import numpy as np
# initial values 
loop = 100 # number of interations
div = 1000 # divisions
# all possible values of c
c = np.linspace(-2,2,div)[:,np.newaxis] + 1j*np.linspace(-1.5,1.5,div)[np.newaxis,:]

cr = c.real
ci = c.imag
# array of ones of same dimensions as c
ones = np.ones(np.shape(c), np.int)
# Array that will hold colors for plot, initial value set here will be
# the color of the points in the mandelbrot set, i.e. where the series
# converges.
# For the code below to work, this initial value must at least be 'loop'.
# Here it is loop + 5
color = ones * loop + 5
z = 0



mod1 = SourceModule("""
__global__ void mandelbrot_iter( int dim_x, int dim_y, float* cr, float* ci,int* color )
{	
	float zr=0;
  float zr_new = 0;
	float zi = 0;
	int indice = 0;
	int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy  =blockIdx.y*blockDim.y + threadIdx.y;
	for (int n=0;n<100;++n){
     	zr_new = zr*zr-zi*zi + cr[idx+idy*dim_x];
     	zi = 2*zr*zi + ci[idx+idy*dim_x] ;
     	zr = zr_new;
     	if (zr*zr+zi*zi < 4){
     		indice = n;
     	}
       }

    color[idx+idy*dim_x] = min(color[idx+idy*dim_x], indice ) 	;
      
}
""")

cr = cr.astype(numpy.float32)

cr_gpu = cuda.mem_alloc(cr.nbytes)
cuda.memcpy_htod(cr_gpu, cr)

ci = ci.astype(numpy.float32)

ci_gpu = cuda.mem_alloc(ci.nbytes)
cuda.memcpy_htod(ci_gpu, ci)

color = color.astype(numpy.float32)

color_gpu = cuda.mem_alloc(color.nbytes)
cuda.memcpy_htod(color_gpu, color)

func = mod1.get_function("mandelbrot_iter")
dim = numpy.int32(1000)
func(dim,dim, cr_gpu,ci_gpu,color_gpu, block=(16,16,1), grid = (np.int((div+15)/16),np.int((div+15)/16),1))

cuda.memcpy_dtoh(color, color_gpu)

plt.rcParams['figure.figsize'] = [12, 7.5]
# contour plot with real and imaginary parts of c as axes
# and colored according to 'color'
plt.contourf(c.real, c.imag, color)
plt.xlabel("Real($c$)")
plt.ylabel("Imag($c$)")
plt.xlim(-2,2)
plt.ylim(-1.5,1.5)
plt.savefig("plot.png")
plt.show()