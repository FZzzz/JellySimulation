# JellySimulation
A simple jelly simulation using Position Based Dynamics.

## Introduction
This project references to the famous [Position based dynamics (PBD)](https://matthias-research.github.io/pages/publications/posBasedDyn.pdf). However, traditional PBD has several problems
* The stiffness is depend on time step.
* Setting **relative** stiffness is difficult.

To address these problem, [XPBD](http://mmacklin.com/xpbd.pdf) is introduced by Miles Macklin and his fellows. Unlike original PBD, XPBD allows users to accurately and efficiently simulate arbitrary elastic and dissipative
energy potentials in an implicit manner.

In this project (code), I provided a simple demostration of how PBD works. With this friendly(?) interface, it should be very easy to create applications based on PBD/XPBD.

## Build
Click on `JellySimulation.sln` and build it in Visual Studio (2019 is prefered). 

## Background - Position-based dynamics
In PBD, each constraints updates its position with

![\Delta x = k_js_jM^{-1}\nabla C_j(x_i)](https://render.githubusercontent.com/render/math?math=%5CDelta%20x%20%3D%20k_js_jM%5E%7B-1%7D%5Cnabla%20C_j(x_i)),

where ![i](https://render.githubusercontent.com/render/math?math=i) denotes the iteration index, ![j](https://render.githubusercontent.com/render/math?math=j) is the constraint index, and ![k \in\[0,1\]](https://render.githubusercontent.com/render/math?math=k%20%5Cin%5B0%2C1%5D). And ![s_j](https://render.githubusercontent.com/render/math?math=s_j) is the scalaring factor that

![s_j = \dfrac{-C_j(x_i)}{\nabla C_j M^{-1} \nabla C_j^T}](https://render.githubusercontent.com/render/math?math=s_j%20%3D%20%5Cdfrac%7B-C_j(x_i)%7D%7B%5Cnabla%20C_j%20M%5E%7B-1%7D%20%5Cnabla%20C_j%5ET%7D)


**Original PBD algorithm**
1. Predict position with velocity $v$ and external forces ![f_{ext}](https://render.githubusercontent.com/render/math?math=f_%7Bext%7D)
2. Initialize ![x_0](https://render.githubusercontent.com/render/math?math=x_0)
3. while ![i < iterationCount](https://render.githubusercontent.com/render/math?math=i%20%3C%20iterationCount)
    - Compute ![\Delta x](https://render.githubusercontent.com/render/math?math=%5CDelta%20x)
    - Update![\Delta x](https://render.githubusercontent.com/render/math?math=%5CDelta%20x)
4. Update positions
5. Update velocities 
    - ![v_{n+1} = \frac{1}{\Delta t}(x_{n+1} - x_{n})](https://render.githubusercontent.com/render/math?math=v_%7Bn%2B1%7D%20%3D%20%5Cfrac%7B1%7D%7B%5CDelta%20t%7D(x_%7Bn%2B1%7D%20-%20x_%7Bn%7D))

In contrast, XPBD uses Lagrangian rather than throw away that value. It is accumulated in a per-constraint variable ![\lambda](https://render.githubusercontent.com/render/math?math=%5Clambda).

**XPBD algorithm**
1. Predict position![x_{predict}](https://render.githubusercontent.com/render/math?math=x_%7Bpredict%7D) with velocity $v$ and external forces ![f_{ext}](https://render.githubusercontent.com/render/math?math=f_%7Bext%7D)
2. Initialize ![x_0](https://render.githubusercontent.com/render/math?math=x_0)
3. **Initialize ![\lambda_0 = 0](https://render.githubusercontent.com/render/math?math=%5Clambda_0%20%3D%200)**
4. While ![i < iterationCount](https://render.githubusercontent.com/render/math?math=i%20%3C%20iterationCount)
    - Compute ![\Delta x](https://render.githubusercontent.com/render/math?math=%5CDelta%20x)
    - **Compute ![\Delta \lambda](https://render.githubusercontent.com/render/math?math=%5CDelta%20%5Clambda)**
    - Update ![x = x + \Delta x](https://render.githubusercontent.com/render/math?math=x%20%3D%20x%20%2B%20%5CDelta%20x)
    - **Update ![\lambda = \lambda + \Delta \lambda](https://render.githubusercontent.com/render/math?math=%5Clambda%20%3D%20%5Clambda%20%2B%20%5CDelta%20%5Clambda)**
5. Update positions
6. Update velocities 
    - ![v_{n+1} = \frac{1}{\Delta t}(x_{n+1} - x_{n})](https://render.githubusercontent.com/render/math?math=v_%7Bn%2B1%7D%20%3D%20%5Cfrac%7B1%7D%7B%5CDelta%20t%7D(x_%7Bn%2B1%7D%20-%20x_%7Bn%7D))



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
