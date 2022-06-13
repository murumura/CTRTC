# RayTracerChallenge
 Ray tracer implementation in C++ inspired by Jamis Buck's book "The Ray Tracer Challenge" 

I tring to make whole project to be able to run at compile time.

(This project is currently under development)

# Quick Start 
## Prerequisites
- CMake
- GTest
- Compliant C++20 compiler
  - The library is sytematically tested on following compilers 

Compiler | Version
---------|--------
GCC      | 12.1.0
clang    | 13.0.0

## Build and run test suite
Use the following commands from the project's top-most directory to run the test suite.
```bash
cd RayTracerChallenge
make test
make run-test
```
## Render Results (starting from chapter 5)
<details><summary>Chapter5</summary>
<p>

[![Sphere](results/sphere_on_wall.png)]

</p>
</details>

<details><summary>Chapter6</summary>
<p>

  ![Silhouette](results/SphereSilhouette.png)
  
</p>
</details>