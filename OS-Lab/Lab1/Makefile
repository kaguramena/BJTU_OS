# 编译器和编译选项
CC = gcc
CFLAGS = -Wall -g  # 常用编译选项，可以根据需要调整

# 源文件列表，注意源文件在子目录中
SRCS = shell.c cd/cd.c ls/ls.c mkdir/mkdir.c pstree/pstree.c echo/echo.c

# 对应的目标文件列表，目标文件与子目录同级
OBJS = shell.o cd.o ls.o mkdir.o pstree.o echo.o

# 默认目标
all: shell

# 规则：编译生成可执行文件
shell: $(OBJS)
	$(CC) -o $@ $(OBJS)

# 自动推导规则：编译 .c 文件生成 .o 文件，.c 文件在子目录，.o 在当前目录
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 手动规则：针对子目录中的 .c 文件，生成当前目录的 .o 文件
cd.o: cd/cd.c
	$(CC) $(CFLAGS) -c $< -o $@

ls.o: ls/ls.c
	$(CC) $(CFLAGS) -c $< -o $@

mkdir.o: mkdir/mkdir.c
	$(CC) $(CFLAGS) -c $< -o $@

pstree.o: pstree/pstree.c
	$(CC) $(CFLAGS) -c $< -o $@

echo.o: echo/echo.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理目标
clean:
	rm -f $(OBJS) shell

# 声明伪目标
.PHONY: all clean