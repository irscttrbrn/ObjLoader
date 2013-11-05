CC = gcc
FLAGS = -Wall -std=c99
OBJ = ObjLoader.o
TARGET = ObjLoaderTest

$(TARGET) : Main.c $(OBJ)
	$(CC) -o $(TARGET) Main.c $(OBJ) $(FLAGS) 

%.o: %.cpp
	$(CC) -c $< $(FLAGS)