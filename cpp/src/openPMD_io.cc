#include "openPMD_io.hh"
#include <iostream>
#include <openPMD/openPMD.hpp> // openPMD C++ API

#ifdef DEBUG
#define DEBUG_START(_METHOD_) \
	std::cout << "[DEBUG][openPMD_io][" << _METHOD_ << "][START] " << std::endl;
#define DEBUG_END(_METHOD_) \
	std::cout << "[DEBUG][openPMD_io][" << _METHOD_ << "][END] " << std::endl;
#define DEBUG_INFO(_METHOD_, _MSG_) \
	std::cout << "[DEBUG][openPMD_io][" << _METHOD_ << "][INFO] " << _MSG_ << std::endl;
#else
#define DEBUG_START(_METHOD)
#define DEBUG_END(_METHOD_)
#define DEBUG_INFO(_METHOD_, _MSG_)
#endif

#include <exception>
///\file
// using namespace raytracing;
// using raytracing::openPMD_io;
/** \brief defines the maximum number of rays that can be stored in memory before dumping to file
 * \todo The CHUNK_SIZE should not be hardcoded
 * it should also be optimized with tests
 */
namespace raytracing {
// constexpr size_t CHUNK_SIZE = 10000;
constexpr size_t CHUNK_SIZE = 3;
} // namespace raytracing

/** \todo use particlePatches ... but I don't understand if/how */

//------------------------------------------------------------

//------------------------------------------------------------
raytracing::openPMD_io::openPMD_io(const std::string& filename, std::string mc_code_name,
                                   std::string mc_code_version, std::string instrument_name,
                                   std::string name_current_component):
    //, int repeat):
    _name(filename),
    _mc_code_name(mc_code_name),
    _mc_code_version(mc_code_version),
    _instrument_name(instrument_name),
    _name_current_component(name_current_component),
    _i_repeat(0),
    _n_repeat(1),
    _offset({0}),
    _series(nullptr){};

//------------------------------------------------------------
void
raytracing::openPMD_io::init_ray_prop(std::string name, openPMD::Dataset& dataset, bool isScalar,
                                      std::map<openPMD::UnitDimension, double> const& dims,
                                      double unitSI) {
	auto rays = rays_pmd();

	rays[name].setUnitDimension(dims);

	if (isScalar) {
		rays[name][openPMD::RecordComponent::SCALAR].resetDataset(dataset);
		rays[name][openPMD::RecordComponent::SCALAR].setUnitSI(unitSI);
	} else {
		// now define the RECORD_COMPONENT
		for (auto dim : {"x", "y", "z"}) {
			rays[name][dim].resetDataset(dataset);
			rays[name][dim].setUnitSI(unitSI);
		}
	}
}

//------------------------------------------------------------
/**
 * \internal \remark
 * The openPMD ray tracing extension settings are set here for the rays' properties.\n
 * n_rays is the ncount of McStas, so the number of rays to be
 * simulated
 **/

void
raytracing::openPMD_io::init_rays(std::string particle_species, unsigned long long int n_rays,
                                  unsigned int iter) {
	_max_allowed_rays = n_rays;
	_nrays            = 0;

	DEBUG_START("INIT_RAYS")

	auto rays = rays_pmd(particle_species);

	// these are a single entry to mark some general properties of the particles
	openPMD::Dataset dataset_single_float =
	        openPMD::Dataset(openPMD::Datatype::FLOAT, openPMD::Extent{1});
	init_ray_prop("directionOfGravity", dataset_single_float, false);
	init_ray_prop("horizontalCoordinate", dataset_single_float, false);
	init_ray_prop("mass", dataset_single_float, true, {{openPMD::UnitDimension::M, 1.}});

	rays.setAttribute("speciesType", particle_species);
	rays.setAttribute("PDGID", particle_species);
	rays.setAttribute("numParticles", 0);

	openPMD::Dataset dataset_float =
	        openPMD::Dataset(openPMD::Datatype::FLOAT, openPMD::Extent{n_rays});
	openPMD::Dataset dataset_int =
	        openPMD::Dataset(openPMD::Datatype::INT, openPMD::Extent{n_rays});
	openPMD::Dataset dataset_ulongint =
	        openPMD::Dataset(openPMD::Datatype::ULONGLONG, openPMD::Extent{n_rays});

	init_ray_prop("position", dataset_float, false, {{openPMD::UnitDimension::L, 1.}}, 1e-2); //cm
	init_ray_prop("direction", dataset_float, false);

	init_ray_prop("nonPhotonPolarization", dataset_float, false);
	init_ray_prop("photonSPolarizationAmplitude", dataset_float, false);
	init_ray_prop("photonSPolarizationPhase", dataset_float, true);
	init_ray_prop("photonPPolarizationAmplitude", dataset_float, false);
	init_ray_prop("photonPPolarizationPhase", dataset_float, true);

	init_ray_prop("wavelength", dataset_float, true, {{openPMD::UnitDimension::L, 1}},
	              1); // 1.6021766e-13); // MeV ///\todo which units?
	init_ray_prop("weight", dataset_float, true);
	init_ray_prop("rayTime", dataset_float, true, {{openPMD::UnitDimension::T, 1.}}, 1e-3); //ms

	init_ray_prop("id", dataset_ulongint, true);
	init_ray_prop("particleStatus", dataset_int, true);

	DEBUG_END("INIT_RAYS")
}

void
raytracing::openPMD_io::init_write(std::string particle_species, unsigned long long int n_rays,
                                   unsigned int iter) {
	_iter                = iter;
	std::string filename = _name;
	// assign the global variable to keep track of it
	_series = std::unique_ptr<openPMD::Series>(
	        new openPMD::Series(filename, openPMD::Access::CREATE));

	_series->setAuthor("openPMD raytracing API");
	// latticeName: name of the instrument
	// latticeFile: name of the instrument file
	// branchIndex: unique index number assigned to the latice branch the[article is in.
	// (it should be per particle
	//

	DEBUG_INFO("init_write", "Filename: " << filename)

	auto i = iter_pmd(iter);
	_series->flush();

	//	openPMD::Record directionOfGravity;
	///\todo I don't how to add the directionOfGravity
	// float directionOfGravity[3] = {0.,0.,0.}; // this ensures that the attribute is
	// float type _series->setAttribute("directionOfGravity_X", directionOfGravity[0]);
	//_series->setAttribute("directionOfGravity_Y", directionOfGravity[1]);
	//_series->setAttribute("directionOfGravity_Z", directionOfGravity[2]);

	DEBUG_INFO("init_write", "Iter: " << _iter)

	// set the mccode, mccode_version, component name, instrument name

	init_rays(particle_species, n_rays, iter);
	// openPMD::Record mass = rays["mass"];
	// openPMD::RecordComponent mass_scalar = mass[openPMD::RecordComponent::SCALAR];

	//	mass_scalar.resetDataset(dataset);

	_series->flush();
	DEBUG_INFO("init_write", "flush done")
}

//------------------------------------------------------------
template <typename T>
void
save_write_single(openPMD::ParticleSpecies& rays, std::string field, std::string record,
                  raytracing::openPMD_io::Rays::Record<T>& rec, openPMD::Offset& offset,
                  openPMD::Extent& extent) {
	rays[field][record].storeChunk(openPMD::shareRaw(rec.vals()), offset, extent);
	rays[field][record].setAttribute("minValue", rec.min());
	rays[field][record].setAttribute("maxValue", rec.max());
}
//------------------------------------------------------------

void
raytracing::openPMD_io::save_write(void) {
	if (_rays.size() == 0) return;

	DEBUG_INFO("save_write",
	           "Number of saved rays: " << _rays.size() << "\t" << _rays._x.vals().size())

	auto rays = rays_pmd();
	// this check is here and not in the trace_write because I believe that loosing time for a
	// CHUNKSIZE simulating rays that are not store is much less frequent that.
	if (_nrays + _rays.size() > _max_allowed_rays)
		throw std::runtime_error("Maximum number of foreseen rays reached, stopping");

	// number of new rays being written
#ifdef DEBUG
	assert(_nrays == _offset[0]);
#endif
	_nrays += _rays.size();

	openPMD::Extent extent = {_rays.size()};

	save_write_single(rays, "position", "x", _rays._x, _offset, extent);
	save_write_single(rays, "position", "y", _rays._y, _offset, extent);
	save_write_single(rays, "position", "z", _rays._z, _offset, extent);

	save_write_single(rays, "direction", "x", _rays._dx, _offset, extent);
	save_write_single(rays, "direction", "y", _rays._dy, _offset, extent);
	save_write_single(rays, "direction", "z", _rays._dz, _offset, extent);

	save_write_single(rays, "nonPhotonPolarization", "x", _rays._sx, _offset, extent);
	save_write_single(rays, "nonPhotonPolarization", "y", _rays._sy, _offset, extent);
	save_write_single(rays, "nonPhotonPolarization", "z", _rays._sz, _offset, extent);

	save_write_single(rays, "photonSPolarizationAmplitude", "x", _rays._sPolAx, _offset,
	                  extent);
	save_write_single(rays, "photonSPolarizationAmplitude", "y", _rays._sPolAy, _offset,
	                  extent);
	save_write_single(rays, "photonSPolarizationAmplitude", "z", _rays._sPolAz, _offset,
	                  extent);
	save_write_single(rays, "photonSPolarizationPhase", openPMD::RecordComponent::SCALAR,
	                  _rays._sPolPh, _offset, extent);

	save_write_single(rays, "photonPPolarizationAmplitude", "x", _rays._pPolAx, _offset,
	                  extent);
	save_write_single(rays, "photonPPolarizationAmplitude", "y", _rays._pPolAy, _offset,
	                  extent);
	save_write_single(rays, "photonPPolarizationAmplitude", "z", _rays._pPolAz, _offset,
	                  extent);
	save_write_single(rays, "photonPPolarizationPhase", openPMD::RecordComponent::SCALAR,
	                  _rays._pPolPh, _offset, extent);

	save_write_single(rays, "rayTime", openPMD::RecordComponent::SCALAR, _rays._time, _offset,
	                  extent);
	save_write_single(rays, "wavelength", openPMD::RecordComponent::SCALAR, _rays._wavelength,
	                  _offset, extent);
	save_write_single(rays, "weight", openPMD::RecordComponent::SCALAR, _rays._weight, _offset,
	                  extent);

	save_write_single(rays, "id", openPMD::RecordComponent::SCALAR, _rays._id, _offset, extent);
	save_write_single(rays, "particleStatus", openPMD::RecordComponent::SCALAR, _rays._status,
	                  _offset, extent);

	rays.setAttribute("numParticles", _nrays);

	_series->flush();
	_rays.clear_chunk();

	for (size_t i = 0; i < extent.size(); ++i)
		_offset[i] += extent[i];
}

//------------------------------------------------------------
//------------------------------------------------------------
template <typename T>
void
read_single(openPMD::ParticleSpecies& rays, std::string field, std::string record,
            raytracing::openPMD_io::Rays::Record<T>& rec, openPMD::Offset& offset,
            openPMD::Extent& chunk_size) {

	auto data = rays[field][record];
	rec.vals().reserve(chunk_size[0]);
	rec.vals().resize(chunk_size[0]);
	data.loadChunk<T>(openPMD::shareRaw(rec.vals()), offset,
	                  chunk_size); // data.loadChunk<T>(offset, chunk_size);
	// if (field == "position") {
	// 	for (auto i = 0; i < chunk_size[0]; ++i) {
	// 		std::cout << "Data " << record << ": " << i << "\t" << rec.vals()[i] << "\t"
	// 		          << rec.vals().size() << std::endl;
	// 	}
	// }
	// rec.store((ddata.get()), chunk_size[0],                              //
	//           rays[field][record].getAttribute("minValue").get<float>(), //
	//           rays[field][record].getAttribute("maxValue").get<float>());
}
//------------------------------------------------------------

void
raytracing::openPMD_io::load_chunk(void) {

	_rays.clear(); // Necessary to set _read to zero
	auto rays = rays_pmd();
	DEBUG_START("load_chunk")

	unsigned long long int remaining = _nrays - _offset[0];
	openPMD::Extent chunk_size = {remaining > raytracing::CHUNK_SIZE ? raytracing::CHUNK_SIZE
	                                                                 : remaining};
	DEBUG_INFO("load_chunk",
	           _nrays << "\t" << _offset[0] << "\t" << remaining << "\t" << chunk_size[0])
	DEBUG_INFO("load_chunk",
	           "  Loading chunk of size " << chunk_size[0] << "; file contains " << _nrays)
	/* I don't understand....
	 * the data type info is embedded in the data... so why do we need to declare
	 * loadChunk<float>? it should overload to the right function... and return the correct
	 * datatype.
	 */
	read_single(rays, "position", "x", _rays._x, _offset, chunk_size);
	read_single(rays, "position", "y", _rays._y, _offset, chunk_size);
	read_single(rays, "position", "z", _rays._z, _offset, chunk_size);

	read_single(rays, "direction", "x", _rays._dx, _offset, chunk_size);
	read_single(rays, "direction", "y", _rays._dy, _offset, chunk_size);
	read_single(rays, "direction", "z", _rays._dz, _offset, chunk_size);

	read_single(rays, "nonPhotonPolarization", "x", _rays._sx, _offset, chunk_size);
	read_single(rays, "nonPhotonPolarization", "y", _rays._sy, _offset, chunk_size);
	read_single(rays, "nonPhotonPolarization", "z", _rays._sz, _offset, chunk_size);

	read_single(rays, "photonSPolarizationAmplitude", "x", _rays._sPolAx, _offset, chunk_size);
	read_single(rays, "photonSPolarizationAmplitude", "y", _rays._sPolAy, _offset, chunk_size);
	read_single(rays, "photonSPolarizationAmplitude", "z", _rays._sPolAz, _offset, chunk_size);
	read_single(rays, "photonSPolarizationPhase", openPMD::RecordComponent::SCALAR,
	            _rays._sPolPh, _offset, chunk_size);

	read_single(rays, "photonPPolarizationAmplitude", "x", _rays._pPolAx, _offset, chunk_size);
	read_single(rays, "photonPPolarizationAmplitude", "y", _rays._pPolAy, _offset, chunk_size);
	read_single(rays, "photonPPolarizationAmplitude", "z", _rays._pPolAz, _offset, chunk_size);
	read_single(rays, "photonPPolarizationPhase", openPMD::RecordComponent::SCALAR,
	            _rays._pPolPh, _offset, chunk_size);

	read_single(rays, "rayTime", openPMD::RecordComponent::SCALAR, _rays._time, _offset,
	            chunk_size);
	read_single(rays, "wavelength", openPMD::RecordComponent::SCALAR, _rays._wavelength,
	            _offset, chunk_size);
	read_single(rays, "weight", openPMD::RecordComponent::SCALAR, _rays._weight, _offset,
	            chunk_size);

	read_single(rays, "id", openPMD::RecordComponent::SCALAR, _rays._id, _offset, chunk_size);
	read_single(rays, "particleStatus", openPMD::RecordComponent::SCALAR, _rays._status,
	            _offset, chunk_size);

	_rays.size(chunk_size[0]);
	DEBUG_INFO("load_chunk", "Before flush")
	_series->flush();
	DEBUG_INFO("load_chunk", "After flush")

	//	std::cout << _rays._x[0] << "\t" << _rays._x[2] << std::endl;
	for (size_t i = 0; i < chunk_size.size(); ++i)
		_offset[i] += chunk_size[i];
	DEBUG_END("load_chunk")
}

unsigned long long int
raytracing::openPMD_io::init_read(std::string particle_species, unsigned int iter,
                                  unsigned long long int n_rays, unsigned int repeat) {

	_n_repeat            = repeat;
	_i_repeat            = 0;
	_iter                = iter;
	_offset              = {0};
	std::string filename = _name;

	// assign the global variable to keep track of it
	_series = std::unique_ptr<openPMD::Series>(new openPMD::Series(
	        filename,
	        openPMD::Access::READ_ONLY)); ///\todo the file access type was defined in
	                                      /// the openPMD_io constructor

	std::cout << "File information: " << filename << std::endl;
	if (_series->containsAttribute("author"))
		std::cout << "  Author  : " << _series->author() << std::endl;
	std::cout << "  Filename: " << filename << std::endl;
	std::cout << "  Number of iterations: " << _series->iterations.size() << std::endl;

	if (_series->iterations.size() == 0)
		std::cout << "  ERROR, no iterations found in openPMD series" << std::endl;

	///\todo check if the iteration exists

	// check the maximum number of rays stored
	_rays.clear(); // Necessary to set _read to zero

	auto i = iter_pmd(_iter);
	_series->flush();
	auto rays = rays_pmd(particle_species);
	_nrays    = rays.getAttribute("numParticles").get<unsigned long long int>();
	std::cout << "numParticles: " << _nrays << std::endl;
	if (n_rays > _nrays) {
		std::cerr << "[ERROR] Requested a number of rays that is not available in "
		             "the "
		             "current file"
		          << std::endl;
		throw std::runtime_error("ERROR"); ///\todo make it more meaningful
	}
	if (n_rays != 0) {
		std::cout << "[WARNING] Requested " << n_rays
		          << ", while available in file: " << _nrays << std::endl;
		_nrays = n_rays;
	}
	_series->flush();
	return _nrays;
}

void
raytracing::openPMD_io::trace_write(raytracing::Ray this_ray) {
	if (_rays.size() == CHUNK_SIZE) {

		DEBUG_INFO("trace_write",
		           "Reached CHUNK_SIZE:\toff=" << _offset[0] << "\tsize=" << _rays.size()
		                                       << "\tmax=" << _max_allowed_rays)

		save_write();
		_rays.clear_chunk(); // clear the vector content but not the min-max values
	}
	_rays.push(this_ray);
}

raytracing::Ray
raytracing::openPMD_io::trace_read(void) {
	///\todo reordering if conditions can improve performance
	DEBUG_INFO("trace_read", "---- i_repeat=" << _i_repeat << "\tn_repeat=" << _n_repeat
	                                          << "\tis_chunk_finished=" << std::boolalpha
	                                          << _rays.is_chunk_finished())
	if (_i_repeat++ == 0) {
		if (_rays.is_chunk_finished()) { load_chunk(); }
		_last_ray = _rays.pop();
	}
	if (_i_repeat >= _n_repeat) _i_repeat = 0;

	DEBUG_INFO("trace_read", "Ray: " << _last_ray)
	return _last_ray;
}

void
raytracing::openPMD_io::set_gravity_direction(float x, float y, float z) {
	auto rays              = rays_pmd();
	openPMD::Offset offset = {0};
	openPMD::Extent extent = {1};
	rays["directionOfGravity"]["x"].storeChunk(openPMD::shareRaw(&x), offset, extent);
	rays["directionOfGravity"]["y"].storeChunk(openPMD::shareRaw(&y), offset, extent);
	rays["directionOfGravity"]["z"].storeChunk(openPMD::shareRaw(&z), offset, extent);
	_series->flush();
}

void
raytracing::openPMD_io::get_gravity_direction(float* x, float* y, float* z) {
	auto rays = rays_pmd();
	auto xx   = rays["directionOfGravity"]["x"].loadChunk<float>();
	auto yy   = rays["directionOfGravity"]["y"].loadChunk<float>();
	auto zz   = rays["directionOfGravity"]["z"].loadChunk<float>();
	_series->flush();
	*x = xx.get()[0];
	*y = yy.get()[0];
	*z = zz.get()[0];
	//	std::cout << "[DEBUG] " << *x << std::endl;
}

void
raytracing::openPMD_io::get_horizontal_direction(float* x, float* y, float* z) {
	auto rays = rays_pmd();
	auto xx   = rays["horizontalCoordinate"]["x"].loadChunk<float>();
	auto yy   = rays["horizontalCoordinate"]["y"].loadChunk<float>();
	auto zz   = rays["horizontalCoordinate"]["z"].loadChunk<float>();
	_series->flush();
	*x = xx.get()[0];
	*y = yy.get()[0];
	*z = zz.get()[0];
}
