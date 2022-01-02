##MandelBrot avec OpenMD+MPI

Je n'arrive pas à faire compiler mon programme 
Ce message d'erreur apparaît
Mandelbrot.cpp:154: warning: ignoring #pragma omp parallel [-Wunknown-pragmas]
  154 | #pragma omp parallel for schedule (dynamic) num_threads(nt)
      | 
 
Pour régler ce problème : je compile à la main avec 
mpic++ -std=c++14 -O3 -march=core-avx2 -Wall -o Mandelbrot.exe Mandelbrot.cpp -lmpi -fopenmp
mpiexec -np nbrdeproc ./Mandelbrot.exe

Nombre de processeur | Valeur de nt | Temps de calcul
---------------------|--------------|------------------
2                    | 5            |11.2094
2                    |10            |8.83839
2                    |2             |9.1102
4                    |2             |3.05489

##Mandelbrot TBB 

                  |Temps calcul ensemble mandelbrot
------------------|---------------------------------
auto_partitioner  |4.85557
simple_partitioner|3.95876
static_partitioner|8.30449

On est beaucoup plus rapide avec parallélisation TBB

##Parallélisation d'un raytracer avec TBB

Temps en séquentiel : Calcul de l'image en 4.62097 secondes.

Problème de compilation après parallélisation: compilation à la main avec : 

g++ -std=c++17 -fPIC -O3 -march=core-avx2 -O2 -march=core-avx2 -Wall raytracer.cpp -o raytracer.exe -lm -ltbb

  Grainsize         |2                  |100               | 1000
--------------------|-------------------|------------------|-----------------
simple_partitioner  |0.748562 secondes  |0.778786          |1.37568 secondes
auto_partitioner    |0.730064 secondes  |1.04489 secondes  | 1.19029 secondes
static_partitioner  | 0.978775 secondes |0.949993 secondes | 1.1378 secondes

Augmentation avec la valeur de Grainsize



