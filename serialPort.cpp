#include "serialPort.h"
#include <vector>
using namespace std;

    int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
                    B38400, B19200, B9600, B4800, B2400, B1200, B300,};

    int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200,  300,
                    38400,  19200,  9600, 4800, 2400, 1200,  300,};  
serialPort::serialPort(string name, int baudrate) : port_name(name), port_baudrate(baudrate)
{
    serial_fd = -1;
}

serialPort::~serialPort()
{
    closePort();
}


void serialPort::set_speed(int fd, int32_t speed)
{
    int   i;
    int   status;
    struct termios   Opt;

    tcgetattr(fd, &Opt);
    for ( i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if  (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0)
            {
                fprintf(stderr, "serial opt error\n");
            }
           	return;
        }
        tcflush(fd, TCIOFLUSH);
    }
}

bool serialPort::set_Parity(int32_t fd, int32_t databits, int32_t stopbits, int32_t parity)
{
    struct termios options;
    if  ( tcgetattr( fd, &options)  !=  0)
    {
        fprintf(stderr, "termios error\n");
        return (false);
    }

    options.c_cflag &= ~CSIZE;
    switch (databits) 
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return (false);
    }

    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;   /* Clear parity enable */
        options.c_iflag &= ~INPCK;     /* Enable parity checking */
        options.c_iflag &= ~(ICRNL | IGNCR);
        options.c_lflag &= ~(ICANON );
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);  
        options.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;     /* Enable parity */
        options.c_cflag &= ~PARODD;   
        options.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return (false);
    }

    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return (false);
    }

    options.c_cc[VTIME] = 10; // 1 seconds
    options.c_cc[VMIN] = 0;
	
    options.c_lflag &= ~(ECHO |ECHOE | ECHOK | ECHONL | ISIG |IEXTEN);
    options.c_iflag &= ~(IXON | IXOFF);
    options.c_oflag &= ~OPOST;
	
    tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        fprintf(stderr, "can not set options\n");
        return (false);
    }
    return (true);
}


int32_t serialPort::openPort(const char *Dev)
{
    extern int errno;
    int	fd = open(Dev, O_RDWR | O_NOCTTY | O_NDELAY);         //| O_NOCTTY | O_NDELAY
    if (fd < 0)
    {        
        fprintf(stderr, "can not open serial port\n");
        cout << "fd: " << fd << " errno: " << errno << endl;
        return -1;
    }
    else
    {
        return fd;
    }
      
}
int32_t serialPort::set_uart()
{
    
	serial_fd = openPort(port_name.c_str());
    if (serial_fd > 0)
    {
        set_speed(serial_fd, port_baudrate);
    }
    else
    {
        fprintf(stderr,"Can't Open Serial Port!\n");
        return -1;
    }

    if (set_Parity(serial_fd, 8, 1, 'N') == false)
    {        
        return -1;
    }
    return 0;
}


void serialPort::closePort()
{
    if (serial_fd > 0)
    {
        /* code */
        close(serial_fd);
    }
    serial_fd = -1;
}

u_int32_t serialPort::readBuffer(u_int8_t *buffer, int size)
{
    return read(serial_fd, buffer, size);
}

u_int32_t serialPort::writeBuffer(u_int8_t *buffer, int size)
{
    return write(serial_fd, buffer, size);
}

u_int8_t serialPort::getChar()
{
    u_int8_t t;
    read(serial_fd, &t, 1);
    return t;
}

u_int32_t serialPort::getLine(u_int8_t *buffer)
{
    uint32_t len = 0;
    uint8_t repeat_time = 0;
    do
    {
        /* code */
        len += readBuffer(&buffer[len], BUF_LEN);
        printf("len:%d buffer[len - 1]:0x%X repeat_time:%d\r\n", len, buffer[len - 1], repeat_time);
        //if(buffer[len - 1] == 0x0D)
        {
            if((len > 2) && (buffer[len -1] == 0x0A))
                break;
        }
        if(buffer[0] == 0x0A)
        {
            len = 0;
            repeat_time = 0;
        }
        repeat_time++;
    } while (repeat_time < 5);
    if (repeat_time < 5)
    {
        /* code */
        return len;
    }
    else
    {
        printf("get wrong data.  len:%d\r\n", len);
        return -1;
    }    
}