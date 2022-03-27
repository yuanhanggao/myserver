CC = g++
CFLAG = -gstabs+ -lpthread

SRC_FILE = $(wildcard src/*.cc) 
INC_FLAG_DIR = -I$(PWD)/include 

TARGET = server

OBJ = $(TARGET).o
OBJ_DIR = obj
OSRC_FILE = $(patsubst %.cc,$(OBJ_DIR)/%.o, $(notdir $(SRC_FILE)))

$(TARGET) : $(OBJ) 
	$(CC) $(OSRC_FILE) -o $(TARGET) $(CFLAG)

$(OBJ) : $(SRC_FILE)
	$(CC) -c $(SRC_FILE) $(INC_FLAG_DIR) $(CFLAG) 
	@mkdir -p $(OBJ_DIR)
	@mv *.o $(OBJ_DIR)

.PHONY : clean

clean: 
	rm -rf $(TARGET) $(OBJ_DIR)
