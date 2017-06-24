// Generic i2c test program

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#define DEBUG 

#ifdef DEBUG
	#define DBG printf
#else
	#define DBG 
#endif

// I2C Linux device handle
int g_i2cFile;

// open the Linux device
void i2cOpen()
{
	g_i2cFile = open("/dev/i2c-1", O_RDWR);
	if (g_i2cFile < 0) {
		perror("i2cOpen");
		exit(1);
	}
}

// close the Linux device
void i2cClose()
{
	close(g_i2cFile);
}

// set the I2C slave address for all subsequent I2C device transfers
void i2cSetAddress(int address)
{
            DBG ("address is %02x\n", address);
            DBG ("File = %d\n", g_i2cFile);
            int open_attempt = ioctl(g_i2cFile, I2C_SLAVE, address);
	if (open_attempt < 0) {
		//perror("i2cSetAddress");
                          printf("Failed to acquire bus access and/or talk to slave, %s\n", strerror(errno));
		//exit(1);
	}
}

// write a 16 bit value to a register pair
// write low byte of value to register reg,
// and high byte of value to register reg+1
int pca9555WriteRegisterPair(uint8_t reg, uint16_t value)
{
	uint8_t data[2];
	data[0] = reg;
	data[1] = value & 0xff;
	//data[2] = (value >> 8) & 0xff;
	if (write(g_i2cFile, data, 2) != 2) {
		perror("pca9555SetRegisterPair");
		return -1;
	}
	return 0;
}

// read a 16 bit value from a register pair
uint8_t pca9555ReadRegisterPair(uint8_t reg)
{
	uint8_t data[2];
	data[0] = reg;
	if (write(g_i2cFile, data, 1) != 1) {
		perror("pca9555ReadRegisterPair set register");
	}
	if (read(g_i2cFile, data, 2) != 2) {
		perror("pca9555ReadRegisterPair read value");
	}
	//return data[0]; //| (data[1] << 8);
	printf ("Read values: %02x %02x\n", data[0], data[1]);
	return (data[0] | (data[1] << 8));
}

// set IO ports to input, if the corresponding direction bit is 1,
// otherwise set it to output
void pca9555SetInputDirection(uint16_t direction)
{
	pca9555WriteRegisterPair(6, direction);
}

// set the IO port outputs
void pca9555SetOutput(uint16_t value)
{
	pca9555WriteRegisterPair(2, value);
}

// read the IO port inputs
uint16_t pca9555GetInput()
{
	return pca9555ReadRegisterPair(0);
}


int main(int argc, char** argv)
{
	DBG("In main\n");
	
	if (argc != 4 && argc != 5)
	{
		DBG ("argc = %d\n", argc);
		printf ("Usage: ./i2crw <slave_address> <0: write, 1: read> <register_address> <value to be written(optional)>\n");
		return -1;
	}

	unsigned char slave_address = strtol(argv[1], NULL, 16);
	int rw_flag = strtol(argv[2], NULL, 16);
	if ( (rw_flag == 0 && argc != 5) || (rw_flag == 1 && argc != 4) )
	{
		DBG ("argc = %d, rw_flag = %d\n", argc, rw_flag);
		printf ("Usage: ./i2crw <slave_address> <0: write, 1: read> <register_address> <value to be written(optional)>\n");
		return -1;
	}
	int register_address = strtol(argv[3], NULL, 16);
	int write_val, read_val;

	// open Linux I2C device
	i2cOpen();

	// set slave address of the I2C device
	i2cSetAddress(slave_address);

	// set input for IO pin 15, rest output
	pca9555SetInputDirection(1 << 15);

	if (rw_flag == 0) //write to register
	{
		write_val = strtol(argv[4], NULL, 16);
		pca9555WriteRegisterPair(register_address, write_val);
	}

	else if (rw_flag == 1) //read from register
	{
		read_val = pca9555ReadRegisterPair(register_address);
		printf ("Reg: 0x%02x; value: 0x%02x (%ld in decimal)\n", register_address, read_val, read_val);

	}
	
	i2cClose();

	return 0;
}
