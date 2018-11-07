OBJ1 = syntax/*.o
TARGET = parser

$(TARGET) :
	cd syntax && make;
	g++ -o $(TARGET) $(OBJ1) -lfl
        
clean :
	cd syntax && make clean;
	rm $(TARGET)