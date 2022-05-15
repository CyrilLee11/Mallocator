CC=gcc
TARGET=main
INCLUDE_PATH=-I ./include
SRC_C=./src/*.c

$(TARGET):$(SRC_C)
	$(CC) main.c $^ -o $@ $(INCLUDE_PATH)
	@ rm -rf ./src/*.o

clean:
	rm -rf $(TARGET) 


