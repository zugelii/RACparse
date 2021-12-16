#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/serial.h>
#include <sys/ioctl.h>

#include <string>
#include <iostream>

using namespace std;
#define BUF_LEN 1000
class serialPort
{
private:
    /* data */
    string port_name;
    int port_baudrate;
  
private:
    void set_speed(int fd, int32_t speed);
    bool set_Parity(int fd, int32_t databits, int32_t stopbits, int32_t parity);
    int32_t openPort(const char *Dev);  
    void closePort();
public:
    int serial_fd;
    serialPort(string name, int baudrate);    
    int32_t set_uart();  
    u_int32_t readBuffer(u_int8_t *buffer, int size);
    u_int32_t writeBuffer(u_int8_t *buffer, int size);
    u_int8_t getChar();
    u_int32_t getLine(u_int8_t *buffer);
    ~serialPort();
};


