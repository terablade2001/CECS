rmdir build /S /Q
mkdir build && cd build
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release ..
cd ..