clear
cd bin
rm gre_cards.exe gre_cards.out
clear

cd ..
cd src

echo "Compiling"
gcc-9 *.c -I../external/glfw/include -I../external/glad/include -I../external/freetype/include ../external/glad/src/glad.c ../external/stb_image.c -L../external/glfw/lib/linux/ -L../external/freetype/lib -lGLU -lGL -lglfw3 -lfreetype -lpng -lz -ldl -lX11 -lpthread -lm -o ../bin/gre_cards.out
cd ..
