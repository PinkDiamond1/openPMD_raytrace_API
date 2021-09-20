#include <openPMD_io.hh>

int main(void){
	std::string filename = "test_file.json";
	raytracing::openPMD_io io(filename, "test code");
	io.init_read(raytracing::HDF5);
	auto ray = io.trace_read();
	
	return 0;
}
