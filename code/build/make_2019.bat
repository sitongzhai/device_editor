@set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
@set QT_PATH=C:\Qt\Qt5.14.2\5.14.2\msvc2017_64\bin
@call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
@set THIRDLIB_PATH=..\..\..\thirdparty
@cd /d %~dp0\..
@set TCL_LIBRARY=%THIRDLIB_PATH%\Windows\tcl\tcl8.4
@set PATH=%THIRDLIB_PATH%\Windows\dll;%QT_PATH%;%PATH%
%QT_PATH%\qmake -spec win32-msvc .qmake.cache.pro
%QT_PATH%\qmake -recursive -spec win32-msvc -tp vc software.pro CONFIG+=console %*
"%VS_PATH%\Common7\IDE\devenv.exe" software.sln 

