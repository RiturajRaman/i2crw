#include "i2c.h"

int main(int argc, char** argv)
{
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

    if (rw_flag == 0) //write to register
    {
        write_val = strtol(argv[4], NULL, 16);
        write_reg(register_address, write_val);
    }

    else if (rw_flag == 1) //read from register
    {
        read_val = read_reg(register_address);
        printf ("Reg: 0x%02x; value: 0x%02x (%ld in decimal)\n", register_address, read_val, read_val);

    }
    
    return 0;
}