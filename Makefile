OBJ1 = syntax/*.o
OBJ2 = semantic/*.o
OBJ3 = IR/*.o
OBJ4 = assembly/*.o
TARGET = parser

$(TARGET) :
	cd syntax && make;
	cd semantic && make;
	cd IR && make;
	cd assembly && make;
	g++ -o $(TARGET) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)  -lfl
        
clean :
	cd syntax && make clean;
	cd semantic && make clean;
	cd IR && make clean;
	cd assembly && make clean;
	rm $(TARGET)