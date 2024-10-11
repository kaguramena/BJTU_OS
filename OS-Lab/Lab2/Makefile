.PHONY: all clean
CC = gcc
CFLAGS = -Wall -g  # 常用编译选项，可以根据需要调整
TARGET = pro-con rw-reader rw-writer Dining_philosopher

pro-con:Pro-Con.c
	$(CC) $(CFLAGS) -o $@ $< -lpthread

rw-reader:rw-reader.c
	$(CC) $(CFLAGS) -o $@ $< -lpthread

rw-writer:rw-writer.c
	$(CC) $(CFLAGS) -o $@ $< -lpthread

Dining_philosopher:Dining_philosopher.c
	$(CC) $(CFLAGS) -o $@ $< -lpthread

all: $(TARGET)


clean:
	rm -f $(TARGET)