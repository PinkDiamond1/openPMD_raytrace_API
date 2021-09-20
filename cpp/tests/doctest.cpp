#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <openPMD_io.hh>
using namespace raytracing;

#include <doctest/doctest.h>
TEST_CASE("[Ray] Read-Write") {
	Ray myray;
	CHECK(myray.get_status() == kAlive);
	myray.set_position(1e-5, 2e-4, 2e2, 0.5);
	CHECK(myray.x() == doctest::Approx(5e-6));
	CHECK(myray.y() == doctest::Approx(1e-4));
	CHECK(myray.z() == doctest::Approx(1e2));

	float x,y,z;
	myray.get_position(&x, &y, &z);
	CHECK(x == doctest::Approx(5e-6));
	x =y =z =0;
	myray.get_position(x, y, z);

	///\todo add more checks for the raytracing::Ray class
}

TEST_CASE("[openPMD_io] Write") {

	std::string filename = "test_file.";
	int to_read =2;
	SUBCASE("JSON") { filename += "json";
	  SUBCASE("Read 5 reays"){ to_read=5;}
	  SUBCASE("Read 2 rays"){ to_read =2;}
	};
	SUBCASE("HDF5") { filename += "h5"; };

	raytracing::openPMD_io iol(filename, "test code");
	unsigned long long int n_rays_max = 11;
	unsigned int iter                 = 2;
	iol.init_write("2112", n_rays_max, iter);
	std::cout << "filename = " << filename << std::endl;

	iol.set_gravity_direction(0.33, 0.33, 0.33);
	raytracing::Ray myray;
	myray.set_position(1, 2, 3);
	//...
	myray.set_direction(1, 1, 1, 1. / sqrt(3));

	// append
	for (size_t i = 0; i < n_rays_max; ++i) { // don't write less than those set during init
		myray.set_position(i+1, i+2, i+3);
		iol.trace_write(myray);
	}

	iol.save_write();

	unsigned int nrepeat = 3;
	//nrays as returned by init_read should be used because
	//it might be < than what requested because there are not enough rays saved in the file
	auto nrays           = iol.init_read("2112", iter, to_read, nrepeat);

	// gravity direction
	float x = 3, y = 5, z = 7;
	iol.get_gravity_direction(&x, &y, &z);
	CHECK(x == doctest::Approx(0.33));
	CHECK(y == doctest::Approx(0.33));
	CHECK(z == doctest::Approx(0.33));

	
	for (unsigned int i = 0; i < nrays; ++i) {
	  for(size_t j  = 0; j < nrepeat; ++j) {
	    std::cout << "\n[DOCTEST] ray " << i << "/" << nrays << " to be returned\t ray: \n";
	    auto ray = iol.trace_read();
	    std::cout << ray << std::endl;
	    CHECK(ray.x() == doctest::Approx(i+1));
	    CHECK(ray.y() == doctest::Approx(i+2));
	    CHECK(ray.z() == doctest::Approx(i+3));
	  }
	}
	std::cout << "#### END ######" << std::endl;


	
}
