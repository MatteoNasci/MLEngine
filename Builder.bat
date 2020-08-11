::If the first argument passed to the batch script is empty go directly to build process (Does not clean build folder)
IF %1.==. GOTO StartBuild
::This line was used to delete completely the build folder for a clean new build process. Commented since normally we don't want to rebuild everything
rmdir /s /q build
::This line was used together with previous line to prepare for new build process
mkdir build

:StartBuild
cd build
cmake ../
cmake --build . --config Release
cmake --build . --config Debug
START UnitTesting/Release/MLEngineUnitTesting.exe
START UnitTesting/Debug/MLEngineUnitTesting.exe
START Test/Release/MLEngineTest.exe