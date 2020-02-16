LIB=..\lib
GXX=g++
FLAGS=-Wall -g -municode

main.exe: process.o main.cpp Camera.cpp my.res
	$(GXX) $(FLAGS) -I$(LIB) process.o Camera.cpp main.cpp my.res -o yakuza0-freecamera.exe

process.o: $(LIB)\header.h $(LIB)\process.h $(LIB)\process.cpp
	$(GXX) $(FLAGS) -c $(LIB)\process.cpp

my.res: my.rc
	windres my.rc -O coff -o my.res