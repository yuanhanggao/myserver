CC = g++
CFLAG = -g3 
PWD = $(shell pwd)
SRC = $(shell find ./ -name "*.cc") 
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