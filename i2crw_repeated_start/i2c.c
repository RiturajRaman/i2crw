#include "i2c.h"

#define DEBUG

#ifdef DEBUG
#define DBG printf
#else
#define DBG(format, args...) ((void)0)
#endif

#define HIGHBYTE_REGADDR                 0x08
#define LOWBYTE_REGADDR                  0x09
#define SLAVE_ADDR                              0x64 //7 bit

int sensor_init(void) 
{  
    int i2cfile;    
    char filename[40];
    sprintf(filename,"/dev/i2c-1");
    if ((i2cfile = open(filename,O_RDWR)) < 0) {
        printf(" Failed to open the i2c bus, %s\n", strerror(errno));
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return -1;
        exit(1);
    }
    else
        DBG("I2C port opened\n");

    if (ioctl(i2cfile, I2C_SLAVE, SLAVE_ADDR) < 0) { //address set
        printf("Failed to acquire bus access and/or talk to slave, %s\n", strerror(errno));
        return -1;
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
    else { 
        DBG("I2C bus acquired\n");
        return i2cfile;
    }
}

unsigned char read_reg(unsigned char reg)
{
    int reg_val;
    int file;
    file = sensor_init();
    if (file == -1)
    {
            printf("Error in sensor init\n");
            return -1;
    }
    else
    {
        DBG("Sensor init done\n");
    }
    reg_val = i2c_smbus_read_byte_data(file, reg);

    close(file);
    return reg_val;
}

unsigned char write_reg(unsigned char reg, unsigned char val_to_be_written)
{
    int temp;
    int file;
    file = sensor_init();
    if (file == -1)
    {
            printf("Error in sensor init\n");
            return -1;
    }
    else
    {
        DBG("Sensor init done\n");
    }
    temp = i2c_smbus_write_byte_data(file, reg, val_to_be_written);
     if (temp == -1) 
    {
             // ERROR HANDLING: i2c transaction failed 
            printf("Failed to write to the i2c bus, %s\n", strerror(errno));
            return -1;
        } 
        else 
        {
            DBG("Interrupt Disable Success\n");
        }
    
    close(file);
    return temp;
}

