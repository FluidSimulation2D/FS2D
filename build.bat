rmdir build /s /q
mkdir build
cd build

cmake ..
if errorlevel 1 goto error

cmake --build . --config debug
if errorlevel 1 goto error

cmake --build . --config release
if errorlevel 1 goto error

cd ..
goto OK

:error
@echo error

:OK

pause