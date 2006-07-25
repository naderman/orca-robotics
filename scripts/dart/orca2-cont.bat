REM @ECHO OFF

REM Set The Environment Variables
REM =============================
CALL "C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\vsvars32.bat"
SET CVS_RSH="C:\Program Files\PuTTY\plink.exe"

SET PATH=C:\Program Files\TortoiseCVS\cvs.exe;C:\Program Files\CMake24\bin;C:\Ice-3.1.0\bin;%PATH%

REM Run the CTest to do nightly updates
REM ===================================
ctest "-S" "C:\ctests\orca\orca2-nightly\orca2-nightly-win32-vc80.cmake" "-VV" >> ctest_log.txt 2>&1

REM uncomment to wait for user input...
REM choice
