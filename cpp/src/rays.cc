//#include "rays.hh"
#include "openPMD_io.hh"
#include <array>
#include <cmath>
///\file

using raytracing::openPMD_io;
using raytracing::Ray;
// using raytracing::openPMD_io::Rays;
#ifndef V2SE
#define VS2E 5.22703725e-6 /* Convert (v[m/s])**2 to E[meV] */
#endif
namespace raytracing {
std::ostream&
operator<<(std::ostream& os, const Ray& ray) {
	return os << "(" << ray.x() << ", " << ray.y() << "\t" << ray.z() << ")";
}
} // namespace raytracing

openPMD_io::Rays::Rays(): _size(0), _read(0) {}

//------------------------------
void
openPMD_io::Rays::push(const Ray& this_ray) {
	_x.push_back(this_ray.x());
	_y.push_back(this_ray.y());
	_z.push_back(this_ray.z());

	_dx.push_back(this_ray.dx());
	_dy.push_back(this_ray.dy());
	_dz.push_back(this_ray.dz());

	_sx.push_back(this_ray.sx());
	_sy.push_back(this_ray.sy());
	_sz.push_back(this_ray.sz());

	_sPolAx.push_back(this_ray.sPolAx());
	_sPolAy.push_back(this_ray.sPolAy());
	_sPolAz.push_back(this_ray.sPolAz());
	_sPolPh.push_back(this_ray.sPolPh());

	_pPolAx.push_back(this_ray.pPolAx());
	_pPolAy.push_back(this_ray.pPolAy());
	_pPolAz.push_back(this_ray.pPolAz());
	_pPolPh.push_back(this_ray.pPolPh());

	_wavelength.push_back(this_ray.get_wavelength());
	_time.push_back(this_ray.get_time());
	_weight.push_back(this_ray.get_weight());

	_id.push_back(this_ray.get_id());
	_status.push_back(this_ray.get_status());
	++_size;
};

//------------------------------
Ray
openPMD_io::Rays::pop(bool next) {

	Ray r;
	r.set_position(_x[_read], _y[_read], _z[_read]);
	r.set_direction(_dx[_read], _dy[_read], _dz[_read]);

	r.set_polarization(_sx[_read], _sy[_read], _sz[_read]);

	r.set_sPolarization(_sPolAx[_read], _sPolAy[_read], _sPolAz[_read], _sPolPh[_read]);
	r.set_pPolarization(_pPolAx[_read], _pPolAy[_read], _pPolAz[_read], _pPolPh[_read]);

	r.set_wavelength(_wavelength[_read]);
	r.set_time(_time[_read]);
	r.set_weight(_weight[_read]);

	r.set_id(_id[_read]);
	r.set_status(_status[_read]);

	if (next) ++_read;
	return r;
}

