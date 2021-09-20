#include <openPMD_io.hh>

int
main(int argc, char* argv[]) {

	std::string filename = "test_file.json";
	raytracing::openPMD_io iol(filename, "test code");

	iol.init_write(  "2112", 8,raytracing::AUTO, 2);
	std::cout << "filename = " << filename << std::endl;
	raytracing::Ray myray;
	myray.set_position(1, 2, 3);
	//...
	myray.set_direction(1, 1, 1, 1. / sqrt(3));

	// append
	for(size_t i =0; i < 10; ++i){
		iol.trace_write(myray);
	}

	std::cout << "trace write done" << std::endl;
	iol.save_write();
	std::cout << "save write done" << std::endl;
	return 0;
}
