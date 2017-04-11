.PHONY: all clean

CC = gcc
HEADERS = conio.h
TARGET = minesweeper
OBJTS = conio.o minesweeper.o

all : $(TARGET)

%.o : %.c
	$(CC) -c $<

$(TARGET) : $(OBJTS) $(HEADERS)
	$(CC) $(OBJTS) -o $(TARGET)

clean:
	rm $(OBJTS) $(TARGET)
