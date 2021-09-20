c++ API for the openPMD ray trace extension  {#readme}
======================================================

This library provides a high-level API in C++ and Python to read and write openPMD files following the raytrace extension.
It uses the official openPMD-api at lower level.


Quantities
------------------------------
The list of variables stored in the openPMD file as defined by the extension is reported in the following table:

	
| Variable                       | Comment                                       | Units                    |
| ---------                      | ----------------                              | ------------------------ |
| x,y,z                          | position                                      | []                       |
| dx,dy,dz                       | direction                                     | (normalized velocity)    |
| sx,sy,sz                       | polarization of non-photons                   |                          |
| sPolAx, sPolAy, sPolAz, sPolPh | s-polarization amplitude and phase of photons |                          |
| pPolAx, pPolAy, pPolAz, pPolPh | p-polarization amplitude of photons           |                          |
| time                           | ray time w.r.t. ray generation                | [ms]                     |
| wavelength                     |                                               | [Ang]                    |
| weight                         | weight                                        |                          |
| id                             | unique ID                                     |                          |
| status                         | Alive or Dead                                 |                          |

	

Dependencies
------------------------------
This package depends on:
 - [**cmake**](https://cmake.org) (3.21.1):
 - [**openPMD-api**](https://www.openpmd.org/openPMD-api/) (0.12.1) used as base layer
 - [**pybind11**](https://github.com/pybind/pybind11) (2.4.3) [optional, ON by default] for the python binding 
 - [**doxygen**](https://doxygen.nl) (1.8.14) [optional]
 
 If you do not have the openPMD-api installed on your system, this package downloads, compiles and install it for you. But installing it separately is highly advisable.
 

Compilation and installation
------------------------------
```
git clone -b devel --depth 1 git@github.com:PaNOSC-ViNYL/openPMD_raytrace_API.git
cd openPMD_raytrace_API/

cmake -S . -B build/
cmake --build build/

# for the documentation
cmake --build build/ --target doc
```

For testing:
```
ctest --test-dir  build/openPMDraytrace-prefix/src/openPMDraytrace-build/tests/ --output-on-failure
```

More information about the usage can be found [here](@ref usage)


To do
------------------------------
 - [X] Make a dedicated set of tests for the C++ API in cpp/tests/
