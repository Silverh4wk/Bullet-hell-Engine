set "PROJECT_ROOT=I:\FYP"
set "BUILD_DIR=I:\build"

set "render=%PROJECT_ROOT%\src\engine\render\*.c"
set "objects=%PROJECT_ROOT%\src\objects\*.c"
set "io=%PROJECT_ROOT%\src\io\*.c"
set "input=%PROJECT_ROOT%\src\engine\input.c"
set "config=%PROJECT_ROOT%\src\engine\config.c"
set "ecs=%PROJECT_ROOT%\src\engine\ecs\*.c"
set "times=%PROJECT_ROOT%\src\engine\time.c"
set "physics=%PROJECT_ROOT%\src\engine\physics\*.c"
set "dataStructs=%PROJECT_ROOT%\src\engine\dataStructs\*.c"
set "MathEquations=%PROJECT_ROOT%\src\equations\*.c"
set "debug=%PROJECT_ROOT%\src\engine\Profiling\*.c "
set "files=%PROJECT_ROOT%\src\main.c %PROJECT_ROOT%\src\glad.c %PROJECT_ROOT%\src\helpers.c %PROJECT_ROOT%\src\engine\global.c"

set "API=%PROJECT_ROOT%\src\BHE\*.c"
set "libs=%PROJECT_ROOT%\lib\x64\SDL3.lib  %PROJECT_ROOT%\lib\x64\SDL2_mixer.lib  %PROJECT_ROOT%\lib\x64\SOIL.lib %PROJECT_ROOT%\lib\x64\freetype.lib  gdi32.lib glu32.lib opengl32.lib " 

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

pushd "%BUILD_DIR%"

cl  /Zi /MDd  /I  "%PROJECT_ROOT%\include" ^
   /DDEBUG_MODE %files% %render% %io% %config% %input% %dataStructs% %MathEquations% %physics% %times% %objects% %ecs% %API% %debug% ^
    /link %libs% /OUT:BHE.exe

popd
