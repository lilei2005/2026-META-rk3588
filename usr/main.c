#include "stdio.h"
#include "gpiod.h"
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>    // close()函数在这里声明
#include <fcntl.h>     // open()函数和O_*常量在这里声明
#include "serial.h"
#include "serial_manager.h"
#include "ring_buffer.h"
#define gpiolin 100
const char* gpiodev="/dev/gpiochip0";
struct gpiod_line *line;
struct gpiod_chip* gpio1;


int main(int agrc,char* agrv)
{
    int fd;
    Serialconfig config;
    Serialmanager manger;
    gpio1=gpiod_chip_open(gpiodev);
    line=gpiod_chip_get_line(gpio1,gpiolin);
    fd=serial_init(&config,&manger);
    close(fd);
    return 0;
}