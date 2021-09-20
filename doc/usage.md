Usage {#usage}
==============================

This library is based on the following public classes under the \ref raytracing namespace:
 - [Ray](@ref raytracing::Ray)
 - [openPMD_io](\ref raytracing::openPMD_io)
 
Here's a schematic of the I/O logic. 

\dot
	   digraph example {
	       rankdir="LR";
	       node [shape=record, fontname=Helvetica, fontsize=10];
		   u    [label="User's program" shape=ellipse];
		   Ray  [label="Ray object" shape=rectangle, URL="\ref raytracing::Ray"];
		   u -> Ray [label="create and fill"];
		   openPMD_io [];
		   Ray -> openPMD_io [label="trace_write", URL="\ref raytracing::openPMD_io::trace_write"];
		   openPMD_io -> pmd [label="save_write", URL="\ref raytracing::openPMD_io::save_write"];
		   pmd  [ label="openPMD file", shape=ellipse];
		   pmd -> openPMD_io [label="load_chunk (internal)"];
		   openPMD_io -> Ray [label="trace_read", URL="\ref raytracing::openPMD_io::trace_read"];
		   Ray -> u [label="get values"];
	   }
\enddot

**NB**: image with hyperlinks, you can click on the boxes to jump directly to the class definition.


## Writing
 -# Declare an openPMD_io object with a filename used for writing (@ref raytracing::openPMD_io())
 -# initialize the file (@ref raytracing::openPMD_io::init_write) 
 -# a Ray object should be constructed and filled with all the relevant information for the ray being traced
 -# the ray is then queued for writing with the @ref raytracing::openPMD_io::trace_write
 -# the queued rays are written to file in bunches of CHUNKSIZE and finally when @ref raytracing::openPMD_io::save_write() is called

In the following a very simple example can be found.
\include test_write.cpp

The Ray class is providing all the conversion/utility operations on the quantities stored in the openPMD file according to the RAYTRACE extension


## Reading

Reading from an openPMD file follows the same logic as the reading, with symmetricly defined methods of the openPMD_io class.
\include test_read.cpp


## Unit conversion

The units of the quantities stored in the openPMD file are pre-defined by the extension and not customizable by the user.

For a more user-friendly user interface, the user should create it's own version of the Ray object (either in C++ or in Python), inheriting from the original Ray object.
The conversion should be done in the overloaded methods.
This should highly simplify the user code.

Otherwise, the user can also use the bare Ray class with the "scale" optional argument for the different methods to scale the units into the pre-defined ones.

## Todo
 - [NO] Units conversion!!!!
 - [X] Setter and getter for gravity direction
 - [X] Setter and getter for horizontalCoordinate
 - [X] Setter and getter for numParticles
 - [X] Interal usage of numParticles
 - [X] Binding of wavelength, time, weight, id, status getters
 - [ ] Test the python binding
 - [ ] Cmake install for the python binding
 - [ ] PyPi package
 - [X] ostream friend for the ray class to make easier to debug
 - [X] Repeating ray multiple times if requested when reading the openPMD file
 - [X] Writing in chunks
 - [X] Throwing runtime_error when over the maximum foreseen number of rays
 - [X] Add doctest package for C++ unit tests



## Examples
Examples can be found 


\example test_write.cpp



