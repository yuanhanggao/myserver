CC = g++
CFLAG = -gstabs+
LDFLAGS = -lpthread -L./thirdparty/lib -lzlog
		  

PWD = $(shell pwd)
SRC_FILE = $(wildcard src/*.cc) 
INC_FLAG_DIR = -I$(PWD)/include -I$(PWD)/thirdparty/include

TARGET = server

OBJ = $(TARGET).o
OBJ_DIR = obj
OSRC_FILE = $(patsubst %.cc, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILE)))

$(TARGET) : $(OBJ) 
	$(CC) $(OSRC_FILE) -o $(TARGET) $(CFLAG) $(LDFLAGS)

$(OBJ) : $(SRC_FILE)
	$(CC) -c $(SRC_FILE) $(INC_FLAG_DIR) $(CFLAG) $(LDFLAGS)
	@mkdir -p $(OBJ_DIR)
	@mv *.o $(OBJ_DIR)

.PHONY : clean

clean: 
	rm -rf $(TARGET) $(OBJ_DIR) *.o ./log/*
	
