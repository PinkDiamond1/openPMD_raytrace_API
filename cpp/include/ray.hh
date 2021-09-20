#ifndef RAY_CLASS_HH
#define RAY_CLASS_HH

#include <cmath>
#include <ostream>

namespace raytracing {
constexpr double V2W = 3.956034e-07; // m^2/
// enum particleStatus_t : int { kDead = 0, kAlive = 1 };

/** \typedef particleStatus_t
 * particle statues are defined by the openPMD extension as integers with
 *  - 1: active/alive particle
 *  - other: any other value is used for "dead" particles, the effective value and meaining
 * depends on the simulation software used
 *
 * In the ray tracing extension only raytracing::kDead and raytracing::kAlive values are
 * used
 */
typedef int particleStatus_t;
constexpr int kDead  = 0; ///< particleStatus_t: dead ray
constexpr int kAlive = 1; ///< alive ray

/** \class Ray
 * \brief Generic ray, containing all the ray information being stored by the API into the openPMD
 * file
 *
 * This is base class provides several methods to set and manipulate the ray information in the
 * appropriate way.
 *
 * Other specific ray classes should inherit from this in order to provide more
 * convenient methods for the user. Derived classes can be created from this and used by the users
 * in their programs.
 *
 * Unit conversions should be applied here, so that the Rays class stores information in a coherent
 * way. There is a small inefficiency if rays are used multiple times, but the time spent in unit
 * conversions is anyway negligible w.r.t. the simulation time.
 */
class Ray {
public:
#define DIM_INIT \
	{ 0, 0, 0 }
#define PHASEDIM_INIT \
	{ 0, 0, 0, 0 }

	static const size_t DIM      = 3;
	static const size_t X        = 0;
	static const size_t Y        = 1;
	static const size_t Z        = 2;
	static const size_t PHASEDIM = DIM + 1;
	static const size_t PHASE    = 3;

private:
	float _position[DIM]           = DIM_INIT;
	float _direction[DIM]          = DIM_INIT;
	float _polarization[DIM]       = DIM_INIT; // for non-photons
	float _sPolarization[PHASEDIM] = PHASEDIM_INIT;
	float _pPolarization[PHASEDIM] = PHASEDIM_INIT;
	float _wavelength              = 0;
	float _time                    = 0;
	float _weight                  = 1;
	unsigned long long int _id     = 0;
	// particleStatus_t _status;
	particleStatus_t _status = kAlive;

public:
	// Ray(){};

	/// \name Getters
	///@{

	/// \name Get position
	///@{
	float x() const { return _position[X]; };
	float y() const { return _position[Y]; };
	float z() const { return _position[Z]; };
  	void get_position(float* xx, float* yy, float* zz) const {
		(*xx) = x();
		(*yy) = y();
		(*zz) = z();
	}
	void get_position(double* xx, double* yy, double* zz) const {
		(*xx) = x();
		(*yy) = y();
		(*zz) = z();
	}
	///\todo add methods using reference instead of pointers for C++ users
	void get_position(float& xx, float& yy, float& zz) const {
		xx = x();
		yy = y();
		zz = z();
	}
	//@}

	/// \name Get direction
	///@{
	float dx() const { return _direction[X]; };
	float dy() const { return _direction[Y]; };
	float dz() const { return _direction[Z]; };
	void get_direction(float* x, float* y, float* z, double scale) const {
		*x = dx() * scale;
		*y = dy() * scale;
		*z = dz() * scale;
	}
	void get_direction(double* x, double* y, double* z, double scale) const {
		*x = dx() * scale;
		*y = dy() * scale;
		*z = dz() * scale;
	}
	///@}

	/// \name Get polarization for non-photons
	///@{
	float sx() const { return _polarization[X]; };
	float sy() const { return _polarization[Y]; };
	float sz() const { return _polarization[Z]; };
	void get_polarization(float* x, float* y, float* z) const {
		*x = sx();
		*y = sy();
		*z = sz();
	}
	void get_polarization(double* x, double* y, double* z) const {
		*x = sx();
		*y = sy();
		*z = sz();
	}
	///@}

	/// \name Get s-polarization amplituded and phase for photons
	///@{
	float sPolAx() const { return _sPolarization[X]; };
	float sPolAy() const { return _sPolarization[Y]; };
	float sPolAz() const { return _sPolarization[Z]; };
	float sPolPh() const { return _sPolarization[PHASE]; };
	void get_sPolarizationAmplitude(float* x, float* y, float* z) const {
		*x = sPolAx();
		*y = sPolAy();
		*z = sPolAz();
	}
	void get_sPolarization(float* x, float* y, float* z, float* phase) const {
		get_sPolarizationAmplitude(x,y,z);
		*phase = sPolPh();
	}

	///@}
	/// \name Get p-polarization amplitude and phase for photons
	float pPolAx() const { return _pPolarization[X]; };
	float pPolAy() const { return _pPolarization[Y]; };
	float pPolAz() const { return _pPolarization[Z]; };
	float pPolPh() const { return _pPolarization[PHASE]; };
	void get_pPolarizationAmplitude(float* x, float* y, float* z) const {
		*x = pPolAx();
		*y = pPolAy();
		*z = pPolAz();
	}

	void get_pPolarization(float* x, float* y, float* z, float* phase) const {
		get_pPolarizationAmplitude(x,y,z);
		*phase = pPolPh();
	}

	///@}

	/** \name Get wavelength, ray time, weight, id and status
	 * Allowed status values are raytracing::kAlive and raytracing::kDead
	 */
	///@{
	float get_wavelength() const { return _wavelength; };
	float get_time() const { return _time; };
	float get_weight() const { return _weight; };
	unsigned long long int get_id(void) const { return _id; };
	/** \brief return the particle status
	 * \return raytracing::kAlive or raytracing::kDead
	 */
	inline particleStatus_t get_status(void) const { return _status; };

	///@}
	///@}

	/// \name Setters
	/// @{

	/// \brief scale and set the position
	void set_position(double x, double y, double z, double scale = 1) {
		_position[X] = x * scale;
		_position[Y] = y * scale;
		_position[Z] = z * scale;
	}

	/// \brief scale and set the direction
	void set_direction(double x, double y, double z, double scale = 1) {
		_direction[X] = x * scale;
		_direction[Y] = y * scale;
		_direction[Z] = z * scale;
	}

	/// \brief scale and set the direction from the velocity
	void set_velocity(double x, double y, double z) {
		double abs_v = sqrt(x * x + y * y + z * z);
		set_direction(x, y, z, 1. / abs_v);
	}

	/// \brief scale and set polarization for non-photons  \todo to implement
	void set_polarization(double x, double y, double z, double scale = 1) {
		_polarization[X] = x * scale;
		_polarization[Y] = y * scale;
		_polarization[Z] = z * scale;
	}
	/// \brief scale and set s-polarization for photons
	void set_sPolarization(double x, double y, double z, double phase, double scale = 1){
		_sPolarization[X] = x * scale;
		_sPolarization[Y] = y * scale;
		_sPolarization[Z] = z * scale;
		_sPolarization[PHASE] = phase;
	};
	/// \brief scale and set p-polarization for photons  \todo to implement
	void set_pPolarization(double x, double y, double z, double phase, double scale = 1){
		_pPolarization[X] = x * scale;
		_pPolarization[Y] = y * scale;
		_pPolarization[Z] = z * scale;
		_pPolarization[PHASE] = phase;
	};
	/// \brief set wavelength
	void set_wavelength(double w) { _wavelength = w; };
	/// \brief set time
	void set_time(double t) { _time = t; };
	/// \brief set weight
	void set_weight(double w) { _weight = w; };
	void set_id(unsigned long long int idv) { _id = idv; };
	void set_status(particleStatus_t s) { _status = s; };

	///@}
public:
	friend std::ostream& operator<<(std::ostream& os, const Ray& ray);
};

#ifdef DD

/** \class photon
 * \brief helper class for photons
 */
class photon : public Ray {};

/** \class neutron
 *  \brief helper class for neutrons
 */
class neutron : public Ray {};

#endif

	
class mcstas_neutron : public Ray {
public:
	void set_position(double x, double y, double z){
		Ray::set_position(x, y, z);
	};
	void set_velocity(double x, double y, double z){
		double abs_v = sqrt(x * x + y * y + z * z);
		set_direction(x, y, z, 1. / abs_v);
		set_wavelength(V2W/abs_v);
	}
	void get_velocity(float *x, float *y, float* z){
		double abs_v = V2W/get_wavelength();
		get_direction(x, y, z, abs_v);
	}
};

} // namespace raytracing

#endif





