rmdir /s /q build
mkdir build
cd build
cmake ../
cmake --build . --config Release
cmake --build . --config Debug
START UnitTesting/Release/MLEngineUnitTesting.exe
START UnitTesting/Debug/MLEngineUnitTesting.exe
START Test/Release/MLEngineTest.exe