@ECHO off
REM ****************************************************************************************************************
REM ** This script builds the debug version of OpenSSL for use by the ANSI C samples.
REM ** It requires that PERL be installed in the path and it must be run from a Visual Studio command line.
REM ****************************************************************************************************************
SETLOCAL

set PERLEXE=perl
set SRCDIR=%~dp0\src\openssl
set INSTALLDIR=%~dp0

IF NOT EXIST %SRCDIR% GOTO noOpenSSL

cd .\src\openssl

IF "%1"=="no-clean" GOTO noClean
IF EXIST %INSTALLDIR%\openssl rmdir /s /q %INSTALLDIR%\openssl

ECHO STEP 1) Configure OpenSSL for debug-VC-WIN32...
%PERLEXE% .\Configure debug-VC-WIN32 no-asm --prefix=%INSTALLDIR%\openssl

ECHO STEP 2) Creating Makefiles...
CALL .\ms\do_ms.bat

nmake -f .\ms\ntdll.mak clean
nmake -f .\ms\ntdll.mak vclean
:noClean

IF NOT EXIST %INSTALLDIR%\openssl MKDIR %INSTALLDIR%\openssl

ECHO STEP 3) Building OpenSSL
nmake -f .\ms\ntdll.mak

ECHO STEP 4) Install OpenSSL...
nmake -f .\ms\ntdll.mak install

ECHO *** ALL DONE ***
GOTO theEnd

:noOpenSSL
ECHO.
ECHO OpenSSL source not found. Please check the path.
ECHO Searched for: %SRCDIR%
GOTO theEnd

:theEnd
ENDLOCAL