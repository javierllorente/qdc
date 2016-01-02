::
:: Script for building a Windows installer using QtIFW
:: Copyright (C) 2015-2016 Javier Llorente <javier@opensuse.org>
:: 

@ECHO OFF

SET buildroot=c:\Users\Javier\Documents
SET builddir=%buildroot%\build-qdc-Desktop_Qt_5_5_0_MinGW_32bit-Release\src\release
SET pkgname=com.javierllorente
SET instpkgs=%buildroot%\qdc\installer\packages
SET pkgdata=%instpkgs%\%pkgname%\data
SET instconfig=%buildroot%\qdc\installer\config\config.xml
SET instfile=qdc.exe

SET qtdir=c:\Qt\Qt5.5.0
SET qtifwdir=C:\Qt\QtIFW2.0.1
SET wdqt=%qtdir%\5.5\mingw492_32\bin\windeployqt.exe
SET mingw32dlls=%qtdir%\Tools\mingw492_32\bin
SET qtifwbin=%qtifwdir%\bin

ECHO Running windeployqt...
%wdqt% --compiler-runtime %builddir%

cd %mingw32dlls%
ECHO Copying mingw32 dlls to build directory...
COPY "libgcc_s_dw2-1.dll" %builddir%
COPY "libstdc++-6.dll" %builddir%
COPY "libwinpthread-1.dll" %builddir%

ECHO Removing unneeded files...
cd %builddir%
del *.o *.cpp

ECHO Copying build to package data directory...
XCOPY /s /y %builddir% %pkgdata%

ECHO Building installer...
cd %buildroot%
%qtifwbin%\binarycreator.exe --offline-only -c %instconfig% -p %instpkgs% %instfile%
if %ERRORLEVEL% EQU 0 ECHO Done; %buildroot%\%instfile%
