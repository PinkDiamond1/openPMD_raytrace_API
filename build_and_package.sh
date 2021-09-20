#!/bin/bash

BASE_BUILD_DIR=${1:-/dev/shm/openPMDraytrace_api/}
[ "$BASE_BUILD_DIR" != "${BASE_BUILD_DIR#/}" ] || BASE_BUILD_DIR=$PWD/$BASE_BUILD_DIR

packages_dir=${BASE_BUILD_DIR}/packages
mkdir ${packages_dir} -p
#mvn  install

function mvPack(){
	mv ${build_dir}/packaging/*.{rpm,deb} ${packages_dir}/
}


#--------------- API
#---------- C++
build_dir=$BASE_BUILD_DIR/openPMDraytrace-api-cpp/
cmake -S cpp -B $build_dir -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_PREFIX_PATH=/tmp/devel/ # with this it adds x86_64-linux-gnu to the path when running cpack
cmake --build $build_dir 
cpack --config $build_dir/CPackConfig.cmake -B $build_dir/packaging 
mvPack

#---------- Python
#build_dir=$BASE_BUILD_DIR/cameo-api-python/
#cmake -S cameo-api-python -B $build_dir -DCMAKE_PREFIX_PATH=$BASE_BUILD_DIR/cameo-api-cpp/ || exit 1
#cmake --build $build_dir || exit 1
#cpack --config $build_dir/CPackConfig.cmake -B $build_dir/packaging/ || exit 1
#mvPack
