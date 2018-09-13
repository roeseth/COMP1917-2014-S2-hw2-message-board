#COMP1917 HW2 Makefile
#Yulun Wu z3486584

CC = gcc
CFLAGS = -Wall

OBJ = hw2
CSRC = $(OBJ).c
HSRC = $(OBJ).h

hw2: $(CSRC) $(HSRC)
	$(CC) $(CFLAGS) -o hw2 $(CSRC)
	
.PHONY: clean
clean: hw2
	rm -f hw2