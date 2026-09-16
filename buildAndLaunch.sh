!/bin/bash

scrDir=$(pwd)
ProjectName=$(grep "project" CMakeLists.txt | sed 's/project(//' | sed 's/.$//')

if [ $1 = "--remove-build" ]; then
	rm -rf build/plugin
	echo "removed plugin build"
fi

if [ $2 = "--all" ]; then
	rm -rf build/
	echo "removed whole build directory"
fi

# build project
echo "configuring project with CMake..."
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1
echo "building project..."
cmake --build build

./build/hello
