

# TP2 de NOM Prénom

`pandoc -s --toc tp2.md --css=./github-pandoc.css -o tp2.html`





## lscpu______________________________________________________________________________________________________

```
coller ici le résultats de lscpu. 
```

*Des infos utiles s'y trouvent : nb core, taille de cache*

Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          8
On-line CPU(s) list:             0-7
Thread(s) per core:              2
Core(s) per socket:              4
Socket(s):                       1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           140
Model name:                      11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz
Stepping:                        1
CPU MHz:                         2419.199
BogoMIPS:                        4838.39
Virtualization:                  VT-x
Hypervisor vendor:               Microsoft
Virtualization type:             full
L1d cache:                       192 KiB
L1i cache:                       128 KiB
L2 cache:                        5 MiB
L3 cache:                        8 MiB
Vulnerability Itlb multihit:     Not affected
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Not affected
Vulnerability Tsx async abort:   Not affected
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ss ht syscall nx pdpe1gb rdtscp
                                  lm constant_tsc rep_good nopl xtopology tsc_reliable nonstop_tsc cpuid pni pclmulqdq vmx ssse3 fma cx16 pcid sse4_1 sse4_2 x2api
                                 c movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm 3dnowprefetch invpcid_single ssbd ibrs ibpb st
                                 ibp ibrs_enhanced tpr_shadow vnmi ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid avx512f avx512dq rdseed ad
                                 x smap avx512ifma clflushopt clwb avx512cd sha_ni avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves avx512vbmi umip avx512_vbmi2 g
                                 fni vaes vpclmulqdq avx512_vnni avx512_bitalg avx512_vpopcntdq rdpid movdiri movdir64b fsrm avx512_vp2intersect flush_l1d arch_ca
                                 pabilities

## Produit scalaire____________________________________________________________________________________________ 

*Expliquer les paramètres, les fichiers, l'optimisation de compil, NbSamples, ...*

Pour dotproduct.cpp

OMP_NUM    | samples=1024 | 
-----------|--------------|----------
séquentiel |   | Temps produits scalaires : 0.154485
1          |   | Temps produits scalaires : 0.160185
2          |   | Temps produits scalaires : 0.0749362
3          |   | Temps produits scalaires : 0.0560558
4          |   | Temps produits scalaires : 0.0556006
8          |   | Temps produits scalaires : 0.303424


*Discuter sur ce qu'on observe, la logique qui s'y cache.*

Entre 2 et 4 thread on a bien un accélération par rapport au calcul séquentiel. Mais à partir de 8 thread le calcul
devient plus long que en séquentiel : le système perd beaucoup de temps à gérer les différents threads.

Pour dotproduct_thread.cpp 

num_threads| samples=1024 | 
-----------|--------------|----------
séquentiel |   | Temps produits scalaires : 0.154485
1          |   | Temps produits scalaires : 0.371621
2          |   | Temps produits scalaires : 0.264096
3          |   | Temps produits scalaires : 0.19716
4          |   | Temps produits scalaires : 0.193561 
8          |   | Temps produits scalaires : 0.318998

*Discuter sur ce qu'on observe, la logique qui s'y cache.*

Plus lent que OpenMd et pareil devient plus lent quand il y a trop de thread.

 *Expliquez pourquoi il n’est pas possible d’améliorer le résultat que vous avez obtenu.*

## Produit matrice-matrice

Dimension |Temps CPU produit matrice-matrice naif
----------|---------------------------------------------
1023      |1.49929 secondes
1024      |5.57088 secondes
1025      |1.4378 secondes
1026      |1.51965 secondes



### Permutation des boucles

*Expliquer comment est compilé le code (ligne de make ou de gcc) : on aura besoin de savoir l'optim, les paramètres, etc. Par exemple :*

`make TestProduct.exe && ./TestProduct.exe 1024`


  ordre           | time    | MFlops  | MFlops(n=2048) 
------------------|---------|---------|----------------
i,j,k (origine)   | 4.95311 | 433.563 |                
j,i,k             | 4.89364 | 438.832 |    
i,k,j             | 13.4916 |  159.17 |    
k,i,j             | 9.87409 | 217.487 |    
j,k,i             | 0.822827| 2609.89 |    
k,j,i             | 0.915193 | 2346.48 |    


*Discussion des résultats*
On remarque que les deux derniers ordres sont nettement plus rapides. En effet lorsque la boucle sur les i est la 
plus intérieure, on effectue deux accès linéaires (pour A et C) ce qui permet d’exploiter les lignes
de cache ou profiter de la mémoire entrelacée, et on reutilise le même B dans la boucle la plus interne.

On conserve l'ordre j,k,i


### OMP sur la meilleure boucle 

`make TestProduct.exe && OMP_NUM_THREADS=8 ./TestProductMatrix.exe 1024`
for ((i=1;i<=8;i++)); do OMP_NUM_THREADS=$i ./TestProductMatrix.exe 1024; done


  OMP_NUM         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 | 2004.12 |1969.05         |2349.92         |2231.1
2                 | 3061.4  |3029.44         |3769.37         |3298.25
3                 | 5100.11 | 4769.93        |4461.69         |5006.68
4                 | 6240.87 |6813.24         |5977.73         |6962.84
5                 | 7767.49 | 8563.06        |8060.25         |6739.36
6                 | 9355.48 |9505.04         |8267.54         |6683.37
7                 | 10465.1 |9821.31         |9053.89         |6638.9
8                 | 10102.6 |10667.5         |10340.2         |6628.68

On commence à stagner à 6 threads.Sauf pour n= 4096 ou on commence à stagner à 4 threads . 
On peut clairement améliorer les résultats en exploitant davantage la mémoire caches

### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=max)    | 1982.69 |2083.09         |1758.53         |1613.33
32                | 1848.29 |1875.54         |1255.44         |1479.83
64                | 1662.38 |1566.22         |1139.01         |1248.53
128               | 2062.86 |1876.13         |1026.3          |1325.18
256               | 1619.87 |1446.29         |1672.07         |1535.02
512               |  1452.76| 1276           |1103.38         |1386.37
1024              |  |




### Bloc + OMP



  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
---------------|---------|---------|------------------------------------------------
A.nbCols       |  1      | 2055.01 | 2014.72        |1827.51         |1489.35
32             |  8      |9810.68  | 10993          |7633.25         |10633.5
64             | 8       |8701.06  |11424.3         |8919.62         |8925.79
128            | 8       |10115.2  |9298.4          |2972.67         |4575.09
256            |8        |6593.3   |6193.77         |3562.11         |5302.89


La taille de bloc permettant d'optimiser au mieux le calcul est szBlock = 32. On compare alors les résultats de cette ligne à la version séquentielle
  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
---------------|---------|---------|----------------|----------------|--------------
Séquentielle   |  1      | 1848.29 |1875.54         |1255.44         |1479.83
32             |  8      |9810.68  | 10993          |7633.25         |10633.5

L'accélération est claire. Comment expliquer cela ? Le calcul par blocs permet d'exploiter la mémoire cache.
. De plus, le nombre de cœfficients de C et de A stockés pour chaque boucle en i et j est suffisamment petit pour rester en mémoire cache, si bien que
la boucle en k pourra au fur et à mesure des itérations réutiliser les données de C stockées
en cache. De même, la boucle en j pourra réutiliser les données de A stockées en cache lors
du premier passage dans la boucle en i.
 
# Tips 

```
	env 
	OMP_NUM_THREADS=4 ./dot_product.exe
```

```
    $ for i in $(seq 1 4); do elap=$(OMP_NUM_THREADS=$i ./TestProductOmp.exe|grep "Temps CPU"|cut -d " " -f 7); echo -e "$i\t$elap"; done > timers.out
```


Tri Bitonic________________________________________________________________

# Pour l'algorithme séquentiel 

Temps calcul tri sur les entiers : 0.700907
Temps calcul tri sur les vecteurs : 35.1449

#pour l'algorithme paralléliser avec thread
Temps calcul tri sur les entiers : 0.256993
Temps calcul tri sur les vecteurs : 20.927

=> accélération obtenue : 
Pour tri sur les entiers : 2,72
Pour tri sur les vecteurs : 1,579

Le tri sur les entiers est plus accéléré 


QUESTION ?
Correction propose de limiter de la profondeur à 4 pourquoi ?

BHUDDA______________________________________________________________________________


Nombre de threads | Bhudda 1 | Bhudda 2 | Bhudda 3
------------------|----------|----------|------------------------
1                 | 4.34843  | 4.16513  |0.256229
2                 |2.75512   |2.15874   |0.149307
3                 | 2.10036  | 1.61338  |0.10617
4                 |1.89628   |1.3187    |0.0837627
8                 |1.28299   |0.782367  |0.0537897


Sans surprise le programme est plus rapide quand on rajoute des threads. Et on commence à stager à partir de 4 threads.

Remarque si on rajoute un clause au schedule (#pragma omp parallel schedule(dynamic,10))
Peu importe le nombre de thread on a un temps équivalent (très rapide à 1 sec), dès qu'il a fini le thread demande un bloc de 10 élèments à calculer d'un coup. 
(le problème avec dynamic : à chaque fois qu'un thread termine sa tache il doit demander une nouvelle tache)  

Accélération(nombre de thread) = tempsparallèle(nombre de thread)/tempsséquentiel
