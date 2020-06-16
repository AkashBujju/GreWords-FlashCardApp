clear
cd bin
rm gre_cards.exe gre_cards.out
clear

cd ..
cd src

echo "Compiling"
gcc-9 *.c -I../external/glfw/include -I../external/glad/include -I../external/freetype/include ../external/glad/src/glad.c ../external/stb_image.c -L../external/glfw/lib/darwin -L../external/freetype/libs/darwin -lglfw3 -lfreetype -framework OpenGL -framework Cocoa -framework IOKit -o ../bin/gre_cards.out
cd ..