# JellySimulation
A simple jelly simulation using Position Based Dynamics.

## Introduction
This project references to the famous [Position based dynamics (PBD)](https://matthias-research.github.io/pages/publications/posBasedDyn.pdf). However, traditional PBD has several problems
* The stiffness is depend on time step.
* Setting **relative** stiffness is difficult.

To address these problem, [XPBD](http://mmacklin.com/xpbd.pdf) is introduced by Miles Macklin and his fellows. Unlike original PBD, XPBD allows users to accurately and efficiently simulate arbitrary elastic and dissipative
energy potentials in an implicit manner.

In this project (code), I provided a simple demostration of how PBD works. With this friendly(?) interface, it should be very easy to create applications based on PBD/XPBD.

## Background - Position-based dynamics
In PBD, each constraints updates its position with

$\Delta x = k_js_jM^{-1}\nabla C_j(x_i)$,

where $i$ denotes the iteration index, $j$ is the constraint index, and $k \in[0,1]$. And $s_j$ is the scalaring factor that

$s_j = \dfrac{-C_j(x_i)}{\nabla C_j M^{-1} \nabla C_j^T}$


**Original PBD algorithm**
1. Predict position with velocity $v$ and external forces $f_{ext}$
2. Initialize $x_0$ 
3. while $i<iterationCount$
    - Compute $\Delta x$
    - Update $\Delta x$
4. Update positions
5. Update velocities 
    - $v_{n+1} = \frac{1}{\Delta t}(x_{n+1} - x_{n})$ 

In contrast, XPBD uses Lagrangian rather than throw away that value. It is accumulated in a per-constraint variable $\lambda$.

**XPBD algorithm**
1. Predict position $x_{predict}$ with velocity $v$ and external forces $f_{ext}$
2. Initialize $x_0 = x_{predict}$ 
3. **Initialize $\lambda_0 = 0$**
4. While $i<iterationCount$
    - Compute $\Delta x$
    - **Compute $\Delta \lambda$**
    - Update $x = x + \Delta x$
    - **Update $\lambda = \lambda + \Delta \lambda$**
5. Update positions
6. Update velocities 
    - $v_{n+1} = \frac{1}{\Delta t}(x_{n+1} - x_{n})$ 



## Experiment Result 
**PBD**

![](https://i.imgur.com/3OfJ97m.gif)

**XPBD**

![](https://i.imgur.com/CgO8adS.gif)

* stiffness setting 
    * PBD stiffness: 0.025
        * The value of PBD stiffness locates between 0 to 1
    * XPBD compliance: 0.0005    
    * **Notice**: PBD and XPBD stiffness is very hard to compare (not in the same unit) because PBD stiffness is **relative** stiffness.

## References
* *M. Müller, B. Heidelberger, M. Hennix, and J. Ratcliff, “Position based dynamics,” Journal of Visual Communication and Image Representation, vol. 18, pp. 109–118, apr 2007.*
* *M. Macklin, M. Müller, and N. Chentanez, “XPBD,” in Proceedings of the 9th International Conference on Motion in Games - MIG '16, ACM Press, 2016.*

## License
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
