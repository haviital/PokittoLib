cd Pokitto\POKITTO_LIBS\MicroPython

@rem clear all
del mpy\*.* /q
del frozen_mpy.c

@rem Compile py-files to mpy-files
@echo off
for %%a in (src_py\*) do (
    
    tools\mpy-cross -o mpy\%%~na.mpy -s %%~na.py src_py\%%~na.py
    @echo Compile to bytecode: %%~na.py 
    if ERRORLEVEL 1 ( 
        @echo *** ERROR: Cannot compile %%~na.py to bytecode!
        pause
    )
)
@echo on

@echo off
@rem Freeze mpy-files to one c-file.
echo|set /p dummyName="python tools/mpy-tool.py -mlongint-impl=none -f -q  genhdr/qstrdefs.preprocessed.h ">tmp_freeze.bat
for %%a in (mpy\*) do echo  | set /p dummyName="%%a ">>tmp_freeze.bat
echo|set /p dummyName=" > frozen_mpy.c">>tmp_freeze.bat
@echo on
call tmp_freeze.bat

@echo off
if ERRORLEVEL 1 ( 
    @echo *** ERROR: Cannot freeze bytecode to C-code!
    pause
)
@echo on



