/*
 * MPU_Control.c
 *
 * Created: 11/22/2013 11:46:39 AM
 *  Author: Jake Simon
 *	
 *	This code is for use with MPU6050 3-axis accelerometer and 3-axis gyroscope
 *   Requires the MPU_Control.h file
 */

/*INLCUDES*/
#include <avr/io.h>  //do I need this?
#include "MPU_Control.h"

void MPU_Selftest (void)
{
	
}

void MPU_Enable (void)
{
	unsigned char messageBuf[MESSAGEBUF_SIZE];
	unsigned char TWI_targetSlaveAddress = 0x68;
	unsigned char temp;
	


	//**************Set up MPU6050***********//
	//power management. wake up chip
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address. if False have write
	messageBuf[1] = 0x6B;             //
	messageBuf[2] = 0x00;      //
	//check temp for errors?
	temp = USI_TWI_Start_Transceiver_With_Data( messageBuf, 3);
	
	//used to compute sample rate. make sample rate 75 Hz
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address. if False have write
	messageBuf[1] = 0x19;             // 
	messageBuf[2] = 0x0D;      // 
	//check temp for errors?
	temp = USI_TWI_Start_Transceiver_With_Data( messageBuf, 3);
	
	//digital low pass filter.
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address. if False have write
	messageBuf[1] = 0x1A;             // 
	messageBuf[2] = 0x06;      // 
	//check temp for errors?
	temp = USI_TWI_Start_Transceiver_With_Data( messageBuf, 3);
	
	//sensitivity. set to 16g
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address. if False have write
	messageBuf[1] = 0x1C;             //
	messageBuf[2] = 0x18;      //
	//check temp for errors?
	temp = USI_TWI_Start_Transceiver_With_Data( messageBuf, 3);
}

//reads from MPU6050. First argument holds acc data, second argument holds gyro data
void MPU_Read (unsigned char *data_a, unsigned char *data_g) 
{
	unsigned char TWI_targetSlaveAddress = 0x68;
	unsigned char temp;

	//Tell the MPU6050 where to read data. acc x higher bit. read all acc data
	data_a[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address.
	data_a[1] = 0x3b;			
	//check temp for errors?
	temp = USI_TWI_Start_Transceiver_With_Data(data_a, 2);
	//read
	data_a[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address.
	temp = USI_TWI_Start_Transceiver_With_Data(data_a, 7);
	
	//Tell the MPU6050 where to read data. gyro x higher bit. read all gyro data
	data_g[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address.
	data_g[1] = 0x43;			
	//check temp for errors?
	temp = USI_TWI_Start_Transceiver_With_Data(data_g, 2);
	//read
	data_g[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address.
	temp = USI_TWI_Start_Transceiver_With_Data(data_g, 7);

}
