LIB=..\lib
GXX=x86_64-w64-mingw32-g++
FLAGS=-Wall -g -municode -m64

main.exe: process.o main.cpp Camera.cpp my.res
	$(GXX) $(FLAGS) -I$(LIB) process.o Camera.cpp main.cpp my.res -o main.exe

release:
	$(GXX) $(FLAGS) -I$(LIB) -static-libgcc -static-libstdc++ process.o Camera.cpp main.cpp my.res -o main.exe

process.o: $(LIB)\header.h $(LIB)\process.h $(LIB)\process.cpp
	$(GXX) $(FLAGS) -c $(LIB)\process.cpp

my.res: my.rc
	windres my.rc -O coff -o my.res