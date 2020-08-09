rmdir /s /q build
mkdir build
cd build
cmake ../
cmake --build . --config release
cmake --build . --config debug
START Release/MLEngineTest.exe