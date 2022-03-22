CC = g++
CFLAG = -gstabs+ -lpthread
PWD = $(shell pwd)
SRC = $(shell find ./ -name "*.cc" | grep -v "test") 
INC = -I$(PWD) 
PROM = server
OBJ = $(PROM).o
OSRC = $(shell find ./ -name "*.o")

$(PROM) : $(OBJ) 
	$(CC) $(OSRC) -o $(PROM) $(CFLAG)

$(OBJ) : $(SRC)
	$(CC) -c $(SRC) $(INC) $(CFLAG)  
clean: 
	rm -rf $(PROM) *.o
