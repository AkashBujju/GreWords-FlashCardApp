import os
import sys
import platform

opr_sys = platform.system()
command = ""
compiler_not_found_str = "Pass a compiler as argument - gcc(MinGW) | vc(VisualC++)"
os.chdir("src")

if opr_sys == 'Darwin':
    os.system("clear")
    print("Operating system is Darwin")
    filenames = "*.c"
    cc = "clang"
    args = " -Wall -O2"
    external_files = "../external/stb_image.c ../external/glad.c"
    includes = "-I../external/glfw/include -I../external/glad/include -I../external/freetype/include"
    libs_dirs = "-L../external/glfw/lib -L../external/freetype/lib"
    libs = "-lglfw3 -lfreetype -framework OpenGL -framework Cocoa -framework IOKit"
    output = "-o ../bin/main.out"
    command = cc + " " + args + " " + filenames + " " + external_files + " " + includes + " " + libs_dirs + " " + libs + " " + output
    os.system(command)
else:
    print("Operating system %s not supported" % opr_sys)
