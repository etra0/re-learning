LIB=..\lib
GXX=g++
FLAGS=-Wall -g -municode

main.exe: process.o main.cpp Camera.cpp
	$(GXX) $(FLAGS) -I$(LIB) process.o Camera.cpp main.cpp -o main.exe

process.o: $(LIB)\header.h $(LIB)\process.h $(LIB)\process.cpp
	$(GXX) $(FLAGS) -c $(LIB)\process.cpp