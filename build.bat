cd script

python --version 2>NUL
if errorlevel 1 goto pythonError

pip install -r requirements.txt
if errorlevel 1 goto pythonError

python download_script.py
if errorlevel 1 goto pythonError

cd ..

rmdir build /s /q
mkdir build
cd build

cmake ..
if errorlevel 1 goto cmakeError

cmake --build . --config debug
if errorlevel 1 goto cmakeError

cmake --build . --config release
if errorlevel 1 goto cmakeError

cd ..
goto OK

:pythonError
@echo Error while downloading SFML

:cmakeError
@echo Error while using cmake

:OK

pause