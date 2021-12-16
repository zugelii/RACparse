// RACparse.cpp: 定义应用程序的入口点。
//

#include "RACparse.h"
#include "TinyGPS++.h"
#include "serialPort.h"
#include <atomic>
#include <csignal>
#include <fstream>
#include <thread>
using namespace std;
using namespace std::chrono_literals;
#define  SERIAL_PORT_DEV 	   "/dev/ttyUSB0"   //USART1
/*
const char* gpsStream =
"$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
"$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.1,211.6,M,-22.5,M,,14,0000*48\r\n"
"$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
"$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,0.3,200.8,M,-22.5,M,,40,0000*44\r\n"
"$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
"$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,0.7,206.9,M,-22.5,M,,74,0000*45\r\n";
*/

const char* gpsStream =
"$GNGGA,023127.10,3111.7739760,N,12126.1486579,E,2,12,0.48,15.43,M,0.000,M,0.00,0000*68\r\n"
"$GPRMC,023127.20,A,3111.7739780,N,12126.1486609,E,0.00,0.000,141221,-163.,0,D*6B\r\n"
"$GNGGA,023127.20,3111.7739780,N,12126.1486609,E,2,12,0.48,15.50,M,0.000,M,0.00,0000*63\r\n"
"$GNGGA,023127.30,3111.7739780,N,12126.1486609,E,2,12,0.48,15.50,M,0.000,M,0.00,0000*62\r\n"
"$GNGGA,023127.40,3111.7739799,N,12126.1486550,E,2,12,0.48,15.50,M,0.000,M,0.00,0000*62\r\n"
"$GNGGA,023127.50,3111.7739799,N,12126.1486550,E,2,12,0.48,15.50,M,0.000,M,0.00,0000*63\r\n";
TinyGPSPlus gps;
TinyGPSCustom pdop(gps, "GPGSA", 15); // $GPGSA sentence, 15th element
TinyGPSCustom hdop(gps, "GPGSA", 16); // $GPGSA sentence, 16th element
TinyGPSCustom vdop(gps, "GPGSA", 17); // $GPGSA sentence, 17th element
void displayInfo()
{
    printf("Location: ");
    if (gps.location.isValid())
    {
        printf("%.6f", gps.location.lat());
        printf(",");
        printf("%.6f", gps.location.lng());
    }
    else
    {
        printf("INVALID");
    }

    printf("  Date/Time: ");
    if (gps.date.isValid())
    {
        printf("%d", gps.date.month());
        printf("/");
        printf("%d", gps.date.day());
        printf("/");
        printf("%d", gps.date.year());
    }
    else
    {
        printf("INVALID");
    }

    printf(" ");
    if (gps.time.isValid())
    {
        if (gps.time.hour() < 10) printf("0");
        printf("%d", gps.time.hour());
        printf(":");
        if (gps.time.minute() < 10) printf("0");
        printf("%d", gps.time.minute());
        printf(":");
        if (gps.time.second() < 10) printf("0");
        printf("%d", gps.time.second());
        printf(".");
        if (gps.time.centisecond() < 10) printf("0");
        printf("%d", gps.time.centisecond());
    }
    else
    {
        printf("INVALID");
    }
    printf(" precision: ");
    if (gps.precision.isValid())
    {
        printf("%f", gps.precision.precision());
    }
    else
    {
        printf("INVALID");
    }

    printf(" ins time: ");
    if (gps.instime.isValid())
    {
        printf("%d ", gps.instime.value());
    }
    else
    {
        printf("INVALID");
    }
    printf("\n");
}
std::atomic<bool> main_thread_run{ true };
#if 1
void on_signal_int(int signum)
{
    (void)signum;
    main_thread_run = false;
}
void thread_read_rac(string port_name, int baud)
{
    ofstream gsensor_log;
    string file_name;
    file_name.append(port_name);
    file_name.append(".txt");
    gsensor_log.open(file_name, ios::out);
    // printf("thread port_name:%s baud:%d\r\n", port_name.c_str(), baud);
    string pname;
    pname.append("/dev/");
    pname.append(port_name);
    serialPort mySerial(pname, baud);
    if (mySerial.set_uart() < 0)
    {
        /* code */
        printf("open serial:%s error\n", port_name.c_str());
        return;
    }
    uint8_t buf[BUF_LEN];
    int read_len;
    while (main_thread_run)
    {
        /* code */
        memset(buf, 0, BUF_LEN);
        read_len = mySerial.readBuffer(buf, BUF_LEN);
        if (read_len > 0)
        {
            gsensor_log << buf;
            //cout << buf << endl;
            //display RAC data
            for (size_t i = 0; i < read_len; i++)
            {
                if (gps.encode((char)buf[i]))
                {
                    displayInfo();
                }
            }
        }
    }
    gsensor_log.close();
}

#endif
int main()
{
    std::cout << "gps data \n";
    while (*gpsStream)
        if (gps.encode(*gpsStream++))
            displayInfo();
	return 0;
}

/*
int main(int argc, char** argv)
{
    signal(SIGINT, on_signal_int);
    string port_name(SERIAL_PORT_DEV);
    int baud = 115200;
    std::cout << argv[1] << " " << argv[2] << endl;
    if (argc >= 3)
    {
        port_name.clear();
        port_name.append(argv[1]);
        baud = std::stoi(argv[2]);
    }
    else
    {
        printf("please add the port name\r\n");
        exit(1);
    }
    auto my_thread = thread{ thread_read_rac, port_name, baud };

    while (main_thread_run)
    {
        std::this_thread::sleep_for(1000ms);
    }
    if (my_thread.joinable())
    {
        my_thread.join();
    }
    cout << "RAC parse exit\n";
}

*/