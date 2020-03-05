LIB=..\lib
GXX=x86_64-w64-mingw32-g++
FLAGS=-Wall -g -municode -m64

main.exe: process.o main.cpp Camera.cpp icon/my.res
	$(GXX) $(FLAGS) -I$(LIB) process.o Camera.cpp main.cpp icon/my.res -o main.exe

release:
	$(GXX) $(FLAGS) -I$(LIB) -static-libgcc -static-libstdc++ process.o Camera.cpp main.cpp icon/my.res -o yakuza0-freecamera.exe

kiwami:
	$(GXX) $(FLAGS) -I$(LIB) -DKIWAMI -static-libgcc -static-libstdc++ process.o Camera.cpp main.cpp icon/my.res -o yakuzakiwami-freecamera.exe

process.o: $(LIB)\header.h $(LIB)\process.h $(LIB)\process.cpp
	$(GXX) $(FLAGS) -c $(LIB)\process.cpp

icon/my.res: icon/my.rc
	windres icon/my.rc -O coff -o icon/my.res