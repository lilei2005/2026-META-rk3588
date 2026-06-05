# 交叉编译器与选项
CC      = /home/z/rk3588/orangepi-build/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc
CFLAGS  = -I /home/z/rk3588/orangepi-build/userpatches/overlay/usr/include
LDFLAGS = -L /home/z/rk3588/orangepi-build/userpatches/overlay/usr/lib/ -lgpiod

# 目标可执行文件
TARGET  = main

# 自动查找 usr 目录下所有 .c 文件（可改成你自己的源码根目录）
SRCS   := $(shell find . -name "*.c")
# 同名 .o 文件，保留原目录结构，比如 usr/common/serial.c -> usr/common/serial.o
OBJS   := $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

# 链接：所有 .o 一起链接成最终程序
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# 通用的 .c -> .o 编译规则（自动应用到所有匹配文件）
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清除所有生成文件
clean:
	rm -f $(OBJS) $(TARGET)