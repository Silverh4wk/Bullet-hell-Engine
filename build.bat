set "PROJECT_ROOT=I:\FYP"
set "BUILD_DIR=I:\build"

set "render=%PROJECT_ROOT%\src\engine\render\render.cpp %PROJECT_ROOT%\src\engine\render\render_init.cpp %PROJECT_ROOT%\src\engine\render\render_util.cpp"
set "objects=%PROJECT_ROOT%\src\objects\shapes.cpp"
set "io=%PROJECT_ROOT%\src\io\io.cpp"
set "input=%PROJECT_ROOT%\src\engine\input.cpp"
set "config=%PROJECT_ROOT%\src\engine\config.cpp"
set "times=%PROJECT_ROOT%\src\engine\time.cpp"
set "physics=%PROJECT_ROOT%\src\engine\physics\physics.cpp"
set "dataStructs=%PROJECT_ROOT%\src\engine\dataStructs\array_list.cpp"
set "files=%PROJECT_ROOT%\src\main.cpp %PROJECT_ROOT%\src\glad.c %PROJECT_ROOT%\src\helpers.cpp %PROJECT_ROOT%\src\engine\global.cpp"

set "libs=%PROJECT_ROOT%\lib\x64\SDL3.lib  %PROJECT_ROOT%\lib\x64\SDL2_mixer.lib"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

pushd "%BUILD_DIR%"

cl /std:c++20 /Zi /I "%PROJECT_ROOT%\include" ^
    %files% %render% %io% %config% %input% %dataStructs% %physics% %times% %objects%  ^
    /link %libs% /OUT:BHE.exe

popd
