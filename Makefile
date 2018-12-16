OBJ1 = syntax/*.o
OBJ2 = semantic/*.o
OBJ3 = IR/*.o
TARGET = parser

$(TARGET) :
	cd syntax && make;
	cd semantic && make;
	cd IR && make;
	g++ -o $(TARGET) $(OBJ1) $(OBJ2) $(OBJ3)  -lfl
        
clean :
	cd syntax && make clean;
	cd semantic && make clean;
	cd IR && make clean;
	rm $(TARGET)