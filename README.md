# JellySimulation
A simple jelly simulation using Position Based Dynamics.

## Introduction
This project references to the famous [Position based dynamics (PBD)](https://matthias-research.github.io/pages/publications/posBasedDyn.pdf). However, traditional PBD stiffness is depend on time step. Therefore, in this project, I implement [XPBD](http://mmacklin.com/xpbd.pdf) to resolve this problem.

## Result 
**PBD**

![](https://i.imgur.com/3OfJ97m.gif)

**XPBD**

![](https://i.imgur.com/CgO8adS.gif)

* stiffness setting 
    * PBD stiffness: 0.025
        * The value of PBD stiffness is in [0,1]
    * XPBD compliance: 0.0005    
    * **Notice**: PBD and XPBD stiffness is very hard to compare (not in the same unit). The stiffness of XPBD can be set using real-world value.
