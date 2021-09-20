#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
namespace py = pybind11;

#include "config.h"
#include <openPMD_io.hh>
#include <ray.hh>
using namespace raytracing;

PYBIND11_MODULE(MODULE_NAME, m) {

	m.doc() = "pybind11 example plugin"; // optional module docstring
m.
	py::class_<openPMD_io>(m, "openPMD_io")
	        .def(py::init<const std::string&, const std::string, const std::string,
	                      const std::string, const std::string>())
	        .def("init_write", &openPMD_io::init_write)
	        .def("init_rays", &openPMD_io::init_rays)
	        .def("trace_write", &openPMD_io::trace_write)
	        .def("save_write", &openPMD_io::save_write)
	        .def("init_read", &openPMD_io::init_read)
	        .def("trace_read", &openPMD_io::trace_read) //
	        ;
	py::class_<Ray>(m, "Ray")
	        .def("x", &Ray::x)
	        .def("y", &Ray::y)
	        .def("z", &Ray::z)
	        ///\todo get position returning an ntuple
	        .def("dx", &Ray::dx)
	        .def("dy", &Ray::dy)
	        .def("dz", &Ray::dz)
	        ///\todo get_direction returning an ntuple
	        .def("sx", &Ray::sx)
	        .def("sy", &Ray::sy)
	        .def("sz", &Ray::sz)
	        ///\todo get_polarization returning an ntuple
	        .def("sPolAx", &Ray::sPolAx)
	        .def("sPolAy", &Ray::sPolAy)
	        .def("sPolAz", &Ray::sPolAz)
	        .def("sPolPh", &Ray::sPolPh)
	        ///\todo get_sPolarization returning an ntuple
	        .def("pPolAx", &Ray::pPolAx)
	        .def("pPolAy", &Ray::pPolAy)
	        .def("pPolAz", &Ray::pPolAz)
	        .def("pPolPh", &Ray::pPolPh)
	        ///\todo get_pPolarization returning an ntuple
	        //	        .def("weight", &Ray::weight)
	        //.def("id", &Ray::id)
	        //.def("status", &Ray::status)
	        // // Setters
	        .def("position", &Ray::get_position)
	        .def("direction", &Ray::get_direction)
	        .def("polarization", &Ray::get_polarization)
	        .def("sPolarization", &Ray::get_sPolarization)
	        .def("pPolarization", &Ray::get_pPolarization)
	        //
	        .def_property("wavelength", &Ray::get_wavelength, &Ray::set_wavelength)
	        .def_property("time", &Ray::get_time, &Ray::set_time)
	        .def_property("weight", &Ray::get_weight, &Ray::set_weight)
	        .def_property("id", &Ray::get_id, &Ray::set_id)
	        .def_property("status", &Ray::get_status, &Ray::set_status)

	        //
	        ;
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif

}

