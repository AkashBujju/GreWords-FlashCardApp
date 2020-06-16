import os
import sys
import platform

opr_sys = platform.system()
command = ""
compiler_not_found_str = "Pass a compiler as argument - gcc(MinGW) | vc(VisualC++)"
os.chdir("src")

if len(sys.argv) != 2:
    print("ScriptError: \n ----- " + compiler_not_found_str + "\n")
    sys.exit()

if opr_sys == 'Darwin':
    os.system("clear")
    print("Operating system is Darwin")
    filenames = "*.c"
    cc = "clang"
    args = " -Wall "
    external_files = "../external/stb_image.c ../external/glad.c"
    includes = "-I../external/glfw/include -I../external/glad/include -I../external/freetype/include"
    libs_dirs = "-L../external/glfw/lib -L../external/freetype/lib"
    libs = "-lglfw3 -lfreetype -framework OpenGL -framework Cocoa -framework IOKit"
    output = "-o ../bin/main.out"
    command = cc + " " + args + " " + filenames + " " + external_files + " " + includes + " " + libs_dirs + " " + libs + " " + output
    os.system(command)
elif opr_sys == 'Windows':
    if sys.argv[1] == 'gcc':
        os.system("cls")
        print("Operating system is Windows")
        cc = "gcc"
        filenames = "*.c"
        external_files = "..\\external\\windows\\stb_image.c ..\\external\\windows\\glad.c"
        includes = "-I..\\external\\windows\\glfw\\mingw\\include -I..\\external\\windows\\glad\\include -I..\\external\\windows\\freetype\\include"
        libs_dirs = "-L..\\external\\windows\\glfw\\mingw\\lib -L..\\external\\windows\\freetype\\libs\\mingw"
        libs = "-lglfw3 -lfreetype"
        #args = "-O2 -ffast-math"
        args = "-Wreturn-type -Wall"
        output = "-o ..\\bin\\gcc\\main.exe"
        command = cc + " " + args + " " + filenames + " " + external_files + " " + includes + " " + libs_dirs + " " + libs + " " + output
        os.system(command)
    elif sys.argv[1] == 'vc':
        cc = "cl"
        filenames = "*.c"
        external_files = "..\\external\\windows\\stb_image.c ..\\external\\windows\\glad.c"
        includes = "-I..\\external\\windows\\glfw\\vc\\x86\\include -I..\\external\\windows\\glad\\include -I..\\external\\windows\\freetype\\include"
        libs_dirs = "/link /LIBPATH:..\\external\\windows\\glfw\\vc\\x86\\lib /LIBPATH:..\\external\\windows\\freetype\\libs\\vc\\x86"
        libs = "gdi32.lib opengl32.lib kernel32.lib user32.lib shell32.lib glfw3.lib freetype.lib"
        args = "/MD /D_CRT_SECURE_NO_WARNINGS"
        output = "/out:..\\bin\\vc\\main.exe"
        command = cc + " " + args + " " + filenames + " " + external_files + " " + includes + " " + libs_dirs + " " + libs + " " + output
        os.system(command)
        # includes = "/I..\\external\\glfw\\vc\\x64\\include /I..\\external\\glad\\include /I..\\external\\easy_profiler\\include"
        # libs_dirs = "/link /LIBPATH:..\\external\\glfw\\vc\\x64\\lib /LIBPATH:..\\external\\easy_profiler\\lib"
        # args = "/DBUILD_WITH_EASY_PROFILER /MD"
        os.system("del *.obj")
    else:
        os.system("cls")
        print("\n-> Compiler '%s' not found\n" % sys.argv[1])
        print("-> " + compiler_not_found_str + "\n")
else:
    print("Operating system %s not supported" % opr_sys)
