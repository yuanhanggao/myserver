CC = g++
CFLAG = -gstabs+ -std=c++11 -lstdc++
LDFLAGS = -lpthread -L./thirdparty/lib -lzlog 		  

TARGET = server
OBJ = $(TARGET).o
OBJ_DIR = obj

SRC_CC_FILE = $(wildcard src/*.cc thirdparty/src/*.cc) 
OSRC_FILE = $(patsubst %.cc, $(OBJ_DIR)/%.o, $(notdir $(SRC_CC_FILE)))

SRC_C_FILE = $(wildcard src/*.c thirdparty/src/*.c) 
OSRC_FILE += $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_C_FILE)))

INC_FLAG_DIR = -Iinclude -Ithirdparty/include


$(TARGET) : $(OBJ) 
	$(CC) $(OSRC_FILE) -o $(TARGET) $(CFLAG) $(LDFLAGS)

$(OBJ) : $(SRC_FILE)
	$(CC) -c $(SRC_CC_FILE) $(SRC_C_FILE) $(INC_FLAG_DIR) $(CFLAG) $(LDFLAGS)
	@mkdir -p $(OBJ_DIR)
	@mv *.o $(OBJ_DIR)

.PHONY : clean

clean: 
	rm -rf $(TARGET) $(OBJ_DIR) *.o ./log/* tags
