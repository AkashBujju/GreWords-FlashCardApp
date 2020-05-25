echo off
cls
remove bin\*.exe
cls

echo Compiling
pushd src

gcc *.c -I..\external\glfw\include -I..\external\glad\include -I..\external\freetype\include ..\external\glad\src\glad.c ..\external\stb_image.c -L..\external\glfw\lib\win32 -L..\external\freetype\libs\mingw -lglfw3 -lfreetype -lgdi32 -lopengl32 -o ..\bin\game.exe

popd
