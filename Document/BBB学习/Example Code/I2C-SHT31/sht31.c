/** Simple I2C example to read the first address of a device in C
* Written by Derek Molloy for the book "Exploring BeagleBone: Tools and 
* Techniques for Building with Embedded Linux" by John Wiley & Sons, 2014
* ISBN 9781118935125. Please see the file README.md in the repository root 
* directory for copyright and GNU GPLv3 license information.            */

#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>

#include <unistd.h>

// Small macro to display value in hexadecimal with 2 places

#define SHT31_ADDR       0x44
#define BUFFER_SIZE 6

unsigned char RXData_Temp[3];
unsigned char RXData_Hum[3];
volatile int TXByteCtr;
volatile int RXByteCtr;

unsigned int temp_sht3x;
volatile float temperatureDegC_sht3x;
volatile float temperatureDegF_sht3x;
int degrees_sht3x;

unsigned int RH_sht3x;
volatile float Relative_Humidity_sht3x;
int humidity_sht3x;

char readBuffer[BUFFER_SIZE];

float temperature_converter_sht3x(void);

float humidity_converter_sht3x(void);

int data_copy(void);

int crc_detect(void);

void display(void);

int main(){
   int file;
   int n;
   char writeBuffer[2] = {0x2C, 0x06};
   
   printf("Starting the SHT31 test application\n");
   if((file=open("/dev/i2c-1", O_RDWR)) < 0){
      perror("failed to open the bus\n");
      return 1;
   }
   if(ioctl(file, I2C_SLAVE, SHT31_ADDR) < 0){
      perror("Failed to connect to the sensor\n");
      return 1;
   }
   
   while (1) {
	   if(write(file, writeBuffer, 2)!=2){
	      perror("Failed to reset the read address\n");
	      return 1;
	   }
	   
	   usleep(16000);
	   
	   if(read(file, readBuffer, BUFFER_SIZE)!=BUFFER_SIZE){
	      perror("Failed to read in the buffer\n");
	      return 1;
	   }
	   
	   for (n = 0; n < BUFFER_SIZE; n++) {
		   printf("The data %d is: 0x%02x\n", n, readBuffer[n]);
	   }
	   
	   data_copy();
	   
	   if (crc_detect()) {
		   printf("sensor data is error\n");
	   }
	   display();
	   
	   sleep(30);
   }
   
   close(file);
   
   return 0;
}

int data_copy(void)
{
	int n, m;
	for (m = 0; m < 6; m++) {
		if (m < 3) 
			RXData_Temp[m] = readBuffer[m];
		else 
			RXData_Hum[m-3] = readBuffer[m];
	}
	
	for (n = 0; n < 6; n++) {
		if (n < 3)
			printf("The Temp data %d is: 0x%02x\n", n, RXData_Temp[n]);
		else
			printf("The Hum data %d is: 0x%02x\n", n-3, RXData_Hum[n-3]);
	}
	
	return 0;
}

// Generator polynomial for CRC
#define POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

static unsigned char SHT3X_CalcCrc(unsigned char data[], unsigned char nbrOfBytes)
{
	unsigned char bit;        // bit mask
	unsigned char crc = 0xFF; // calculated checksum
	unsigned char byteCtr;    // byte counter
  
	// calculates 8-Bit checksum with given polynomial
	for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++) {
		crc ^= (data[byteCtr]);
		for(bit = 8; bit > 0; --bit) {
			if(crc & 0x80)
				crc = (crc << 1) ^ POLYNOMIAL;
			else
				crc = (crc << 1);
			}
	}
  
	return crc;
}

int crc_detect(void)
{
	unsigned char temp_crc, hum_crc;
	temp_crc = SHT3X_CalcCrc(RXData_Temp, 2);
	hum_crc = SHT3X_CalcCrc(RXData_Hum, 2);
	
	if (temp_crc != RXData_Temp[2])
		return 1;
	if (hum_crc != RXData_Hum[2])
		return 2;
	
	return 0;
}

float temperature_converter_sht3x(void)
{
    // Temperature in Celsius. See the Device Descriptor Table section in the
    // System Resets, Interrupts, and Operating Modes, System Control Module
    // chapter in the device user's guide for background information on the
    // used formula.
	temp_sht3x = RXData_Temp[0];
	temp_sht3x= temp_sht3x << 8 | RXData_Temp[1];
	temperatureDegC_sht3x = (long)temp_sht3x / 65535.0f * 175.0f - 45.0f;

    // Temperature in Fahrenheit Tf = (9/5)*Tc + 32
    temperatureDegF_sht3x = temperatureDegC_sht3x * 9.0f / 5.0f + 32.0f;
    return temperatureDegC_sht3x;
}

float humidity_converter_sht3x(void)
{
	RH_sht3x = RXData_Hum[0];
	RH_sht3x= RH_sht3x << 8 | RXData_Hum[1];
	Relative_Humidity_sht3x = (long)RH_sht3x / 65535.0f * 100.0f;
	return Relative_Humidity_sht3x;
}

void display(void)
{
	degrees_sht3x = (int)(temperature_converter_sht3x() + 0.5);
	
	humidity_sht3x = (int)(humidity_converter_sht3x() + 0.5);
	
	printf("The temperature is %d	The humidity is %d\n", degrees_sht3x, humidity_sht3x);
}
