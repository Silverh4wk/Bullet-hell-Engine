set "PROJECT_ROOT=I:\FYP"
set "BUILD_DIR=I:\build"

set "render=%PROJECT_ROOT%\src\engine\render\render.c %PROJECT_ROOT%\src\engine\render\render_init.c %PROJECT_ROOT%\src\engine\render\render_util.c %PROJECT_ROOT%\src\engine\render\camera.c"
set "objects=%PROJECT_ROOT%\src\objects\shapes.c"
set "io=%PROJECT_ROOT%\src\io\io.c"
set "input=%PROJECT_ROOT%\src\engine\input.c"
set "config=%PROJECT_ROOT%\src\engine\config.c"
set "ecs=%PROJECT_ROOT%\src\engine\ecs\ecs.c"
set "times=%PROJECT_ROOT%\src\engine\time.c"
set "physics=%PROJECT_ROOT%\src\engine\physics\physics.c %PROJECT_ROOT%\src\engine\physics\spatial_hashing.c"
set "dataStructs=%PROJECT_ROOT%\src\engine\dataStructs\array_list.c %PROJECT_ROOT%\src\engine\dataStructs\hash_table.c %PROJECT_ROOT%\src\engine\dataStructs\pool_allocator.c %PROJECT_ROOT%\src\engine\dataStructs\linked_list.c %PROJECT_ROOT%\src\engine\dataStructs\Quad_trees.c %PROJECT_ROOT%/src/engine/dataStructs/dataStructs.c"
set "debug=%PROJECT_ROOT%\src\engine\Profiling\FPS_counter.c"
set "pattern=%PROJECT_ROOT%\src\BHE\pattern.c"
set "files=%PROJECT_ROOT%\src\main.c %PROJECT_ROOT%\src\glad.c %PROJECT_ROOT%\src\helpers.c %PROJECT_ROOT%\src\engine\global.c"

set "API=%PROJECT_ROOT%\src\BHE\engineAPI.c"
set "libs=%PROJECT_ROOT%\lib\x64\SDL3.lib  %PROJECT_ROOT%\lib\x64\SDL2_mixer.lib  %PROJECT_ROOT%\lib\x64\SOIL.lib  gdi32.lib glu32.lib opengl32.lib " 

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

pushd "%BUILD_DIR%"

cl  /Zi /MDd  /I  "%PROJECT_ROOT%\include" ^
    %files% %render% %io% %config% %input% %dataStructs% %physics% %times% %objects% %ecs% %API% %debug% %pattern% ^
    /link %libs% /OUT:BHE.exe

popd
