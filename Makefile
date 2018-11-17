OBJ1 = syntax/*.o
OBJ2 = semantic/*.o
TARGET = parser

$(TARGET) :
	cd syntax && make;
	cd semantic && make;
	g++ -o $(TARGET) $(OBJ1) $(OBJ2) -lfl
        
clean :
	cd syntax && make clean;
	cd semantic && make clean;
	rm $(TARGET)